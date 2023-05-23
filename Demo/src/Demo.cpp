#include "Demo.h"

//Sends the Application object to the Engine
DivergenceEngine::Application* DivergenceEngine::CreateApplication(HINSTANCE hInstance, LPWSTR lpCmdLine)
{
	return new Demo(hInstance, lpCmdLine);
}

//Class functions----------------------------------------------------------------------------------
Demo::Demo(HINSTANCE hInstance, LPWSTR lpCmdLine)
{
	processInstance = hInstance;
	commandLineArgs = lpCmdLine;
	DivergenceEngine::Logger::Log(L"Demo Constructed");
}

Demo::~Demo()
{
	DivergenceEngine::Logger::Log(L"Demo Destructed");
}

void Demo::Initialize()
{
	DivergenceEngine::Logger::Log(L"Demo Initialized");
}