#include "Audio/WAVSimpleSoundEffect.h"
#include "Logger/Logger.h"
#include <filesystem>
#include <memory>

namespace fs = std::filesystem;

namespace DivergenceEngine
{
	WAVSimpleSoundEffect::WAVSimpleSoundEffect(DirectX::AudioEngine* engine, const std::wstring& filePath, float initialVolume)
	{
		//Validate arguments
		if(engine == nullptr)
		{
			throw std::invalid_argument("WAVSimpleSoundEffect::WAVSimpleSoundEffect() - engine is nullptr");
		}

		if (!fs::exists(filePath))
		{
			throw std::invalid_argument("WAVSimpleSoundEffect::WAVSimpleSoundEffect() - filePath does not exist");
		}
		FilePath = filePath;

		if(initialVolume < 0 || initialVolume > 1)
		{
			throw std::invalid_argument("WAVSimpleSoundEffect::WAVSimpleSoundEffect() - initialVolume is not in range [0, 1]");
		}
		CurrentVolume = initialVolume;

		SimpleSoundEffect = std::make_unique<DirectX::SoundEffect>(engine, FilePath.c_str());

		Logger::Log(std::format(L"Loaded {}", FilePath));
	}

	WAVSimpleSoundEffect::~WAVSimpleSoundEffect()
	{
		Logger::Log(std::format(L"Destroyed {}", FilePath));
	}

	void WAVSimpleSoundEffect::Play()
	{
		SimpleSoundEffect->Play(CurrentVolume, 0, 0);
	}
}