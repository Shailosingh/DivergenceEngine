#pragma once
#include "Audio/IAudioInstance.h"
#include "vorbis/vorbisfile.h"

namespace DivergenceEngine
{
	class OGGAudioInstance : public IAudioInstance
	{
	private:
		//General Datafields
		DirectX::AudioEngine* EnginePointer;
		std::unique_ptr<DirectX::DynamicSoundEffectInstance> SoundEffectInstance;
		OggVorbis_File VorbisFileObject;
		uint8_t PlaybackSpeedMultiplier;
		bool IsLoop;

		//Buffer variables
		const int32_t MAX_BUFFERS = 5;

		//Buffer functions
		void BufferNeeded(DirectX::DynamicSoundEffectInstance* instance);

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