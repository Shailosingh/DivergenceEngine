#pragma once
#include "DivergenceEngine.h"

class MainMenuPage : public DivergenceEngine::IPage
{
private:
	//Datafields
	DivergenceEngine::Window* WindowReference = nullptr;
	std::unique_ptr<DivergenceEngine::IAudioInstance> BackgroundMusic;
	std::unique_ptr<DivergenceEngine::ISimpleSoundEffect> MenuSoundEffect;
	bool IsMusicPlaying = true;

	//Menu callback functions
	void Menu_Start();
	void Menu_Load();
	void Menu_Replay();
	void Menu_Options();
	void Menu_Exit();

public:
	//Constructors and destructors
	void Initialize(DivergenceEngine::Window* windowReference) override;
	~MainMenuPage();

	//Overridden functions
	void UpdatePage(const DX::StepTimer& timer) override;
	void HandleScroll(int scrollDelta) override {}
	void HandleMouseMove(DirectX::XMINT2 newMousePos) override {}
	bool OnWindowDestructionRequest() override { return true; }
};