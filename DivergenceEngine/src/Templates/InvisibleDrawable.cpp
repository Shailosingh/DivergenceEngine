#include "Templates/InvisibleDrawable.h"

namespace DivergenceEngine::Templates
{
	InvisibleDrawable::InvisibleDrawable(DirectX::SimpleMath::Vector2 position, DirectX::SimpleMath::Vector2 size) :
		Position(position),
		Size(size)
	{}
	
	bool InvisibleDrawable::IsCoordInObject(DirectX::XMINT2 mousePos)
	{
		bool withinXBounds = (mousePos.x >= Position.x) && (mousePos.x <= (Position.x + Size.x));
		bool withinYBounds = (mousePos.y >= Position.y) && (mousePos.y <= (Position.y + Size.y));

		return withinXBounds && withinYBounds;
	}
}