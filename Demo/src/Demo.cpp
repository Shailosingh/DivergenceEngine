#include "Demo.h"
#include <format>
#include "TypableTitleWindow.h"

//Sends the Application object to the Engine
DivergenceEngine::Application* DivergenceEngine::CreateApplication(LPWSTR lpCmdLine)
{
	return new Demo(lpCmdLine, 60);
}

//Class functions----------------------------------------------------------------------------------
Demo::Demo(LPWSTR lpCmdLine, uint32_t frameRate)
	:Application(frameRate)
{
	ProcessInstance = GetModuleHandle(nullptr);
	CommandLineArgs = lpCmdLine;
	DivergenceEngine::Logger::Log(L"Demo Constructed");
}

Demo::~Demo()
{
	DivergenceEngine::Logger::Log(L"Demo Destructed");
}

void Demo::Initialize()
{
	AddWindow(std::make_unique<TypableTitleWindow>(800, 450, L"Window 1", RemoveWindow));
	AddWindow(std::make_unique<TypableTitleWindow>(450, 800, L"Window 2", RemoveWindow));
	AddWindow(std::make_unique<TypableTitleWindow>(800, 800, L"Window 3", RemoveWindow));

	DivergenceEngine::Logger::Log(L"Demo Initialized");
}