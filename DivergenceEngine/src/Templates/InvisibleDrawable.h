#pragma once
#include "Templates//UnclickableDrawable.h"
#include <SimpleMath.h>

namespace DivergenceEngine::Templates
{
	class InvisibleDrawable : public UnclickableDrawable
	{
	private:
		//Datafields
		DirectX::SimpleMath::Vector2 Position;
		DirectX::SimpleMath::Vector2 Size;

	public:
		//Constructors and Destructors
		InvisibleDrawable(DirectX::SimpleMath::Vector2 position, DirectX::SimpleMath::Vector2 size);

		//Helpers
		DirectX::SimpleMath::Vector2 GetPosition() { return Position; }
		DirectX::SimpleMath::Vector2 GetSize() { return Size; }

		//Overriden functions
		void Draw() override {}
		bool IsCoordInObject(DirectX::XMINT2 mousePos) override;
	};
}