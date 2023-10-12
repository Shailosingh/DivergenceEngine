#pragma once
#include "Templates/UnclickableDrawable.h"
#include "Graphics/Graphics.h"
#include <string>

namespace DivergenceEngine::Templates
{
	class PlainText : public DivergenceEngine::UnclickableDrawable
	{
	private:
		//Datafields
		std::wstring TextString;
		DirectX::SimpleMath::Vector2 PositionCoord;
		DirectX::FXMVECTOR Colour;
		bool DropShadow;
		bool IsPositionCoordCentredOnString;
		std::weak_ptr<DirectX::SpriteFont> SpriteFont;
		std::weak_ptr<Graphics> WindowGraphicsController;

	public:
		//Constructors and Destructor
		PlainText(std::weak_ptr<Graphics> graphicsController, const std::wstring& textString, std::wstring spriteFontPath, DirectX::SimpleMath::Vector2 positionCoord, DirectX::FXMVECTOR colour = DirectX::Colors::White, bool dropShadow = false, bool isPositionCoordCentredOnString = false);
		PlainText(std::weak_ptr<Graphics> graphicsController, const std::wstring& textString, DefaultFonts::DefaultFontIndices defaultFontIndex, DirectX::SimpleMath::Vector2 positionCoord, DirectX::FXMVECTOR colour = DirectX::Colors::White, bool dropShadow = false, bool isPositionCoordCentredOnString = false);

		//Setters
		void SetTextString(const std::wstring& textString);

		//Overriden functions
		void Draw() override;
		bool IsCoordInObject(DirectX::XMINT2 mousePos) override;
	};
}