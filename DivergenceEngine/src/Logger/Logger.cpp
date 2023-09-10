#include "Logger.h"
#include <Windows.h>
#include <format>
#include <filesystem>
#include <StringConverter.h>

//https://en.cppreference.com/w/cpp/utility/source_location

namespace fs = std::filesystem;

namespace DivergenceEngine
{
	void Logger::Log(std::wstring message, std::source_location location)
	{
		std::wstring output = std::format(L"DIVERGENCE ENGINE: File: {} ({}:{}) '{}': {}\n", 
			DivergenceEngine::StringConverter::ConvertNarrowStringToWideString(fs::path(location.file_name()).filename().string()),
			location.line(), 
			location.column(), 
			DivergenceEngine::StringConverter::ConvertNarrowStringToWideString(location.function_name()),
			message);

		OutputDebugString(output.c_str());
	}

	void Logger::RawLog(std::wstring message)
	{
		message += '\n';
		OutputDebugString(message.c_str());
	}

	
}