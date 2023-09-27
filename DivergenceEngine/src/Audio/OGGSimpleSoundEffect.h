#pragma once
#include "Audio/ISimpleSoundEffect.h"
#include "vorbis/vorbisfile.h"

namespace DivergenceEngine
{
	class OGGSimpleSoundEffect : public ISimpleSoundEffect
	{
	private:
		std::unique_ptr<DirectX::SoundEffect> SimpleSoundEffect;
		std::unique_ptr<uint8_t[]> WaveData;
		std::wstring FilePath;
		OggVorbis_File VorbisFileObject;
		vorbis_info* VorbisInfo;
		const int BIT_DEPTH = 16;
		uint64_t BlockAlign;

	public:
		OGGSimpleSoundEffect(DirectX::AudioEngine* engine, const std::wstring& filePath);
		~OGGSimpleSoundEffect();

		//Overriden functions
		void Play(float volume = 1) override;
	};
}