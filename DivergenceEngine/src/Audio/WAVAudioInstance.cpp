#define NOMINMAX
#include "WAVAudioInstance.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include "StringConverter.h"
#include "Logger/Logger.h"

namespace fs = std::filesystem;

namespace DivergenceEngine
{
	WAVAudioInstance::WAVAudioInstance(DirectX::AudioEngine* engine, std::wstring filePath, uint8_t initialPlaybackSpeedMultiplier, float initialVolume)
	{
		//Handle invalid parameters
		if (engine == nullptr)
		{
			throw std::invalid_argument("WAVAudioInstance::WAVAudioInstance() - engine cannot be nullptr");
		}
		EnginePointer = engine;

		if (initialPlaybackSpeedMultiplier == 0)
		{
			throw std::invalid_argument("WAVAudioInstance::WAVAudioInstance() - initialPlaybackSpeedMultiplier cannot be 0");
		}
		PlaybackSpeedMultiplier = initialPlaybackSpeedMultiplier;

		if (initialVolume > 1 || initialVolume < 0)
		{
			throw std::invalid_argument("WAVAudioInstance::WAVAudioInstance() - initialVolume must be between 0 and 1");
		}
		
		//Read the WAV file header info
		FileInfo = WAVFileReader::ReadWAVFile(filePath);
		DataChunkCurrentIndex = 0;

		//Inspect the granularity and based on this, find the correct file map start, map size and view size
		SYSTEM_INFO systemInfo;
		GetSystemInfo(&systemInfo);
		DWORD granularity = systemInfo.dwAllocationGranularity;
		DWORD fileMapStart = static_cast<DWORD>(FileInfo.DataChunkOffsetInFile / granularity) * granularity;
		DWORD fileViewSize = static_cast<DWORD>(FileInfo.DataChunkOffsetInFile % granularity) + FileInfo.DataChunkSize;
		DWORD viewDelta = static_cast<DWORD>(FileInfo.DataChunkOffsetInFile) - fileMapStart;

		//Memory map the data chunk
		FileHandle = CreateFile
		(
			FileInfo.FilePath.c_str(),
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
		if (FileHandle == INVALID_HANDLE_VALUE)
		{
			throw std::runtime_error(DivergenceEngine::StringConverter::ConvertWideStringToANSI(std::format(L"WAVAudioInstance::WAVAudioInstance() - '{}' unable to be opened as a Win32 file handle", FileInfo.FilePath)));
		}

		FileMappingHandle = CreateFileMapping
		(
			FileHandle,
			NULL,
			PAGE_READONLY,
			0,
			fileViewSize,
			NULL
		);
		if (FileMappingHandle == NULL)
		{
			throw std::runtime_error(DivergenceEngine::StringConverter::ConvertWideStringToANSI(std::format(L"WAVAudioInstance::WAVAudioInstance() - '{}' unable to be opened as a Win32 File Map", FileInfo.FilePath)));
		}
		
		OriginalFileMappingPointer = (uint8_t*)MapViewOfFile
		(
			FileMappingHandle,
			FILE_MAP_READ,
			0,
			fileMapStart,
			fileViewSize
		);
		if (OriginalFileMappingPointer == NULL)
		{
			DWORD lastErrorCode = GetLastError();
			HRESULT hr = HRESULT_FROM_WIN32(lastErrorCode);
			throw std::runtime_error(DivergenceEngine::StringConverter::ConvertWideStringToANSI(std::format(L"WAVAudioInstance::WAVAudioInstance() - '{}' data chunk unable to be opened as a Win32 File Map View", FileInfo.FilePath)));
		}

		//Resolve delta
		DataChunkFileMappingPointer = OriginalFileMappingPointer + viewDelta;

		//Create the sound instance
		SoundEffectInstance = std::make_unique<DirectX::DynamicSoundEffectInstance>(
			EnginePointer,
			std::bind(&WAVAudioInstance::BufferNeeded, this, std::placeholders::_1),
			FileInfo.FormatChunk.SampleRate*PlaybackSpeedMultiplier,
			FileInfo.FormatChunk.NumChannels,
			FileInfo.FormatChunk.BitsPerSample);

		//Set the volume
		SoundEffectInstance->SetVolume(initialVolume);

		Logger::Log(std::format(L"Loaded {}", FileInfo.FilePath));
	}

	WAVAudioInstance::~WAVAudioInstance()
	{
		SoundEffectInstance->Pause();

		//Clear up all MMAP stuff
		UnmapViewOfFile(DataChunkFileMappingPointer);
		CloseHandle(FileMappingHandle);
		CloseHandle(FileHandle);

		Logger::Log(std::format(L"Destroyed {}", FileInfo.FilePath));
	}

	void WAVAudioInstance::Play(bool isLoop)
	{
		IsLoop = isLoop;
		SoundEffectInstance->Play();
	}

	void WAVAudioInstance::Stop()
	{
		SoundEffectInstance->Stop();
		DataChunkCurrentIndex = 0;
	}

	void WAVAudioInstance::Pause()
	{
		SoundEffectInstance->Pause();
	}

	void WAVAudioInstance::Resume()
	{
		SoundEffectInstance->Resume();
	}

	void WAVAudioInstance::SetVolume(float volume)
	{
		if(volume > 1 || volume < 0)
		{
			throw std::invalid_argument("WAVAudioInstance::SetVolume() - volume must be between 0 and 1");
		}

		SoundEffectInstance->SetVolume(volume);
	}

	void WAVAudioInstance::SetPlaybackSpeedMultiplier(uint8_t newPlaybackSpeedMultiplier)
	{
		//Ensure that the new playback speed multiplier is not 0
		if (newPlaybackSpeedMultiplier == 0)
		{
			throw std::invalid_argument("WAVAudioInstance::SetPlaybackSpeedMultiplier() - newPlaybackSpeedMultiplier cannot be 0");
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
		SoundEffectInstance = std::make_unique<DirectX::DynamicSoundEffectInstance>(
			EnginePointer,
			std::bind(&WAVAudioInstance::BufferNeeded, this, std::placeholders::_1),
			FileInfo.FormatChunk.SampleRate * PlaybackSpeedMultiplier,
			FileInfo.FormatChunk.NumChannels,
			FileInfo.FormatChunk.BitsPerSample);
		
		this->Play(IsLoop);

		if (!isPlaying)
		{
			this->Pause();
		}
	}

	//TODO: Handle "Detecting new audio devices" (https://github.com/microsoft/DirectXTK/wiki/Adding-audio-to-your-project#detecting-new-audio-devices)
	void WAVAudioInstance::BufferNeeded(DirectX::DynamicSoundEffectInstance* instance)
	{		
		//Get the target buffer size
		//If the byte rate is 176400 and a max number of buffers is 5, then 2048*3/176400 = 0.05 seconds (pretty good buffer sizing for latency, don't hear crackling either)
		//Multiplying by PlaybackSpeed so the buffers can keep up without crackle (might cause latency, check later)
		uint32_t targetBufferSize = 2048*PlaybackSpeedMultiplier;

		//This while loop ensures all the buffers needed are given at one time and the callback doesn't have to constantly be called killing performance
		while (!StopLoadingBuffers && instance->GetState() == DirectX::PLAYING && instance->GetPendingBufferCount() <= MAX_BUFFERS)
		{
			//Choose the minimum of the target buffer size and the actual remaining amount of data in the chunk
			uint32_t bufferSize = std::min(targetBufferSize, FileInfo.DataChunkSize - DataChunkCurrentIndex);

			//Submit buffer
			instance->SubmitBuffer(&DataChunkFileMappingPointer[DataChunkCurrentIndex], bufferSize);

			//Increment the current index by the buffer submitted
			DataChunkCurrentIndex += bufferSize;

			//If we got to the end of the data segment, reset the index to the beginning if it is loop
			if (DataChunkCurrentIndex == FileInfo.DataChunkSize && IsLoop)
			{
				DataChunkCurrentIndex = 0;
			}

			else if (DataChunkCurrentIndex == FileInfo.DataChunkSize && !IsLoop)
			{
				StopLoadingBuffers = true;
			}
		}

		if (StopLoadingBuffers && instance->GetPendingBufferCount() == 0)
		{
			this->Stop();
		}
	}
}