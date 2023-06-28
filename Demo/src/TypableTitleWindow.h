#pragma once
#include "DivergenceEngine.h"

class TypableTitleWindow : public DivergenceEngine::Window
{	
private:
	void UpdateWindowTitle(WPARAM characterCode);
	
	//Overridden functions
	void UpdateWindow(const DX::StepTimer& timer) override;
	
public:
	//Constructors and destructors
	TypableTitleWindow(uint16_t clientWidth, uint16_t clientHeight, const wchar_t* windowTitle);
	~TypableTitleWindow();
};