#pragma once
#include "DivergenceEngine.h"

class TypableTitlePage : public DivergenceEngine::IPage
{
private:
	//Datafields
	DivergenceEngine::Window* WindowReference;
	
	//Private helpers
	void UpdateWindowTitle(WPARAM characterCode);

public:
	//Constructors and destructors
	void Initialize(DivergenceEngine::Window* windowReference) override;
	~TypableTitlePage();

	//Overridden functions
	void UpdateWindow(const DX::StepTimer& timer) override;
	bool OnWindowDestructionRequest() override;
};