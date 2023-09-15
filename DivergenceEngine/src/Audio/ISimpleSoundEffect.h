#pragma once
#include <Audio.h>

namespace DivergenceEngine
{
	class ISimpleSoundEffect
	{
	public:
		virtual void Play() = 0;
	};
}