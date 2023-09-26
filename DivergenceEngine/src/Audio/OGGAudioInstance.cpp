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

//https://xiph.org/vorbis/doc/vorbisfile/overview.html
//https://github.com/edubart/minivorbis

namespace DivergenceEngine
{
	OGGAudioInstance::OGGAudioInstance(DirectX::AudioEngine* engine, std::wstring filePath, uint8_t initialPlaybackSpeedMultiplier, float initialVolume)
	{
		//Handle invalid parameters
		if (engine == nullptr)
		{
			throw std::invalid_argument("OGGAudioInstance::OGGAudioInstance() - engine cannot be nullptr");
		}
		EnginePointer = engine;

		if (initialPlaybackSpeedMultiplier == 0)
		{
			throw std::invalid_argument("OGGAudioInstance::OGGAudioInstance() - initialPlaybackSpeedMultiplier cannot be 0");
		}
		PlaybackSpeedMultiplier = initialPlaybackSpeedMultiplier;

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
				VorbisInfo->rate * PlaybackSpeedMultiplier,
				VorbisInfo->channels,
				BIT_DEPTH
			);

		//Set the volume
		SoundEffectInstance->SetVolume(initialVolume);

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

		//Wait for thread to close
		BankLoadingThreadObject.join();

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
		ov_pcm_seek(&VorbisFileObject, 0);
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

	void OGGAudioInstance::SetPlaybackSpeedMultiplier(uint8_t newPlaybackSpeedMultiplier)
	{
		//Ensure that the new playback speed multiplier is not 0
		if (newPlaybackSpeedMultiplier == 0)
		{
			throw std::invalid_argument("OGGAudioInstance::SetPlaybackSpeedMultiplier() - newPlaybackSpeedMultiplier cannot be 0");
		}

		//If the new playback speed is unchanged, do nothing
		if (PlaybackSpeedMultiplier == newPlaybackSpeedMultiplier)
		{
			return;
		}

		//Check if the song is already playing and if it was looping
		bool isPlaying = SoundEffectInstance->GetState() == DirectX::SoundState::PLAYING;

		//Reset the instance with the altered sample rate based on the new playback speed multiplier
		this->Pause();
		PlaybackSpeedMultiplier = newPlaybackSpeedMultiplier;
		SoundEffectInstance = std::make_unique<DirectX::DynamicSoundEffectInstance>
			(
				EnginePointer,
				std::bind(&OGGAudioInstance::BufferNeeded, this, std::placeholders::_1),
				VorbisInfo->rate * PlaybackSpeedMultiplier,
				VorbisInfo->channels,
				BIT_DEPTH
			);
		this->Play(IsLoop);

		if (!isPlaying)
		{
			this->Pause();
		}
	}

	void OGGAudioInstance::BufferNeeded(DirectX::DynamicSoundEffectInstance* instance)
	{
		//Lock the current bank
		std::unique_lock<std::mutex> lock(BankMutexArray[CurrentBankIndex]);

		//If the bank has 0 size, the file must be over
		if (TrueBankSizeArray[CurrentBankIndex] == 0)
		{
			StopLoadingBuffers = true;
		}

		while (!StopLoadingBuffers && instance->GetState() == DirectX::PLAYING && instance->GetPendingBufferCount() <= MAX_BUFFERS)
		{
			//Submit the next buffer
			long bufferSize = std::min(MAX_BUFFER_SIZE, TrueBankSizeArray[CurrentBankIndex] - CurrentBankDataIndex);
			instance->SubmitBuffer(reinterpret_cast<uint8_t*>(&BankArray[CurrentBankIndex][CurrentBankDataIndex]), bufferSize);
			CurrentBankDataIndex += bufferSize;

			//If we are at the end of the bank, load the next bank in the currently expired bank and increase the bank index
			if (CurrentBankDataIndex >= TrueBankSizeArray[CurrentBankIndex])
			{
				BankLoadEventArray[CurrentBankIndex] = true;

				CurrentBankIndex = (CurrentBankIndex + 1) % NUMBER_OF_BANKS;
				lock = std::unique_lock<std::mutex>(BankMutexArray[CurrentBankIndex]);

				CurrentBankDataIndex = 0;
			}
		}

		if (StopLoadingBuffers && instance->GetPendingBufferCount() == 0)
		{
			this->Stop();
		}
	}

	//https://github.com/dougbinks/enkiTS/issues/11
	void OGGAudioInstance::BankLoadingThread()
	{
		while (true)
		{
			//Search for a signal from the array of event booleans and get the index for the signal code
			bool searchingForSignal = true;
			uint32_t signalCode = THREAD_EXIT_EVENT_INDEX;
			while (searchingForSignal)
			{
				Sleep(500);

				for (uint32_t index = 0; index < NUMBER_OF_EVENTS; index++)
				{
					if (BankLoadEventArray[index])
					{
						signalCode = index;
						BankLoadEventArray[index] = false;
						searchingForSignal = false;
						break;
					}
				}
			}

			//If the thread is signaled to exit, exit
			if (signalCode == THREAD_EXIT_EVENT_INDEX)
			{
				break;
			}

			//If it has made it here, the signal is to load a new bank
			LoadBank(signalCode);
		}

		ThreadIsRunning = false;
	}

	void OGGAudioInstance::LoadBank(uint32_t bankIndex)
	{
		//Lock the bank
		std::lock_guard<std::mutex> lock(BankMutexArray[bankIndex]);
		Logger::Log(std::format(L"Begin loading bank {}", bankIndex));

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
		}
		Logger::Log(std::format(L"Finished loading bank {}", bankIndex));
	}
}