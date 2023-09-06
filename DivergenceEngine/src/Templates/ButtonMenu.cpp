#include "ButtonMenu.h"

namespace DivergenceEngine::Templates
{
	ButtonMenu::ButtonMenu(std::vector<ButtonDesc> listOfButtonDescriptions)
	{
		for (auto& buttonDesc : listOfButtonDescriptions)
		{
			ButtonList.push_back(
				{ 
					.ButtonDescription=buttonDesc, 
					.IsCursorHoveringOver=false 
				});
		}
	}
	
	void ButtonMenu::Draw()
	{
		for (auto& button : ButtonList)
		{
			if (button.IsCursorHoveringOver)
			{
				button.ButtonDescription.HoverVisual->Draw();
			}
			else
			{
				button.ButtonDescription.DefaultVisual->Draw();
			}
		}
	}

	bool ButtonMenu::IsCoordInObject(DirectX::XMINT2 mousePos)
	{
		for (auto& button : ButtonList)
		{
			if (button.ButtonDescription.DefaultVisual->IsCoordInObject(mousePos))
			{
				return true;
			}
		}
		return false;
	}

	void ButtonMenu::OnMouseOver(DirectX::XMINT2 mousePos)
	{
		for (auto& button : ButtonList)
		{	
			bool isCursorHoveringOver = button.ButtonDescription.DefaultVisual->IsCoordInObject(mousePos);
			if (isCursorHoveringOver && !button.IsCursorHoveringOver)
			{
				//TODO: Maybe do something here to trigger a sound for when a button is hovered
				button.IsCursorHoveringOver = true;
			}
			else if(!isCursorHoveringOver)
			{
				button.IsCursorHoveringOver = false;
			}
		}
	}

	void ButtonMenu::OnMouseNotOver()
	{
		for (auto& button : ButtonList)
		{
			button.IsCursorHoveringOver = false;
		}
	}

	bool ButtonMenu::OnLeftPress(DirectX::XMINT2 mousePos)
	{
		for (auto& button : ButtonList)
		{
			if (button.ButtonDescription.DefaultVisual->IsCoordInObject(mousePos))
			{
				button.ButtonDescription.OnClickEvent();
				return true;
			}
		}
		return false;
	}
}