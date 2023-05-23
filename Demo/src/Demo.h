#pragma once
#include <DivergenceEngine.h>

class Demo : public DivergenceEngine::Application
{
public:
	Demo(HINSTANCE hInstance, LPWSTR lpCmdLine);
	~Demo() override;
	
	void Initialize() override;
};