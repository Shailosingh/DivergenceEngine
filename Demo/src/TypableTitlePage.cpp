#include "TypableTitlePage.h"
#include "DefaultFonts.h"
#include <format>

void TypableTitlePage::Initialize(DivergenceEngine::Window* windowReference)
{
	//Load in the window pointer
	WindowReference = windowReference;

	//Get the buffer size
	DirectX::XMINT2 bufferSize = WindowReference->GraphicsController->GetBufferSize();
	
	//Load image on foreground
	std::shared_ptr<DivergenceEngine::Templates::Image> bisonImage = std::make_shared<DivergenceEngine::Templates::Image>(L"Images\\TypableTitlePage\\Bison.png", WindowReference->GraphicsController, DirectX::SimpleMath::Vector2(350, 200), DirectX::SimpleMath::Vector2(300, 207));
	WindowReference->AddDrawableComponent(bisonImage, 1);

	//Load image on background layer
	std::shared_ptr<DivergenceEngine::Templates::Image> backgroundImage = std::make_shared<DivergenceEngine::Templates::Image>(L"Images\\TypableTitlePage\\Plains.jpg", WindowReference->GraphicsController, DirectX::SimpleMath::Vector2(0, 0));
	WindowReference->AddDrawableComponent(backgroundImage, 0);

	//Load hello world text onto screen (NOTE: You must do .v to the DirectX::Colors::White to get the color in the correct format for the PlainText constructor, otherwise it will be a compile error)
	ScreenText = std::make_shared<DivergenceEngine::Templates::PlainText>(WindowReference->GraphicsController, L"The quick brown fox jumps over the lazy dog!", DefaultFonts::GetFontPath(DefaultFonts::DefaultFontIndices::M_PLUS_1_Size20), DirectX::SimpleMath::Vector2(bufferSize.x/2, 400), DivergenceEngine::Templates::PlainText::TextOriginClass::Centre, DirectX::Colors::White.v, true);
	WindowReference->AddDrawableComponent(ScreenText, 2);

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