#pragma once
#include "DivergenceEngine.h"

class TypableTitleWindow : public DivergenceEngine::Window
{	
private:
	void UpdateWindowTitle(WPARAM characterCode);
	
public:
	//Constructors and destructors
	TypableTitleWindow(uint16_t clientWidth, uint16_t clientHeight, const wchar_t* windowTitle, DivergenceEngine::SignalWindowDestructionFunction signalFunction);
	~TypableTitleWindow();
	
	void UpdateWindow() override;
};