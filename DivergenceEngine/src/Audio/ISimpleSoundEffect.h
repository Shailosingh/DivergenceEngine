#pragma once
#include <Audio.h>

namespace DivergenceEngine
{
	class ISimpleSoundEffect
	{
	public:
		virtual ~ISimpleSoundEffect() {}

		virtual void Play(float volume = 1) = 0;
	};
}