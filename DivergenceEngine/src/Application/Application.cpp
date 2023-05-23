#include "Application.h"
#include "Logger/Logger.h"

namespace DivergenceEngine
{
	Application::Application()
	{
		DivergenceEngine::Logger::Log(L"Application Constructed");
	}

	Application::~Application()
	{
		DivergenceEngine::Logger::Log(L"Application Destructed");
	
	}
}