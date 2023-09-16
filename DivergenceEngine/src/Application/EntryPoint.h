#pragma once
#include <Windows.h>
#include "Logger/Logger.h"
#include "Application/Application.h"
#include "Window/Window.h"
#include "DXComErrorHandler.h"

//External function that shall setup the application
extern DivergenceEngine::Application* DivergenceEngine::CreateApplication(LPWSTR lpCmdLine);

//Different versions of entry point
int WINAPI ReleaseEntryPoint(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow);
int WINAPI DebugEntryPoint(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow);

//Entry point of program
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	int returnCode = 0;

	//Call the respective debug or release entry point
#ifndef NDEBUG
	DivergenceEngine::Logger::Log(L"DEBUG MODE");
	returnCode = DebugEntryPoint(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
#else
	DivergenceEngine::Logger::Log(L"RELEASE MODE");
	returnCode = ReleaseEntryPoint(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
#endif

	return returnCode;
}

int WINAPI DebugEntryPoint(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	DivergenceEngine::Logger::Log(L"Welcome to the Divergence Engine! Here we take the road less travelled by...");

	//Retrieve the Application object from the user
	DivergenceEngine::Application* application = DivergenceEngine::CreateApplication(lpCmdLine);

	//Initialize the application (all windows and other info)
	application->Initialize();

	//Pump message loop
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
			application->UpdateAndDrawAllWindows();
		}
	}

	//Cleanup 
	delete application;
	return (int)incomingMessage.wParam;
}

int WINAPI ReleaseEntryPoint(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	DivergenceEngine::Logger::Log(L"Welcome to the Divergence Engine! Here we take the road less travelled by...");

	//Throw MessageBox for any unhandled exceptions during initialization of application and then exit
	DivergenceEngine::Application* application;
	try
	{
		//Retrieve the Application object from the user
		application = DivergenceEngine::CreateApplication(lpCmdLine);

		//Initialize the application (all windows and other info)
		application->Initialize();
	}
	catch (const std::exception& e)
	{
		MessageBoxA(NULL, e.what(), "Error During Initialization", MB_OK | MB_ICONERROR);
		return -1;
	}

	//Pump message loop and put up message box for any unhandled exceptions. Terminate immediately, without cleanup
	MSG incomingMessage = { 0 };
	try
	{
		while (incomingMessage.message != WM_QUIT)
		{
			if (PeekMessage(&incomingMessage, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&incomingMessage);
				DispatchMessage(&incomingMessage);
			}

			else
			{
				application->UpdateAndDrawAllWindows();
			}
		}
	}
	catch (const std::exception& e)
	{
		MessageBoxA(NULL, e.what(), "Error In Message Loop", MB_OK | MB_ICONERROR);
		return -1;
	}

	//Cleanup and throw a dialog message for any unhandled exceptiond during cleanup. Terminate immediately.
	try
	{
		delete application;
	}
	catch (std::exception& e)
	{
		MessageBoxA(NULL, e.what(), "Error On Exit Of Game", MB_OK | MB_ICONERROR);
	}

	return (int)incomingMessage.wParam;
}