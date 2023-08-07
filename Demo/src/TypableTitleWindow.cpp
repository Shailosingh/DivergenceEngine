#include "TypableTitleWindow.h"

TypableTitleWindow::TypableTitleWindow(uint16_t clientWidth, uint16_t clientHeight, const wchar_t* windowTitle)
	:Window(clientWidth, clientHeight, windowTitle)
{
	//Load image on background layer
	std::shared_ptr<DivergenceEngine::Templates::Image> backgroundImage = std::make_shared<DivergenceEngine::Templates::Image>(L"Images\\Plains.jpg", GraphicsController, DirectX::SimpleMath::Vector2(0, 0));
	this->AddDrawableComponent(backgroundImage, 0);

	//Load image on foreground
	std::shared_ptr<DivergenceEngine::Templates::Image> menuImage = std::make_shared<DivergenceEngine::Templates::Image>(L"Images\\Bison.png", GraphicsController, DirectX::SimpleMath::Vector2(350, 200), DirectX::SimpleMath::Vector2(300, 207));
	this->AddDrawableComponent(menuImage, 0);
	
	DivergenceEngine::Logger::Log(L"TypableTitleWindow constructed");
	
}

TypableTitleWindow::~TypableTitleWindow()
{
	DivergenceEngine::Logger::Log(L"TypableTitleWindow destructed");
}

void TypableTitleWindow::UpdateWindow(const DX::StepTimer& timer)
{
	//Handle the char queue
	while (!KeyboardObject.CharQueueIsEmpty())
	{
		UpdateWindowTitle(*KeyboardObject.ReadChar());
	}

	//Handle the key queue
	while (!KeyboardObject.KeyQueueIsEmpty())
	{
		//Get the key code
		DivergenceEngine::Keyboard::Event currentEvent = *KeyboardObject.ReadKey();

		//If the key is pressed
		if (currentEvent.IsPress())
		{
			//If the key is escape, send WM_CLOSE
			if (currentEvent.GetCode() == VK_ESCAPE)
			{
				PostMessage(WindowHandle, WM_CLOSE, 0, 0);
			}
		}
	}
}

void TypableTitleWindow::UpdateWindowTitle(WPARAM characterCode)
{
	std::vector<WPARAM> INVALID_CHARS = { '\n', 0x0D, 0x1B }; //'\n', carriage return, escape
	
	//Ensure that the character is valid
	if (std::find(INVALID_CHARS.begin(), INVALID_CHARS.end(), characterCode) != INVALID_CHARS.end())
	{
		return;
	}

	//Process backspace
	if (characterCode == '\b')
	{
		if (WindowTitle.empty())
		{
			return;
		}

		WindowTitle.pop_back();
	}
	
	//Add the character to the window title, if no other valid nondisplayable character is pressed
	else
	{
		WindowTitle += (wchar_t)characterCode;
	}

	//Set the window title
	this->SetWindowTitle(WindowTitle);
}