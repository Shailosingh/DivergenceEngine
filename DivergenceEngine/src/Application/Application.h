#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include "Window/Window.h"

namespace DivergenceEngine
{
	class Application
	{
	private:
		inline static std::vector<Window> ListOfApplicationWindows;
		
	protected:
		HINSTANCE processInstance = nullptr;
		std::wstring commandLineArgs = L"";
		
	public:
		//Constructors and Destructors
		Application();
		virtual ~Application() = 0;

		//Initialization functions
		virtual void Initialize() = 0;

		//Window management functions
		static void AddWindow(Window& window);
		static void RemoveWindow(HWND windowHandle);
		static void DrawAllWindows();
	};

	//USER MUST DEFINE THIS FUNCTION
	Application* CreateApplication(LPWSTR lpCmdLine);
}