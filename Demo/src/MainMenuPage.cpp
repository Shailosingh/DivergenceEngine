#include "MainMenuPage.h"
#include "TypableTitlePage.h"

using namespace DivergenceEngine::Templates;

void MainMenuPage::Initialize(DivergenceEngine::Window* windowReference)
{
	WindowReference = windowReference;

	//Load background music
	BackgroundMusic = std::make_unique<DivergenceEngine::WAVAudioInstance>(WindowReference->AudioController.get(), L"Audio\\MainMenu.wav", 1);
	BackgroundMusic->Play(true);
	
	//Load image on foreground
	std::shared_ptr<Image> menuSplash = std::make_shared<Image>(L"Images\\MainMenuPage\\TITLE.png", WindowReference->GraphicsController, DirectX::SimpleMath::Vector2(0, 0));
	WindowReference->AddDrawableComponent(menuSplash, 1);

	//Load image on background layer
	std::shared_ptr<Image> backgroundImage = std::make_shared<Image>(L"Images\\MainMenuPage\\TITLEBASE.PNG", WindowReference->GraphicsController, DirectX::SimpleMath::Vector2(0, 0));
	WindowReference->AddDrawableComponent(backgroundImage, 0);

	//Setup main menu
	std::vector<ButtonMenu::ButtonDesc> buttonDescriptions;
	buttonDescriptions.reserve(5);
	
	std::shared_ptr<Image> startImageHover = std::make_shared<Image>(L"Images\\MainMenuPage\\START.png", WindowReference->GraphicsController, DirectX::SimpleMath::Vector2(112, 356));
	std::shared_ptr<InvisibleDrawable> startImageDefault = std::make_shared<InvisibleDrawable>(DirectX::SimpleMath::Vector2(112, 356), startImageHover->GetSize());
	buttonDescriptions.push_back(
		ButtonMenu::ButtonDesc
		{ 
			.DefaultVisual=startImageDefault, 
			.HoverVisual=startImageHover, 
			.OnClickEvent=std::bind(&MainMenuPage::Menu_Start, this) 
		});
	
	std::shared_ptr<Image> loadImageHover = std::make_shared<Image>(L"Images\\MainMenuPage\\LOAD.png", WindowReference->GraphicsController, DirectX::SimpleMath::Vector2(229, 356));
	std::shared_ptr<InvisibleDrawable> loadImageDefault = std::make_shared<InvisibleDrawable>(DirectX::SimpleMath::Vector2(229, 356), loadImageHover->GetSize());
	buttonDescriptions.push_back(
		ButtonMenu::ButtonDesc
		{
			.DefaultVisual = loadImageDefault,
			.HoverVisual = loadImageHover,
			.OnClickEvent = std::bind(&MainMenuPage::Menu_Load, this)
		});
	
	std::shared_ptr<Image> replayImageHover = std::make_shared<Image>(L"Images\\MainMenuPage\\REPLAY.png", WindowReference->GraphicsController, DirectX::SimpleMath::Vector2(346, 356));
	std::shared_ptr<InvisibleDrawable> replayImageDefault = std::make_shared<InvisibleDrawable>(DirectX::SimpleMath::Vector2(346, 356), replayImageHover->GetSize());
	buttonDescriptions.push_back(
		ButtonMenu::ButtonDesc
		{
			.DefaultVisual = replayImageDefault,
			.HoverVisual = replayImageHover,
			.OnClickEvent = std::bind(&MainMenuPage::Menu_Replay, this)
		});

	std::shared_ptr<Image> optionsImageHover = std::make_shared<Image>(L"Images\\MainMenuPage\\OPTION.png", WindowReference->GraphicsController, DirectX::SimpleMath::Vector2(463, 356));
	std::shared_ptr<InvisibleDrawable> optionsImageDefault = std::make_shared<InvisibleDrawable>(DirectX::SimpleMath::Vector2(463, 356), optionsImageHover->GetSize());
	buttonDescriptions.push_back(
		ButtonMenu::ButtonDesc
		{
			.DefaultVisual = optionsImageDefault,
			.HoverVisual = optionsImageHover,
			.OnClickEvent = std::bind(&MainMenuPage::Menu_Options, this)
		});
	
	std::shared_ptr<Image> exitImageHover = std::make_shared<Image>(L"Images\\MainMenuPage\\EXIT.png", WindowReference->GraphicsController, DirectX::SimpleMath::Vector2(580, 356));
	std::shared_ptr<InvisibleDrawable> exitImageDefault = std::make_shared<InvisibleDrawable>(DirectX::SimpleMath::Vector2(580, 356), exitImageHover->GetSize());
	buttonDescriptions.push_back(
		ButtonMenu::ButtonDesc
		{
			.DefaultVisual = exitImageDefault,
			.HoverVisual = exitImageHover,
			.OnClickEvent = std::bind(&MainMenuPage::Menu_Exit, this)
		});

	//Load menu onto top layer
	std::shared_ptr<ButtonMenu> mainMenu = std::make_shared<ButtonMenu>(buttonDescriptions);
	WindowReference->AddDrawableComponent(mainMenu, 2);

	DivergenceEngine::Logger::Log(L"MainMenuPage Constructed");
}

MainMenuPage::~MainMenuPage()
{
	DivergenceEngine::Logger::Log(L"MainMenuPage Destructed");
}

void MainMenuPage::UpdatePage(const DX::StepTimer& timer)
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
			PostMessage(WindowReference->GetHandle(), WM_CLOSE, 0, 0);
			break;

		//Enter key
		case VK_RETURN:
			WindowReference->ToggleFullscreen();
			break;

		//1 key
		case '1':
			BackgroundMusic->SetPlaybackSpeedMultiplier(1);
			break;
			
		//2 key
		case '2':
			BackgroundMusic->SetPlaybackSpeedMultiplier(2);
			break;

		//Space key
		case VK_SPACE:
			if (IsMusicPlaying)
			{
				BackgroundMusic->Pause();
			}

			else
			{
				BackgroundMusic->Resume();
			}
			IsMusicPlaying = !IsMusicPlaying;
			break;
		}		
	}
}

//Menu Callback Functions----------------------------------------------------------
void MainMenuPage::Menu_Start()
{
	WindowReference->QueueNewPage(std::make_unique<TypableTitlePage>());
}

void MainMenuPage::Menu_Load()
{

}

void MainMenuPage::Menu_Replay()
{

}

void MainMenuPage::Menu_Options()
{
	
}

void MainMenuPage::Menu_Exit()
{
	DivergenceEngine::Logger::Log(L"Exit Button Selected");
	PostMessage(WindowReference->GetHandle(), WM_CLOSE, 0, 0);
}