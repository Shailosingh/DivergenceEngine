#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <memory>
#include "Window/Window.h"
#include "StepTimer.h"

namespace DivergenceEngine
{
	class Application
	{
	private:
		inline static std::vector<std::unique_ptr<Window>> ListOfApplicationWindows;
		inline static DX::StepTimer Timer;
		
	protected:
		HINSTANCE ProcessInstance = nullptr;
		std::wstring CommandLineArgs = L"";
		uint32_t FramesPerSecond = 60;
		
	public:
		//Constructors and Destructors
		Application(uint32_t frameRate);
		virtual ~Application() = 0;

		//Initialization functions
		virtual void Initialize() = 0;

		//Window management functions
		static void AddWindow(std::unique_ptr<Window>&& window);
		static void RemoveWindow(HWND windowHandle);
		static void UpdateAndDrawAllWindows();
	};

	//USER MUST DEFINE THIS FUNCTION
	Application* CreateApplication(LPWSTR lpCmdLine);
}