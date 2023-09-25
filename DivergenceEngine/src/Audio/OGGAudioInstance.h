#pragma once
#include "Audio/IAudioInstance.h"
#include "vorbis/vorbisfile.h"
#include <array>
#include <vector>

namespace DivergenceEngine
{
	class OGGAudioInstance : public IAudioInstance
	{
	private:
		//General Datafields
		DirectX::AudioEngine* EnginePointer;
		std::unique_ptr<DirectX::DynamicSoundEffectInstance> SoundEffectInstance;
		uint8_t PlaybackSpeedMultiplier;
		bool IsLoop;

		//Buffer variables
		const static int32_t MAX_BUFFERS = 5;
		std::array<std::vector<uint8_t>, MAX_BUFFERS> Buffers;
		bool StopLoadingBuffers = false;

		//Buffer functions
		void BufferNeeded(DirectX::DynamicSoundEffectInstance* instance);

		//OGG variables
		std::wstring FilePath;
		OggVorbis_File VorbisFileObject;
		vorbis_info* VorbisInfo;
		ogg_int64_t TotalSamples;
		const uint64_t BIT_DEPTH = 16;
		uint64_t BlockAlign;

	public:
		//Constructors and Destructors
		OGGAudioInstance(DirectX::AudioEngine* engine, std::wstring filePath, uint8_t initialPlaybackSpeedMultiplier = 1, float initialVolume = 1);
		~OGGAudioInstance();

		//Overriden functions
		void Play(bool isLoop = true) override;
		void Stop() override;
		void Pause() override;
		void Resume() override;
		void SetVolume(float newVolume) override;
		void SetPlaybackSpeedMultiplier(uint8_t newPlaybackSpeedMultiplier) override;
	};
}