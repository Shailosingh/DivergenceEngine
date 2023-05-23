#pragma once
#include <string>
#include <source_location>

namespace DivergenceEngine
{
	class Logger
	{
	public:
		static void Log(std::wstring message, std::source_location location = std::source_location::current());

	private:
		static std::wstring ConvertNarrowStringToWideString(const std::string& narrowString);
	};
}