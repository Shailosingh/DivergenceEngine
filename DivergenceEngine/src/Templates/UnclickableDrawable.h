#pragma once
#include "Window/IDrawable.h"
#include <SimpleMath.h>

namespace DivergenceEngine
{
	class UnclickableDrawable : public IDrawable
	{
	public:
		void OnMouseOver() override {};
		void OnMouseNotOver() override {};
		bool OnLeftPress(DirectX::XMINT2 mousePos) override { return false; };
		bool OnLeftRelease(DirectX::XMINT2 mousePos) override { return false; };
		bool OnMiddlePress(DirectX::XMINT2 mousePos) override { return false; };
		bool OnMiddleRelease(DirectX::XMINT2 mousePos) override { return false; };
		bool OnRightPress(DirectX::XMINT2 mousePos) override { return false; };
		bool OnRightRelease(DirectX::XMINT2 mousePos) override { return false; };
	};
}
