#include "Templates/PlainText.h"

namespace DivergenceEngine::Templates
{
	PlainText::PlainText(std::weak_ptr<Graphics> graphicsController, const std::wstring& textString, std::wstring spriteFontPath, DirectX::SimpleMath::Vector2 positionCoord, DirectX::FXMVECTOR colour, bool dropShadow, bool isPositionCoordCentredOnString):
		WindowGraphicsController(graphicsController),
		TextString(textString),
		PositionCoord(positionCoord),
		Colour(colour),
		DropShadow(dropShadow),
		IsPositionCoordCentredOnString(isPositionCoordCentredOnString)
	{
		//Load the font
		WindowGraphicsController.lock()->LoadFont(spriteFontPath, SpriteFont);
	}

	PlainText::PlainText(std::weak_ptr<Graphics> graphicsController, const std::wstring& textString, DefaultFonts::DefaultFontIndices defaultFontIndex, DirectX::SimpleMath::Vector2 positionCoord, DirectX::FXMVECTOR colour, bool dropShadow, bool isPositionCoordCentredOnString):
		WindowGraphicsController(graphicsController),
		TextString(textString),
		PositionCoord(positionCoord),
		Colour(colour),
		DropShadow(dropShadow),
		IsPositionCoordCentredOnString(isPositionCoordCentredOnString)
	{
		//Load the font
		WindowGraphicsController.lock()->LoadFont(defaultFontIndex, SpriteFont);
	}

	void PlainText::SetTextString(const std::wstring& textString)
	{
		TextString = textString;
	}

	void PlainText::Draw()
	{
		if(IsPositionCoordCentredOnString)
		{
			WindowGraphicsController.lock()->DrawString_CentreCoordinate(SpriteFont.lock().get(), TextString, PositionCoord, Colour, DropShadow);
		}
		else
		{
			WindowGraphicsController.lock()->DrawString_TopLeftCoordinate(SpriteFont.lock().get(), TextString, PositionCoord, Colour, DropShadow);
		}
	}

	bool PlainText::IsCoordInObject(DirectX::XMINT2 mousePos)
	{
		//Measure the size of the string
		DirectX::SimpleMath::Vector2 stringSize = SpriteFont.lock()->MeasureString(TextString.c_str());
	}
}