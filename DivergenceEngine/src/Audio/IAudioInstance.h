#pragma once
#include <Audio.h>

namespace DivergenceEngine
{
	class IAudioInstance
	{
	public:
		virtual ~IAudioInstance() {}

		virtual void Play(bool isLoop = true) = 0;

		virtual void Stop() = 0;
		
		virtual void Pause() = 0;

		virtual void Resume() = 0;

		virtual void SetVolume(float newVolume) = 0;

		virtual void SetPlaybackSpeedMultiplier(uint8_t newPlaybackSpeedMultiplier) = 0;
	};
}