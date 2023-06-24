#include "Application.h"
#include "Logger/Logger.h"

namespace DivergenceEngine
{
	Application::Application(uint32_t frameRate, LPWSTR lpCmdLine):
		CommandLineArgs(lpCmdLine)
	{
		//This is a game engine. A game will probably have 5 windows max. Reserve enough memory for that and if there is more, there will be reallocation
		ListOfApplicationWindows.reserve(5);

		//Set the initial data for the Window
		ProcessInstance = GetModuleHandle(nullptr);

		//Take in the input fps and if it is 0, set it to 60
		if (frameRate == 0)
		{
			FrameRate = 60;
		}
		else
		{
			FrameRate = frameRate;
		}

		//By default, set the timer to 60 fps
		Timer.SetFixedTimeStep(true);
		Timer.SetTargetElapsedSeconds(1.0 / (double)FrameRate);
		
		DivergenceEngine::Logger::Log(L"Application Constructed");
	}

	Application::~Application()
	{
		DivergenceEngine::Logger::Log(L"Application Destructed");
	
	}

	uint32_t Application::GetFrameRate()
	{
		return FrameRate;
	}

	void Application::AddWindow(std::unique_ptr<Window>&& window)
	{
		ListOfApplicationWindows.push_back(std::move(window));
	}

	void Application::RemoveWindow(HWND windowHandle)
	{
		for (int index = 0; index < ListOfApplicationWindows.size(); index++)
		{
			if (ListOfApplicationWindows[index]->IsEqualHandle(windowHandle))
			{
				ListOfApplicationWindows.erase(ListOfApplicationWindows.begin() + index);
				break;
			}
		}

		if (ListOfApplicationWindows.size() == 0)
		{
			PostQuitMessage(0);
		}
	}

	void Application::UpdateAndDrawAllWindows()
	{
		//Ensure the update and draw only happens at the target frame rate
		Timer.Tick([&]()
			{
				//Cycle through every window and draw them
				for (std::unique_ptr<Window>& window : ListOfApplicationWindows)
				{
					window->UpdateAndDraw(Timer);
				}
			});
	}
}