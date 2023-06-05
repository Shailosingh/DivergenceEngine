#pragma once
#include <Windows.h>
#include <string>

namespace DivergenceEngine
{
	class Application
	{
	protected:
		HINSTANCE processInstance = nullptr;
		std::wstring commandLineArgs = L"";
		
	public:
		//Constructors and Destructors
		Application();
		virtual ~Application() = 0;

		//Initialization functions
		virtual void Initialize() = 0;
	};

	//USER MUST DEFINE THIS FUNCTION
	Application* CreateApplication(LPWSTR lpCmdLine);
}