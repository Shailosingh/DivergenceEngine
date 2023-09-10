#pragma once
#include <string>
#include <Windows.h>

namespace DivergenceEngine
{
	class StringConverter
	{
	public:
		static std::wstring ConvertNarrowStringToWideString(const std::string& narrowString)
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

		static std::string ConvertWideStringToANSI(const std::wstring& wideString)
		{
			//Allocates a string with appropriate number of characters (except for null) to take in the converted wide string
			int numberOfCharactersInANSIString = WideCharToMultiByte(CP_ACP, 0, wideString.c_str(), -1, NULL, 0, NULL, NULL) - 1;
			std::string ansiString(numberOfCharactersInANSIString, 0);

			//Fills and returns the ANSI string
			WideCharToMultiByte(CP_ACP, 0, wideString.c_str(), -1, &ansiString[0], numberOfCharactersInANSIString, NULL, NULL);

			return ansiString;
		}
	};
}