#define NOMINMAX
#include "Audio/IAudioInstance.h"
#include "Audio/OGGAudioInstance.h"
#include "Logger/Logger.h"
#include <algorithm>
#include <stdexcept>
#include <stdio.h>
#include <format>
#include <thread>
#include <exception>
#include <StringConverter.h>

//https://xiph.org/vorbis/doc/vorbisfile/overview.html
//https://github.com/edubart/minivorbis

namespace DivergenceEngine
{
	OGGAudioInstance::OGGAudioInstance(DirectX::AudioEngine* engine, std::wstring filePath, PlaybackSpeed initialPlaybackSpeed, float initialVolume)
	{
		//Handle invalid parameters
		if (engine == nullptr)
		{
			throw std::invalid_argument("OGGAudioInstance::OGGAudioInstance() - engine cannot be nullptr");
		}
		EnginePointer = engine;

		if (initialVolume > 1 || initialVolume < 0)
		{
			throw std::invalid_argument("OGGAudioInstance::OGGAudioInstance() - initialVolume must be between 0 and 1");
		}

		//Try to open the file C style
		FILE* fileObjectPointer = nullptr;
		errno_t fileError = _wfopen_s(&fileObjectPointer, filePath.c_str(), L"rb");
		if (fileError != 0)
		{
			throw std::invalid_argument("OGGAudioInstance::OGGAudioInstance() - filePath cannot be opened");
		}
		FilePath = filePath;

		//Open the file as a Vorbis file
		int vorbisError = ov_open_callbacks(fileObjectPointer, &VorbisFileObject, nullptr, 0, OV_CALLBACKS_DEFAULT);
		if(vorbisError != 0)
		{
			throw std::invalid_argument("OGGAudioInstance::OGGAudioInstance() - filePath is not a valid Vorbis file");
		}

		//Ensure that it only had one logical stream
		if (ov_streams(&VorbisFileObject) != 1)
		{
			throw std::invalid_argument("OGGAudioInstance::OGGAudioInstance() - filePath has more than one logical stream");
		}

		//Ensure that the file is seekable
		if (ov_seekable(&VorbisFileObject) == 0)
		{
			throw std::invalid_argument("OGGAudioInstance::OGGAudioInstance() - filePath is not seekable");
		}

		//Get the file information
		VorbisInfo = ov_info(&VorbisFileObject, -1);

		//Get the total number of PCM samples
		TotalSamples = ov_pcm_total(&VorbisFileObject, -1);

		//Calculate the block align
		BlockAlign = VorbisInfo->channels * BIT_DEPTH / 8;

		//Create the sound instance
		SoundEffectInstance = std::make_unique<DirectX::DynamicSoundEffectInstance>
			(
				EnginePointer,
				std::bind(&OGGAudioInstance::BufferNeeded, this, std::placeholders::_1),
				VorbisInfo->rate * 2, //This is multiplied by two to make 1x, 2x, 4x speedup possible by just pitching using DXTK function (this will 2x the speed by default, making -1.0 pitch 1x and +1.0 4 speed)
				VorbisInfo->channels,
				BIT_DEPTH
			);

		//Set the volume
		SoundEffectInstance->SetVolume(initialVolume);

		//Set the playback speed
		CurrentPlaybackSpeed = initialPlaybackSpeed;
		SetPlaybackSpeed(CurrentPlaybackSpeed);

		//Initialize bank event
		BankLoadEventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (BankLoadEventHandle == nullptr)
		{
			throw std::runtime_error("OGGAudioInstance::OGGAudioInstance() - Failed to create bank load event");
		}

		//Load all banks
		for (uint32_t index = 0; index < NUMBER_OF_BANKS; index++)
		{
			LoadBank(index);
		}

		//Initialize the events
		for (uint32_t index = 0; index < NUMBER_OF_EVENTS; index++)
		{
			BankLoadEventArray[index] = false;
		}

		//Start the thread
		ThreadIsRunning = true;
		BankLoadingThreadObject = std::thread(&OGGAudioInstance::BankLoadingThread, this);

		Logger::Log(std::format(L"Loaded {}", FilePath));
	}

	OGGAudioInstance::~OGGAudioInstance()
	{
		SoundEffectInstance->Pause();

		//Signal the thread to be closed
		BankLoadEventArray[THREAD_EXIT_EVENT_INDEX] = true;
		SetEvent(BankLoadEventHandle);

		//Wait for thread to close
		BankLoadingThreadObject.join();

		//Close up the bank loading event
		CloseHandle(BankLoadEventHandle);

		ov_clear(&VorbisFileObject);
		Logger::Log(std::format(L"Destroyed {}", FilePath));
	}

	void OGGAudioInstance::Play(bool isLoop)
	{
		IsLoop = isLoop;
		SoundEffectInstance->Play();
	}

	void OGGAudioInstance::Stop()
	{
		SoundEffectInstance->Stop();
		RestartRequested = true;
		SetEvent(BankLoadEventHandle);
	}

	void OGGAudioInstance::Pause()
	{
		SoundEffectInstance->Pause();
	}

	void OGGAudioInstance::Resume()
	{
		SoundEffectInstance->Resume();
	}

	void OGGAudioInstance::SetVolume(float volume)
	{
		if (volume > 1 || volume < 0)
		{
			throw std::invalid_argument("WAVAudioInstance::SetVolume() - volume must be between 0 and 1");
		}

		SoundEffectInstance->SetVolume(volume);
	}

	void OGGAudioInstance::SetPlaybackSpeed(PlaybackSpeed newPlaybackSpeed)
	{
		CurrentPlaybackSpeed = newPlaybackSpeed;

		if (newPlaybackSpeed == PlaybackSpeed::Normal)
		{
			SoundEffectInstance->SetPitch(-1.0f);
		}
		else if (newPlaybackSpeed == PlaybackSpeed::Double)
		{
			SoundEffectInstance->SetPitch(0.0f);
		}
		else if (newPlaybackSpeed == PlaybackSpeed::Quadruple)
		{
			SoundEffectInstance->SetPitch(1.0f);
		}
	}

	void OGGAudioInstance::BufferNeeded(DirectX::DynamicSoundEffectInstance* instance)
	{
		//Check if the banks have been refreshed, if so, reset the current bank index and data index
		if (BanksRefreshed)
		{
			BanksRefreshed = false;
			CurrentBankIndex = 0;
			CurrentBankDataIndex = 0;
			StopLoadingBuffers = false;
		}

		//Calculate max buffer size the same way as done in WAVs using ((byteRate*128) / (MAX_NUM_BANKS*2205))*PlaybackSpeed
		long pcmByteRate = BlockAlign * VorbisInfo->rate;
		long targetBufferSize = ((128 * pcmByteRate) / (2205 * MAX_BUFFERS)) * static_cast<uint32_t>(CurrentPlaybackSpeed.load());

		//If the current bank is signaled for refill, return early as it is not time to use the bank
		if (BankLoadEventArray[CurrentBankIndex])
		{
			return;
		}

		while (!StopLoadingBuffers && instance->GetState() == DirectX::PLAYING && instance->GetPendingBufferCount() <= MAX_BUFFERS)
		{
			//If the bank has 0 size, the file must be over
			if (TrueBankSizeArray[CurrentBankIndex] == 0)
			{
				StopLoadingBuffers = true;
				break;
			}

			//Submit the next buffer
			long bufferSize = std::min(targetBufferSize, TrueBankSizeArray[CurrentBankIndex] - CurrentBankDataIndex);
			instance->SubmitBuffer(reinterpret_cast<uint8_t*>(&BankArray[CurrentBankIndex][CurrentBankDataIndex]), bufferSize);
			CurrentBankDataIndex += bufferSize;

			//If we are at the end of the bank, load the next bank in the currently expired bank and increase the bank index
			if (CurrentBankDataIndex >= TrueBankSizeArray[CurrentBankIndex])
			{
				BankLoadEventArray[CurrentBankIndex] = true;

				CurrentBankIndex = (CurrentBankIndex + 1) % NUMBER_OF_BANKS;
				CurrentBankDataIndex = 0;

				//If the current bank is signaled for refill, return early as it is not time to use the bank
				if (BankLoadEventArray[CurrentBankIndex])
				{
					return;
				}
			}
		}

		if (StopLoadingBuffers && instance->GetPendingBufferCount() == 0)
		{
			this->Stop();
		}
	}

	//https://github.com/dougbinks/enkiTS/issues/11
	//Consider replacing atomic bools with either semaphores or manual reset events
	void OGGAudioInstance::BankLoadingThread()
	{
		//Removes thread priority boost so it doesn't preempt the audio thread and cause crackling.
		SetThreadPriorityBoost(GetCurrentThread(), TRUE);

		//Set the thread priority to below normal so it doesn't preempt the audio thread and cause crackling.
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);

		while (true)
		{
			//Wait for event to be signaled that bank needs to be loaded (or 15 ms since the event doesn't get set in the BufferNeeded function to avoid crackling)
			WaitForSingleObject(BankLoadEventHandle, 500);

			//If the thread is signaled to exit, exit
			if (BankLoadEventArray[THREAD_EXIT_EVENT_INDEX])
			{
				break;
			}

			//If a restart to the audio has been requested, seek to the beginning and have all banks refreshed
			if (RestartRequested)
			{
				RestartRequested = false;

				//Mark all banks as requiring a refill
				for (size_t index = 0; index < NUMBER_OF_BANKS; index++)
				{
					BankLoadEventArray[index] = true;
				}

				//Seek to the beginning of the file
				ov_pcm_seek(&VorbisFileObject, 0);

				//Load all banks
				for (size_t index = 0; index < NUMBER_OF_BANKS; index++)
				{
					LoadBank(index);
				}

				//Mark all banks filled
				for (size_t index = 0; index < NUMBER_OF_BANKS; index++)
				{
					BankLoadEventArray[index] = false;
				}

				//Signal to the audio thread that the banks have been refreshed
				BanksRefreshed = true;
			}

			//Refresh all banks that have been signaled for refill
			for (size_t index = 0; index < NUMBER_OF_BANKS; index++)
			{
				if (BankLoadEventArray[index])
				{
					LoadBank(index);
					BankLoadEventArray[index] = false;
				}
			}
		}

		ThreadIsRunning = false;
	}

	void OGGAudioInstance::LoadBank(uint32_t bankIndex)
	{
		//NOTE: This used to have a mutex to prevent reads from audio thread while writing here. This is handled by the BankLoadEventArray now outside of this function so no synchronization required here anymore as it is handled elsewhere

		//Fill the bank until it is either full or the file is finished and loop is disabled
		TrueBankSizeArray[bankIndex] = 0;
		while (TrueBankSizeArray[bankIndex] < MAX_BANK_SIZE)
		{
			long currentBytesRead = ov_read(&VorbisFileObject, reinterpret_cast<char*>(&BankArray[bankIndex][TrueBankSizeArray[bankIndex]]), MAX_BANK_SIZE - TrueBankSizeArray[bankIndex], 0, BIT_DEPTH / 8, 1, nullptr);

			//If the read results in a 0 return, then the end of the file has been reached
			if (currentBytesRead == 0)
			{
				//If the song is looping, seek to the beginning of the file
				if (IsLoop)
				{
					ov_pcm_seek(&VorbisFileObject, 0);
				}

				else
				{
					return;
				}
			}
			
			else if (currentBytesRead > 0)
			{
				TrueBankSizeArray[bankIndex] += currentBytesRead;
			}

			else if (currentBytesRead == OV_HOLE)
			{
				Logger::Log(std::format(L"OV_HOLE error in '{}', continuing...", FilePath));
				continue;
			}

			// OV_EBADLINK, OV_EINVAL, OV_EREAD, OV_EFAULT…
			else
			{
				//The file is likely corrupted and unable to decode
				std::wstring message = std::format(L"Failed to decode audio file '{}' (Vorbis error {})", FilePath, currentBytesRead);
				Logger::Log(message);

				//Displays message box letting user know the error (If this was on the main thread it would have shown a window as it went up the stack but, this stack doesn't lead to main)
				MessageBoxW(nullptr, message.c_str(), L"Audio Error", MB_OK | MB_ICONERROR);

				//Crash or allow upper layer to catch
				throw std::runtime_error(StringConverter::ConvertWideStringToANSI(message));
			}
		}
		//Logger::Log(std::format(L"Finished loading bank {}", bankIndex));
	}
}