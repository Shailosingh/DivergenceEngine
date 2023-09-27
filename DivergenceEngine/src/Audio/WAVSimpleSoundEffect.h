#pragma once
#include "Audio/ISimpleSoundEffect.h"

namespace DivergenceEngine
{
	class WAVSimpleSoundEffect : public ISimpleSoundEffect
	{
	private:
		std::unique_ptr<DirectX::SoundEffect> SimpleSoundEffect;
		std::wstring FilePath;

	public:
		WAVSimpleSoundEffect(DirectX::AudioEngine* engine, const std::wstring& filePath);
		~WAVSimpleSoundEffect();

		//Overriden functions
		void Play(float volume = 1) override;
	};
}