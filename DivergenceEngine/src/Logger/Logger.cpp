#include "Logger.h"
#include <Windows.h>
#include <format>
#include <filesystem>

//https://en.cppreference.com/w/cpp/utility/source_location

namespace fs = std::filesystem;

namespace DivergenceEngine
{
	void Logger::Log(std::wstring message, std::source_location location)
	{
		std::wstring output = std::format(L"DIVERGENCE ENGINE: File: {} ({}:{}) '{}': {}\n", 
			ConvertNarrowStringToWideString(fs::path(location.file_name()).filename().string()),
			location.line(), 
			location.column(), 
			ConvertNarrowStringToWideString(location.function_name()),
			message);

		OutputDebugString(output.c_str());
	}

	std::wstring Logger::ConvertNarrowStringToWideString(const std::string& narrowString)
	{
		//This method only works with single byte strings. Not UTF-8 strings with multibyte chars
		//std::wstring wideString(narrowString.begin(), narrowString.end());

		//Allocates a wstring with appropriate number of characters (except for null) to take in the converted narrow string
		int numberOfCharactersInWideString = MultiByteToWideChar(CP_UTF8, 0, narrowString.c_str(), -1, NULL, 0) - 1;
		std::wstring wideString(numberOfCharactersInWideString, 0);

		//Fills and returns the wide string
		MultiByteToWideChar(CP_UTF8, 0, narrowString.c_str(), -1, &wideString[0], numberOfCharactersInWideString);

		return wideString;
	}
}