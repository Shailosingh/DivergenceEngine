#include "Application.h"
#include "Logger/Logger.h"

namespace DivergenceEngine
{
	Application::Application()
	{
		//This is a game engine. A game will probably have 5 windows max. Reserve enough memory for that and if there is more, there will be reallocation
		ListOfApplicationWindows.reserve(5);

		//By default, set the timer to 60 fps
		Timer.SetFixedTimeStep(true);
		Timer.SetTargetElapsedSeconds(1.0 / 30.0);
		
		DivergenceEngine::Logger::Log(L"Application Constructed");
	}

	Application::~Application()
	{
		DivergenceEngine::Logger::Log(L"Application Destructed");
	
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