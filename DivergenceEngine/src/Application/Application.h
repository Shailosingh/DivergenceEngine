#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <memory>
#include "Window/Window.h"
#include "StepTimer.h"
#include "Window/IPage.h"


namespace DivergenceEngine
{
	class Application
	{
	private:
		inline static std::vector<std::unique_ptr<Window>> ListOfApplicationWindows;
		inline static DX::StepTimer Timer;
		inline static uint32_t FrameRate;
		
	protected:
		HINSTANCE ProcessInstance = nullptr;
		std::wstring CommandLineArgs = L"";
		
	public:
		//Constructors and Destructors
		Application(uint32_t frameRate, LPWSTR lpCmdLine);
		virtual ~Application() = 0;

		//Getters
		static uint32_t GetFrameRate();

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