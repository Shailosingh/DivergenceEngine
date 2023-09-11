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
		FileInputReader.open(FilePath, std::ios::binary);
		FileInputReader >> std::noskipws;
		if (!FileInputReader)
		{
			assert(false);
			throw std::runtime_error(DivergenceEngine::StringConverter::ConvertWideStringToANSI(std::format(L"WAVAudioInstance::WAVAudioInstance() - failed to open '{}' for reading", FilePath)));
		}

		//Read the RIFF header
		RIFFHeader riffHeader;
		FileInputReader.read(reinterpret_cast<char*>(&riffHeader), sizeof(RIFFHeader));

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
		while (FileInputReader.read(reinterpret_cast<char*>(&subchunkHeader), sizeof(SubchunkHeader)))
		{
			if (IsCorrectFourCC(subchunkHeader.Header, FMT_CHUNK))
			{
				isFoundFmtChunk = true;
				break;
			}
			else
			{
				FileInputReader.seekg(subchunkHeader.ChunkSize, std::ios::cur);
			}
		}

		//Check if the FMT chunk was found
		if (!isFoundFmtChunk)
		{
			assert(false);
			throw std::runtime_error(DivergenceEngine::StringConverter::ConvertWideStringToANSI(std::format(L"WAVAudioInstance::WAVAudioInstance() - '{}' does not contain a FMT chunk", FilePath)));
		}

		//Read the FMT chunk
		FileInputReader.read(reinterpret_cast<char*>(&FormatChunk), sizeof(FMTChunk));
		
		//Ensure the data will be PCM
		if (FormatChunk.AudioFormat != PCM_FORMAT)
		{
			assert(false);
			throw std::runtime_error(DivergenceEngine::StringConverter::ConvertWideStringToANSI(std::format(L"WAVAudioInstance::WAVAudioInstance() - '{}' is not PCM", FilePath)));
		}

		//Search for data chunk
		bool isFoundDataChunk = false;
		while (FileInputReader.read(reinterpret_cast<char*>(&subchunkHeader), sizeof(SubchunkHeader)))
		{
			if (IsCorrectFourCC(subchunkHeader.Header, DATA_CHUNK))
			{
				isFoundDataChunk = true;
				break;
			}
			else
			{
				FileInputReader.seekg(subchunkHeader.ChunkSize, std::ios::cur);
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
		DataChunkOffsetInFile = FileInputReader.tellg();
		DataChunkCurrentIndex = 0;

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
		FileInputReader.close();
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
		uint8_t numberOfNewBuffersRequired = MAX_NUM_OF_BUFFERS - instance->GetPendingBufferCount();
		assert(numberOfNewBuffersRequired <= MAX_NUM_OF_BUFFERS);

		uint8_t realNumberOfLoadedAudioBuffers = LoadNewAudioBuffers(numberOfNewBuffersRequired);

		for (int index = 0; index < realNumberOfLoadedAudioBuffers; index++)
		{
			instance->SubmitBuffer(&AudioBuffers[index].front(), AudioBuffers[index].size());
		}

		if (realNumberOfLoadedAudioBuffers < numberOfNewBuffersRequired)
		{
			this->Stop();
		}
	}

	uint8_t WAVAudioInstance::LoadNewAudioBuffers(uint8_t numberOfNewBuffersRequired)
	{
		uint32_t targetBufferSize = 15* FormatChunk.ByteRate * PlaybackSpeedMultiplier;
		//uint32_t targetBufferSize = 1024 * FormatChunk.BlockAlign * PlaybackSpeedMultiplier;

		//Seek to the correct position
		FileInputReader.seekg(DataChunkOffsetInFile + DataChunkCurrentIndex, std::ios::beg);

		//Fill the buffers and stop early if the end of the file is reached
		uint8_t realNumberOfNewBuffersLoaded = 0;
		while (realNumberOfNewBuffersLoaded < numberOfNewBuffersRequired)
		{
			uint32_t bytesLeftInFile = DataChunkSize - DataChunkCurrentIndex;

			if (bytesLeftInFile == 0)
			{
				break;
			}

			uint32_t bytesToRead = std::min(bytesLeftInFile, targetBufferSize);
			AudioBuffers[realNumberOfNewBuffersLoaded].resize(bytesToRead);
			FileInputReader.read(reinterpret_cast<char*>(&AudioBuffers[realNumberOfNewBuffersLoaded].front()), AudioBuffers[realNumberOfNewBuffersLoaded].size());
			
			//Increment the index by the bytes read and if the end of the file is reached, loop back to the start if looping is enabled
			DataChunkCurrentIndex += bytesToRead;
			if (DataChunkCurrentIndex == DataChunkSize && IsLoop)
			{
				DataChunkCurrentIndex = 0;
				FileInputReader.seekg(DataChunkOffsetInFile, std::ios::beg);
			}

			realNumberOfNewBuffersLoaded++;
		}

		return realNumberOfNewBuffersLoaded;
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