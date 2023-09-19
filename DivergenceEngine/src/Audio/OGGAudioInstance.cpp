#include "Audio/IAudioInstance.h"
#include "Audio/OGGAudioInstance.h"
#include "Logger/Logger.h"
#include <stdexcept>
#include <stdio.h>

//https://xiph.org/vorbis/doc/vorbisfile/overview.html
//https://github.com/edubart/minivorbis

namespace DivergenceEngine
{
	OGGAudioInstance::OGGAudioInstance(DirectX::AudioEngine* engine, std::wstring filePath, uint8_t initialPlaybackSpeedMultiplier, float initialVolume)
	{
		//Handle invalid parameters
		if (engine == nullptr)
		{
			throw std::invalid_argument("OGGAudioInstance::OGGAudioInstance() - engine cannot be nullptr");
		}
		EnginePointer = engine;

		if (initialPlaybackSpeedMultiplier == 0)
		{
			throw std::invalid_argument("OGGAudioInstance::OGGAudioInstance() - initialPlaybackSpeedMultiplier cannot be 0");
		}
		PlaybackSpeedMultiplier = initialPlaybackSpeedMultiplier;

		if (initialVolume > 1 || initialVolume < 0)
		{
			throw std::invalid_argument("OGGAudioInstance::OGGAudioInstance() - initialVolume must be between 0 and 1");
		}

		//Try to open the file C style
		FILE* fileObject = _wfopen(filePath.c_str(), L"r");
		if (fileObject == nullptr)
		{

		}
	}

	OGGAudioInstance::~OGGAudioInstance()
	{
	}
}