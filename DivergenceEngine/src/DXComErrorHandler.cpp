#include "DXComErrorHandler.h"

namespace DX
{
    const char* com_exception::what() const noexcept
    {
        _com_error error(result);
        std::wstring wideErrorMessage = error.ErrorMessage();
        std::string errorMessage = ConvertWideStringToANSI(wideErrorMessage);
        whatMessage = std::format("Failure with HRESULT of {:#0x}\nEXCEPTION: {}", static_cast<const unsigned int>(result), errorMessage);
        return whatMessage.c_str();
    }

    std::string com_exception::ConvertWideStringToANSI(const std::wstring& wideString) const
    {
        //Allocates a string with appropriate number of characters (except for null) to take in the converted wide string
        int numberOfCharactersInANSIString = WideCharToMultiByte(CP_ACP, 0, wideString.c_str(), -1, NULL, 0, NULL, NULL) - 1;
        std::string ansiString(numberOfCharactersInANSIString, 0);

        //Fills and returns the ANSI string
        WideCharToMultiByte(CP_ACP, 0, wideString.c_str(), -1, &ansiString[0], numberOfCharactersInANSIString, NULL, NULL);

        return ansiString;
    }
}