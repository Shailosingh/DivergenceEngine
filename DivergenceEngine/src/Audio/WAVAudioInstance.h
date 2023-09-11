#pragma once
#include "Audio/IAudioInstance.h"
#include <Audio.h>
#include <memory>
#include <string>
#include <array>
#include <vector>
#include <fstream>

//http://soundfile.sapp.org/doc/WaveFormat/
//https://en.wikipedia.org/wiki/WAV
//https://github.com/microsoft/DirectXTK/wiki/Audio

namespace DivergenceEngine
{
	class WAVAudioInstance : public IAudioInstance
	{
	private:
		const char RIFF_HEADER[4] = { 'R', 'I', 'F', 'F' };
		const char WAVE_FORMAT[4] = { 'W', 'A', 'V', 'E' };
		struct RIFFHeader
		{
			char Header[4];
			uint32_t RemainingFileSize;
			char Format[4];
		};

		struct SubchunkHeader
		{
			char Header[4];
			uint32_t ChunkSize;
		};

		const char FMT_CHUNK[4] = { 'f', 'm', 't', ' ' };
		const uint16_t PCM_FORMAT = 1;
		struct FMTChunk
		{
			uint16_t AudioFormat;
			uint16_t NumChannels;
			uint32_t SampleRate;
			uint32_t ByteRate;
			uint16_t BlockAlign;
			uint16_t BitsPerSample;
		};
		
		const char DATA_CHUNK[4] = { 'd', 'a', 't', 'a' };

		//General Datafields
		DirectX::AudioEngine* EnginePointer;
		std::unique_ptr<DirectX::DynamicSoundEffectInstance> SoundEffectInstance;
		uint8_t PlaybackSpeedMultiplier;
		bool IsLoop;

		//Memory mapped file datafields
		HANDLE FileHandle;
		HANDLE FileMappingHandle;
		uint8_t* OriginalFileMappingPointer;
		uint8_t* DataChunkFileMappingPointer;
		uint32_t DataChunkCurrentIndex;

		//WAV Datafields
		std::wstring FilePath;
		FMTChunk FormatChunk;
		uint32_t DataChunkSize;
		size_t DataChunkOffsetInFile;

		//Buffer functions
		void BufferNeeded(DirectX::DynamicSoundEffectInstance* instance);

		//Helpers
		bool IsCorrectFourCC(const char* chunkFourCC, const char* correctFourCC) const;
		
	public:
		//Constructors and destructors
		WAVAudioInstance(DirectX::AudioEngine* engine, std::wstring filePath, uint8_t initialPlaybackSpeedMultiplier = 1);
		~WAVAudioInstance();

		//Overriden functions
		void Play(bool isLoop = true) override;
		void Stop() override;
		void Pause() override;
		void Resume() override;
		void SetVolume(float newVolume) override;
		void SetPlaybackSpeedMultiplier(uint8_t newPlaybackSpeedMultiplier) override;
	};
}