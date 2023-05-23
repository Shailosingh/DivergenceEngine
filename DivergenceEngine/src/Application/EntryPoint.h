#pragma once
#include <Windows.h>
#include "Logger/Logger.h"
#include "Application/Application.h"

//External function that shall setup the application
extern DivergenceEngine::Application* DivergenceEngine::CreateApplication(HINSTANCE hInstance, LPWSTR lpCmdLine);

//Entry point of program
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	DivergenceEngine::Logger::Log(L"Welcome to the Divergence Engine! Here we take the road less travelled by...");

	//Retrieve the Application object from the user
	DivergenceEngine::Application* application = DivergenceEngine::CreateApplication(hInstance, lpCmdLine);
	
	//Initialize the application (all windows and other info)
	application->Initialize();

	//TODO: Message loop
	MSG incomingMessage = { 0 };
	while (incomingMessage.message != WM_QUIT)
	{
		if (PeekMessage(&incomingMessage, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&incomingMessage);
			DispatchMessage(&incomingMessage);
		}

		else
		{
			//TODO: Update frame
		}
	}
	
	//Cleanup
	delete application;
	
	return incomingMessage.wParam;
}