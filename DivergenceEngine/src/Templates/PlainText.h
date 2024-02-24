#pragma once
#include "Templates/UnclickableDrawable.h"
#include "Graphics/Graphics.h"
#include <string>

namespace DivergenceEngine::Templates
{
	class PlainText : public DivergenceEngine::UnclickableDrawable
	{
	public:
		enum class TextOriginClass
		{
			TopLeft,
			TopCentre,
			TopRight,
			Centre,
			BottomLeft,
			BottomCentre,
			BottomRight
		};

	private:
		//Datafields
		std::wstring TextString;
		DirectX::SimpleMath::Vector2 PositionCoord;
		DirectX::SimpleMath::Vector2 OriginCoord;
		DirectX::FXMVECTOR Colour;
		bool DropShadow;
		std::weak_ptr<DirectX::SpriteFont> SpriteFont;
		std::weak_ptr<Graphics> WindowGraphicsController;

		//Helpers
		DirectX::SimpleMath::Vector2 ComputeOrigin(TextOriginClass originClass) noexcept;

	public:
		//Constructors and Destructor
		PlainText(std::weak_ptr<Graphics> graphicsController, const std::wstring& textString, const std::wstring& spriteFontPath, DirectX::SimpleMath::Vector2 positionCoord, TextOriginClass originClass = TextOriginClass::TopLeft, DirectX::FXMVECTOR colour = DirectX::Colors::White, bool dropShadow = false);

		//Setters
		void SetTextString(const std::wstring& textString);

		//Overriden functions
		void Draw() override;
		bool IsCoordInObject(DirectX::XMINT2 mousePos) override;
	};
}