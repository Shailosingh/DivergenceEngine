#pragma once
#include "Audio/ISimpleSoundEffect.h"

namespace DivergenceEngine
{
	class WAVSimpleSoundEffect : public ISimpleSoundEffect
	{
	private:
		std::unique_ptr<DirectX::SoundEffect> SimpleSoundEffect;
		std::wstring FilePath;
		float CurrentVolume;

	public:
		WAVSimpleSoundEffect(DirectX::AudioEngine* engine, const std::wstring& filePath, float initialVolume = 1);

		//Overriden functions
		void Play() override;
	};
}