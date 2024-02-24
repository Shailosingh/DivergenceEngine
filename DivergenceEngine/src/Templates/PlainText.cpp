#include "Templates/PlainText.h"
#include <Logger/Logger.h>

namespace DivergenceEngine::Templates
{
	PlainText::PlainText(std::weak_ptr<Graphics> graphicsController, const std::wstring& textString, const std::wstring& spriteFontPath, DirectX::SimpleMath::Vector2 positionCoord, TextOriginClass originClass, DirectX::FXMVECTOR colour, bool dropShadow):
		WindowGraphicsController(graphicsController),
		TextString(textString),
		PositionCoord(positionCoord),
		Colour(colour),
		DropShadow(dropShadow)
	{
		//Load the font
		WindowGraphicsController.lock()->LoadFont(spriteFontPath, SpriteFont);

		//Compute the origin
		OriginCoord = ComputeOrigin(originClass);
	}

	void PlainText::SetTextString(const std::wstring& textString)
	{
		TextString = textString;
	}

	void PlainText::Draw()
	{
		WindowGraphicsController.lock()->DrawString(SpriteFont.lock().get(), TextString, PositionCoord, OriginCoord, Colour, DropShadow);
	}

	bool PlainText::IsCoordInObject(DirectX::XMINT2 mousePos)
	{
		//Get the top left corner of the text
		DirectX::SimpleMath::Vector2 topLeft = PositionCoord - OriginCoord;

		//Get the rectangle bounding the text
		RECT boundingRect = SpriteFont.lock()->MeasureDrawBounds(TextString.c_str(), topLeft, false);
		DirectX::SimpleMath::Rectangle boundingRectangle = DirectX::SimpleMath::Rectangle(boundingRect.left, boundingRect.top, boundingRect.right - boundingRect.left, boundingRect.bottom - boundingRect.top);

		//Check if the mouse is in the rectangle
		return boundingRectangle.Contains(static_cast<long>(mousePos.x), static_cast<long> (mousePos.y));
	}

	DirectX::SimpleMath::Vector2 PlainText::ComputeOrigin(TextOriginClass originClass) noexcept
	{
		//Get the size of the string
		DirectX::SimpleMath::Vector2 stringSize = SpriteFont.lock()->MeasureString(TextString.c_str(), false);

		//If the size is zero, return 0
		if (stringSize == DirectX::SimpleMath::Vector2(0, 0))
		{
			return DirectX::SimpleMath::Vector2(0,0);
		}

		//Taking into account the origin setting, compute the origin vector
		DirectX::SimpleMath::Vector2 origin = stringSize;
		switch (originClass)
		{
		case TextOriginClass::TopLeft:
			origin = DirectX::SimpleMath::Vector2(0, 0);
			break;

		case TextOriginClass::TopCentre:
			origin = DirectX::SimpleMath::Vector2(origin.x / 2.f, 0);
			break;

		case TextOriginClass::TopRight:
			origin = DirectX::SimpleMath::Vector2(origin.x, 0);
			break;

		case TextOriginClass::Centre:
			origin /= 2.f;
			break;

		case TextOriginClass::BottomLeft:
			origin = DirectX::SimpleMath::Vector2(0, origin.y);
			break;

		case TextOriginClass::BottomCentre:
			origin = DirectX::SimpleMath::Vector2(origin.x / 2.f, origin.y);
			break;

		case TextOriginClass::BottomRight:
			origin = DirectX::SimpleMath::Vector2(origin.x, origin.y);
			break;

		default:
			DivergenceEngine::Logger::Log(L"Invalid TextOriginClass");
			assert(false);
			return DirectX::SimpleMath::Vector2(0, 0);
			break;
		}

		return origin;
	}
}