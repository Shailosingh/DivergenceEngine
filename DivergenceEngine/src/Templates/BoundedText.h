/*
#pragma once
#include "Templates/PlainText.h"
#include "Graphics/Graphics.h"

namespace DivergenceEngine::Templates
{
	class BoundedText : public DivergenceEngine::UnclickableDrawable
	{
	public:
		enum class PrintLinesOrientationClass
		{
			PrintFromTop,
			PrintFromCentre,
			PrintFromBottom
		};

		enum class TextAlignmentClass
		{
			Left,
			Centre,
			Right
		};

	private:
		//Datafields
		std::wstring TextString;
		std::vector<PlainText> TextLines;
		DirectX::SimpleMath::Vector2 PositionCoord;
		DirectX::SimpleMath::Rectangle BoundingRectangle;
		PrintLinesOrientationClass PrintLinesOrientation;
		TextAlignmentClass TextAlignment;
		DirectX::SimpleMath::Color Colour;
		bool DropShadow;
		std::weak_ptr<DirectX::SpriteFont> SpriteFont;
		std::weak_ptr<Graphics> WindowGraphicsController;

	public:
		//Constructors and Destructor
		BoundedText(std::weak_ptr<Graphics> graphicsController, const std::wstring& textString, const std::wstring& spriteFontPath, DirectX::SimpleMath::Rectangle boundingRectangle, PrintLinesOrientationClass printLinesOrientation, TextAlignmentClass textAlignment, DirectX::SimpleMath::Color colour = DirectX::Colors::White.v, bool dropShadow = true);

		//Getters
		DirectX::SimpleMath::Rectangle GetBoundingRectangle() const noexcept;

		//Setters
		void SetTextString(const std::wstring& textString);
		void SetDropShadow(bool dropShadow) noexcept;
		void SetColour(DirectX::SimpleMath::Color colour) noexcept;

		//Overriden functions
		void Draw() override;
		bool IsCoordInObject(DirectX::XMINT2 mousePos) override;
	};
}
*/