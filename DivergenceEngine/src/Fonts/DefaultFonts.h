#pragma once
#include <array>
#include <string>

namespace DivergenceEngine::DefaultFonts
{
	enum class DefaultFontIndices
	{
		M_PLUS_1_Size16,
		M_PLUS_1_Size18,
		M_PLUS_1_Size20,
		M_PLUS_1_Size24,

		M_PLUS_1_Medium_Size16,
		M_PLUS_1_Medium_Size18,
		M_PLUS_1_Medium_Size20,
		M_PLUS_1_Medium_Size24,

		TOTAL_DEFAULT_FONTS
	};

	const static std::array<std::wstring, static_cast<size_t>(DefaultFontIndices::TOTAL_DEFAULT_FONTS)> FontPaths
	{
		L"C:\\Code Projects\\C++ Projects\\DivergenceEngine\\DivergenceEngine\\src\\Fonts\\FontFiles\\M PLUS 1 (Size_16).spritefont",
		L"C:\\Code Projects\\C++ Projects\\DivergenceEngine\\DivergenceEngine\\src\\Fonts\\FontFiles\\M PLUS 1 (Size_18).spritefont",
		L"C:\\Code Projects\\C++ Projects\\DivergenceEngine\\DivergenceEngine\\src\\Fonts\\FontFiles\\M PLUS 1 (Size_20).spritefont",
		L"C:\\Code Projects\\C++ Projects\\DivergenceEngine\\DivergenceEngine\\src\\Fonts\\FontFiles\\M PLUS 1 (Size_24).spritefont",

		L"C:\\Code Projects\\C++ Projects\\DivergenceEngine\\DivergenceEngine\\src\\Fonts\\FontFiles\\M PLUS 1 Medium (Size_16).spritefont",
		L"C:\\Code Projects\\C++ Projects\\DivergenceEngine\\DivergenceEngine\\src\\Fonts\\FontFiles\\M PLUS 1 Medium (Size_18).spritefont",
		L"C:\\Code Projects\\C++ Projects\\DivergenceEngine\\DivergenceEngine\\src\\Fonts\\FontFiles\\M PLUS 1 Medium (Size_20).spritefont",
		L"C:\\Code Projects\\C++ Projects\\DivergenceEngine\\DivergenceEngine\\src\\Fonts\\FontFiles\\M PLUS 1 Medium (Size_24).spritefont"
	};
}