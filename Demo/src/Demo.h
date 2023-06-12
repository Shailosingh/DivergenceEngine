#pragma once
#include <DivergenceEngine.h>
#include "Application/EntryPoint.h"

class Demo : public DivergenceEngine::Application
{
public:
	Demo(LPWSTR lpCmdLine);
	~Demo() override;
	
	void Initialize() override;
};