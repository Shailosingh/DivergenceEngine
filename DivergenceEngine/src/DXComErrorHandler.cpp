#include "DXComErrorHandler.h"
#include "StringConverter.h"

namespace DX
{
    const char* com_exception::what() const noexcept
    {
        _com_error error(result);
        std::wstring wideErrorMessage = error.ErrorMessage();
        std::string errorMessage = DivergenceEngine::StringConverter::ConvertWideStringToANSI(wideErrorMessage);
        whatMessage = std::format("Failure with HRESULT of {:#0x}\nEXCEPTION: {}", static_cast<const unsigned int>(result), errorMessage);
        return whatMessage.c_str();
    }
}