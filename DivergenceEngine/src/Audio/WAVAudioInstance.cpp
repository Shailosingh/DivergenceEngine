#define NOMINMAX
#include "WAVAudioInstance.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include "StringConverter.h"

namespace fs = std::filesystem;

namespace DivergenceEngine
{
	WAVAudioInstance::WAVAudioInstance(DirectX::AudioEngine* engine, std::wstring filePath, uint8_t initialPlaybackSpeedMultiplier)
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
		
		if (!fs::exists(filePath))
		{
			throw std::invalid_argument("WAVAudioInstance::WAVAudioInstance() - filePath does not exist");
		}
		FilePath = filePath;
		
		//Open up the file for binary reading
		std::ifstream fileInputReader;
		fileInputReader.open(FilePath, std::ios::binary);
		fileInputReader >> std::noskipws;
		if (!fileInputReader)
		{
			assert(false);
			throw std::runtime_error(DivergenceEngine::StringConverter::ConvertWideStringToANSI(std::format(L"WAVAudioInstance::WAVAudioInstance() - failed to open '{}' for reading", FilePath)));
		}

		//Read the RIFF header
		RIFFHeader riffHeader;
		fileInputReader.read(reinterpret_cast<char*>(&riffHeader), sizeof(RIFFHeader));

		//Check if the RIFF header is correct
		size_t correctRemainingFileSize = fs::file_size(FilePath) - 8;
		bool isCorrectRemainingFileSize = riffHeader.RemainingFileSize == correctRemainingFileSize;
		if (!(IsCorrectFourCC(riffHeader.Header, RIFF_HEADER) && IsCorrectFourCC(riffHeader.Format, WAVE_FORMAT) && isCorrectRemainingFileSize))
		{
			assert(false);
			throw std::runtime_error(DivergenceEngine::StringConverter::ConvertWideStringToANSI(std::format(L"WAVAudioInstance::WAVAudioInstance() - '{}' is not a valid RIFF file", FilePath)));
		}

		//Search for the FMT chunk
		SubchunkHeader subchunkHeader;
		bool isFoundFmtChunk = false;
		while (fileInputReader.read(reinterpret_cast<char*>(&subchunkHeader), sizeof(SubchunkHeader)))
		{
			if (IsCorrectFourCC(subchunkHeader.Header, FMT_CHUNK))
			{
				isFoundFmtChunk = true;
				break;
			}
			else
			{
				fileInputReader.seekg(subchunkHeader.ChunkSize, std::ios::cur);
			}
		}

		//Check if the FMT chunk was found
		if (!isFoundFmtChunk)
		{
			assert(false);
			throw std::runtime_error(DivergenceEngine::StringConverter::ConvertWideStringToANSI(std::format(L"WAVAudioInstance::WAVAudioInstance() - '{}' does not contain a FMT chunk", FilePath)));
		}

		//Read the FMT chunk
		fileInputReader.read(reinterpret_cast<char*>(&FormatChunk), sizeof(FMTChunk));
		
		//Ensure the data will be PCM
		if (FormatChunk.AudioFormat != PCM_FORMAT)
		{
			assert(false);
			throw std::runtime_error(DivergenceEngine::StringConverter::ConvertWideStringToANSI(std::format(L"WAVAudioInstance::WAVAudioInstance() - '{}' is not PCM", FilePath)));
		}

		//Search for data chunk
		bool isFoundDataChunk = false;
		while (fileInputReader.read(reinterpret_cast<char*>(&subchunkHeader), sizeof(SubchunkHeader)))
		{
			if (IsCorrectFourCC(subchunkHeader.Header, DATA_CHUNK))
			{
				isFoundDataChunk = true;
				break;
			}
			else
			{
				fileInputReader.seekg(subchunkHeader.ChunkSize, std::ios::cur);
			}
		}

		//Check if the data chunk was found
		if (!isFoundDataChunk)
		{
			assert(false);
			throw std::runtime_error(DivergenceEngine::StringConverter::ConvertWideStringToANSI(std::format(L"WAVAudioInstance::WAVAudioInstance() - '{}' does not contain a DATA chunk", FilePath)));
		}
		
		//Read the data chunk size
		DataChunkSize = subchunkHeader.ChunkSize;
		
		//Record the current byte offset
		DataChunkOffsetInFile = fileInputReader.tellg();
		DataChunkCurrentIndex = 0;

		//Close up the file reader
		fileInputReader.close();

		//Inspect the granularity and based on this, find the correct file map start, map size and view size
		SYSTEM_INFO systemInfo;
		GetSystemInfo(&systemInfo);
		DWORD granularity = systemInfo.dwAllocationGranularity;
		DWORD fileMapStart = static_cast<DWORD>(DataChunkOffsetInFile / granularity) * granularity;
		DWORD fileViewSize = static_cast<DWORD>(DataChunkOffsetInFile % granularity) + DataChunkSize;
		DWORD fileMapSize = static_cast<DWORD>(DataChunkOffsetInFile + DataChunkSize);
		DWORD viewDelta = DataChunkOffsetInFile - fileMapStart;

		//Memory map the data chunk
		FileHandle = CreateFile
		(
			FilePath.c_str(),
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
		if (FileHandle == INVALID_HANDLE_VALUE)
		{
			throw std::runtime_error(DivergenceEngine::StringConverter::ConvertWideStringToANSI(std::format(L"WAVAudioInstance::WAVAudioInstance() - '{}' unable to be opened as a Win32 file handle", FilePath)));
		}

		FileMappingHandle = CreateFileMapping
		(
			FileHandle,
			NULL,
			PAGE_READONLY,
			0,
			fileMapSize,
			NULL
		);
		if (FileMappingHandle == NULL)
		{
			throw std::runtime_error(DivergenceEngine::StringConverter::ConvertWideStringToANSI(std::format(L"WAVAudioInstance::WAVAudioInstance() - '{}' unable to be opened as a Win32 File Map", FilePath)));
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
			throw std::runtime_error(DivergenceEngine::StringConverter::ConvertWideStringToANSI(std::format(L"WAVAudioInstance::WAVAudioInstance() - '{}' data chunk unable to be opened as a Win32 File Map View", FilePath)));
		}

		//Resolve delta
		DataChunkFileMappingPointer = OriginalFileMappingPointer + viewDelta;

		//Create the sound instance
		SoundEffectInstance = std::make_unique<DirectX::DynamicSoundEffectInstance>(
			EnginePointer,
			std::bind(&WAVAudioInstance::BufferNeeded, this, std::placeholders::_1),
			FormatChunk.SampleRate*PlaybackSpeedMultiplier, 
			FormatChunk.NumChannels, 
			FormatChunk.BitsPerSample);
	}

	WAVAudioInstance::~WAVAudioInstance()
	{
		SoundEffectInstance->Pause();

		//Clear up all MMAP stuff
		UnmapViewOfFile(DataChunkFileMappingPointer);
		CloseHandle(FileMappingHandle);
		CloseHandle(FileHandle);
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
		SoundEffectInstance->SetVolume(volume);
	}

	void WAVAudioInstance::SetPlaybackSpeedMultiplier(uint8_t newPlaybackSpeedMultiplier)
	{
		//Ensure that the new playback speed multiplier is not 0
		if (newPlaybackSpeedMultiplier == 0)
		{
			assert(false);
			throw std::invalid_argument("WAVAudioInstance::SetPlaybackSpeedMultiplier() - newPlaybackSpeedMultiplier cannot be 0");
		}
		
		//Check if the song is already playing and if it was looping
		bool isPlaying = SoundEffectInstance->GetState() == DirectX::SoundState::PLAYING;
		
		//Reset the instance with the altered sample rate based on the new playback speed multiplier
		PlaybackSpeedMultiplier = newPlaybackSpeedMultiplier;
		SoundEffectInstance = std::make_unique<DirectX::DynamicSoundEffectInstance>(
			EnginePointer,
			std::bind(&WAVAudioInstance::BufferNeeded, this, std::placeholders::_1),
			FormatChunk.SampleRate * PlaybackSpeedMultiplier,
			FormatChunk.NumChannels,
			FormatChunk.BitsPerSample);

		if (isPlaying)
		{
			this->Play(IsLoop);
		}
	}

	void WAVAudioInstance::BufferNeeded(DirectX::DynamicSoundEffectInstance* instance)
	{
		//Get the target buffer size
		uint32_t targetBufferSize = 5*PlaybackSpeedMultiplier * FormatChunk.ByteRate;

		//Choose the minimum of the target buffer size and the actual remaining amount of data in the chunk
		uint32_t bufferSize = std::min(targetBufferSize, DataChunkSize - DataChunkCurrentIndex);

		//Submit buffer
		instance->SubmitBuffer(&DataChunkFileMappingPointer[DataChunkCurrentIndex], bufferSize);

		//Increment the current index by the buffer submitted
		DataChunkCurrentIndex += bufferSize;

		//If we got to the end of the data segment, reset the index to the beginning if it is loop
		if (DataChunkCurrentIndex == DataChunkSize && IsLoop)
		{
			DataChunkCurrentIndex = 0;
		}

		else if (DataChunkCurrentIndex == DataChunkSize && !IsLoop)
		{
			this->Stop();
		}
	}
	
	bool WAVAudioInstance::IsCorrectFourCC(const char* chunkFourCC, const char* correctFourCC) const
	{
		for (int index = 0; index < 4; index++)
		{
			if (chunkFourCC[index] != correctFourCC[index])
			{
				return false;
			}
		}

		return true;
	}
}