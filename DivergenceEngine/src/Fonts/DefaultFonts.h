#pragma once
#include <array>
#include <string>

namespace DivergenceEngine::DefaultFonts
{
	enum class DefaultFontIndices
	{
		M_PLUS_1_Size24,

		TOTAL_DEFAULT_FONTS
	};

	const static std::array<std::wstring, static_cast<size_t>(DefaultFontIndices::TOTAL_DEFAULT_FONTS)> FontPaths
	{
		L"C:\\Code Projects\\C++\\DivergenceEngine\\DivergenceEngine\\src\\Fonts\\FontFiles\\M PLUS 1 (Size_24).spritefont"
	};
}