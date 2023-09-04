#include "TypableTitlePage.h"
#include <format>

void TypableTitlePage::Initialize(DivergenceEngine::Window* windowReference)
{
	//Load in the window pointer
	WindowReference = windowReference;
	
	//Load image on foreground
	std::shared_ptr<DivergenceEngine::Templates::Image> bisonImage = std::make_shared<DivergenceEngine::Templates::Image>(L"Images\\Bison.png", WindowReference->GraphicsController, DirectX::SimpleMath::Vector2(350, 200), DirectX::SimpleMath::Vector2(300, 207));
	WindowReference->AddDrawableComponent(bisonImage, 1);

	//Load image on background layer
	std::shared_ptr<DivergenceEngine::Templates::Image> backgroundImage = std::make_shared<DivergenceEngine::Templates::Image>(L"Images\\Plains.jpg", WindowReference->GraphicsController, DirectX::SimpleMath::Vector2(0, 0));
	WindowReference->AddDrawableComponent(backgroundImage, 0);

	DivergenceEngine::Logger::Log(L"TypableTitlePage constructed");

}

TypableTitlePage::~TypableTitlePage()
{
	DivergenceEngine::Logger::Log(L"TypableTitlePage destructed");
}

void TypableTitlePage::UpdatePage(const DX::StepTimer& timer)
{
	//Handle the char queue
	while (!WindowReference->KeyboardObject.CharQueueIsEmpty())
	{
		//Read the character
		wchar_t typedChar = *WindowReference->KeyboardObject.ReadChar();

		switch (typedChar)
		{
			//Escape Key
		case VK_ESCAPE:
			PostMessage(WindowReference->GetHandle() , WM_CLOSE, 0, 0);
			break;

			//Enter key
		case VK_RETURN:
			WindowReference->ToggleFullscreen();
			break;

			//Typable char
		default:
			UpdateWindowTitle(typedChar);
			break;
		}
	}

	//Handle the key queue
	while (!WindowReference->KeyboardObject.KeyQueueIsEmpty())
	{
		//Get the key code
		DivergenceEngine::Keyboard::Event currentEvent = *WindowReference->KeyboardObject.ReadKey();

		//If the key is pressed
		if (currentEvent.IsPress())
		{
			//DivergenceEngine::Logger::RawLog(std::format(L"Pressed Key Code: {:#02x}", currentEvent.GetCode()));
		}

		else
		{
			//DivergenceEngine::Logger::RawLog(std::format(L"Released Key Code: {:#02x}", currentEvent.GetCode()));
		}
	}
	
	//TODO: Create a Menu IDrawable that will take a pair of images for each menu item (default and hover), the coords and of each item and delegates/function pointers to handle selection
}

void TypableTitlePage::HandleScroll(int scrollDelta)
{
	//TypableTitlePage doesn't scroll
}

void TypableTitlePage::HandleMouseMove(DirectX::XMINT2 newMousePos)
{
	//Doesn't respond to mouse events
}

bool TypableTitlePage::OnWindowDestructionRequest()
{
	return true;
}

void TypableTitlePage::UpdateWindowTitle(WPARAM characterCode)
{
	std::vector<WPARAM> INVALID_CHARS = { '\n', 0x0D, 0x1B }; //'\n', carriage return, escape
	
	//Get the current window title
	std::wstring currentWindowTitle = WindowReference->GetWindowTitle();

	//Ensure that the character is valid
	if (std::find(INVALID_CHARS.begin(), INVALID_CHARS.end(), characterCode) != INVALID_CHARS.end())
	{
		return;
	}

	//Process backspace
	if (characterCode == '\b')
	{
		if (currentWindowTitle.empty())
		{
			return;
		}

		currentWindowTitle.pop_back();
	}

	//Add the character to the window title, if no other valid nondisplayable character is pressed
	else
	{
		currentWindowTitle += (wchar_t)characterCode;
	}

	//Set the window title
	WindowReference->SetWindowTitle(currentWindowTitle);
}