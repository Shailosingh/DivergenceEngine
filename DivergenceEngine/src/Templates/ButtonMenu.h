#pragma once
#include "Templates/UnclickableDrawable.h"
#include <functional>
#include <memory>
#include <vector>

namespace DivergenceEngine::Templates
{
	class ButtonMenu : public IDrawable
	{
	public:
		//The Default and Hover visuals should always be in the same position and size
		struct ButtonDesc
		{
			std::shared_ptr<IDrawable> DefaultVisual; //Visual of button without hovering
			std::shared_ptr<IDrawable> HoverVisual; //Visual of button while cursor hovers
			std::function<void()> OnClickEvent;
		};

		//Constructors & Destructors
		ButtonMenu(std::vector<ButtonDesc> listOfButtonDescriptions);
		
		//Overriden Functions
		void Draw() override;
		bool IsCoordInObject(DirectX::XMINT2 mousePos) override;
		void OnMouseOver(DirectX::XMINT2 mousePos) override;
		void OnMouseNotOver() override;
		bool OnLeftPress(DirectX::XMINT2 mousePos) override;
		
		bool OnLeftRelease(DirectX::XMINT2 mousePos) override { return false; }
		bool OnMiddlePress(DirectX::XMINT2 mousePos) override { return false; }
		bool OnMiddleRelease(DirectX::XMINT2 mousePos) override { return false; }
		bool OnRightPress(DirectX::XMINT2 mousePos) override { return false; }
		bool OnRightRelease(DirectX::XMINT2 mousePos) override { return false; }

	private:
		struct Button
		{
			ButtonDesc ButtonDescription;
			bool IsCursorHoveringOver;
		};

		//Datafields
		std::vector<Button> ButtonList;
	};
}