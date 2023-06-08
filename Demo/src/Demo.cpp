#include "Demo.h"
#include <format>

//Sends the Application object to the Engine
DivergenceEngine::Application* DivergenceEngine::CreateApplication(LPWSTR lpCmdLine)
{
	return new Demo(lpCmdLine);
}

//Class functions----------------------------------------------------------------------------------
Demo::Demo(LPWSTR lpCmdLine)
{
	processInstance = GetModuleHandle(nullptr);
	commandLineArgs = lpCmdLine;
	DivergenceEngine::Logger::Log(L"Demo Constructed");
}

Demo::~Demo()
{
	DivergenceEngine::Logger::Log(L"Demo Destructed");
}

void Demo::Initialize()
{
	DivergenceEngine::Window window1(800, 450, L"Window 1", RemoveWindow);
	DivergenceEngine::Window window2(450, 800, L"Window 2", RemoveWindow);
	DivergenceEngine::Window window3(800, 800, L"Window 3", RemoveWindow);

	AddWindow(window1);
	AddWindow(window2);
	AddWindow(window3);

	DivergenceEngine::Logger::Log(L"Demo Initialized");
}