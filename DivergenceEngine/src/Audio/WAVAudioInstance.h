#pragma once
#include "Audio/IAudioInstance.h"
#include <Audio.h>
#include <memory>
#include <string>
#include <array>
#include <vector>

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
		const static uint8_t MAX_NUM_OF_BUFFERS = 20;
		std::array<std::vector<uint8_t>, MAX_NUM_OF_BUFFERS> AudioBuffers;
		std::unique_ptr<DirectX::DynamicSoundEffectInstance> SoundEffectInstance;
		uint8_t PlaybackSpeedMultiplier;
		uint32_t DataChunkCurrentIndex;
		bool IsLoop;

		//WAV Datafields
		std::wstring FilePath;
		FMTChunk FormatChunk;
		uint32_t DataChunkSize;
		size_t DataChunkOffsetInFile;

		//Buffer functions
		void BufferNeeded(DirectX::DynamicSoundEffectInstance* instance);

		/// <summary>
		/// Loads a number of new buffers into the array of buffers
		/// so they can be played. It returns the number of buffers sucessfully
		/// retrieved
		/// </summary>
		/// <param name="numberOfNewBuffersRequired">Requested number of new buffers</param>
		/// <returns>Actual number of new buffers</returns>
		uint8_t LoadNewAudioBuffers(uint8_t numberOfNewBuffersRequired);

		//Helpers
		bool IsCorrectFourCC(const char* chunkFourCC, const char* correctFourCC) const;
		
	public:
		//Constructors and destructors
		WAVAudioInstance(DirectX::AudioEngine* engine, std::wstring filePath, uint8_t initialPlaybackSpeedMultiplier = 1);

		//Overriden functions
		void Play(bool isLoop = true) override;
		void Stop() override;
		void Pause() override;
		void Resume() override;
		void SetVolume(float newVolume) override;
		void SetPlaybackSpeedMultiplier(uint8_t newPlaybackSpeedMultiplier) override;
	};
}