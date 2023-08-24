#pragma once
#include <Windows.h>
#include <exception>
#include <stdio.h>
#include <comdef.h>
#include <string>
#include <format>
#include "Logger/Logger.h"

namespace DX
{
    // Helper class for COM exceptions
    class com_exception : public std::exception
    {
    private:
        const HRESULT result;
        mutable std::string whatMessage;
        
        std::string ConvertWideStringToANSI(const std::wstring& wideString) const;
        
    public:
        com_exception(HRESULT hr) noexcept : result(hr) {}

        const char* what() const noexcept override;
    };

    // Helper utility converts D3D API failures into exceptions.
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            _com_error error(hr);
            std::wstring errorMessage = std::format(L"EXCEPTION: {}", error.ErrorMessage());
            DivergenceEngine::Logger::Log(errorMessage);
            throw com_exception(hr);
        }
    }
}

