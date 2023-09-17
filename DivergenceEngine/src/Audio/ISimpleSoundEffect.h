#pragma once
#include <Audio.h>

namespace DivergenceEngine
{
	class ISimpleSoundEffect
	{
	public:
		virtual ~ISimpleSoundEffect() {}

		virtual void Play() = 0;
	};
}