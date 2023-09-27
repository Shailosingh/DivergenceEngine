#include "Audio/WAVSimpleSoundEffect.h"
#include "Logger/Logger.h"
#include <filesystem>
#include <memory>

namespace fs = std::filesystem;

namespace DivergenceEngine
{
	WAVSimpleSoundEffect::WAVSimpleSoundEffect(DirectX::AudioEngine* engine, const std::wstring& filePath)
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

		SimpleSoundEffect = std::make_unique<DirectX::SoundEffect>(engine, FilePath.c_str());

		Logger::Log(std::format(L"Loaded {}", FilePath));
	}

	WAVSimpleSoundEffect::~WAVSimpleSoundEffect()
	{
		Logger::Log(std::format(L"Destroyed {}", FilePath));
	}

	void WAVSimpleSoundEffect::Play(float volume)
	{
		if (volume < 0 || volume > 1)
		{
			throw std::invalid_argument("WAVSimpleSoundEffect::Play() - volume is not in range [0, 1]");
		}

		SimpleSoundEffect->Play(volume, 0, 0);
	}
}