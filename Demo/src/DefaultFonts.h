#pragma once
#include <array>
#include <string>

class DefaultFonts
{
public:
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

private:
	static inline const std::array<std::wstring, static_cast<size_t>(DefaultFontIndices::TOTAL_DEFAULT_FONTS)> FontPaths
	{
		L"Fonts\\M PLUS 1 (Size_16).spritefont",
		L"Fonts\\M PLUS 1 (Size_18).spritefont",
		L"Fonts\\M PLUS 1 (Size_20).spritefont",
		L"Fonts\\M PLUS 1 (Size_24).spritefont",

		L"Fonts\\M PLUS 1 Medium (Size_16).spritefont",
		L"Fonts\\M PLUS 1 Medium (Size_18).spritefont",
		L"Fonts\\M PLUS 1 Medium (Size_20).spritefont",
		L"Fonts\\M PLUS 1 Medium (Size_24).spritefont"
	};

public:
	DefaultFonts() = delete;

	static const std::wstring& GetFontPath(DefaultFontIndices index)
	{
		if (index == DefaultFontIndices::TOTAL_DEFAULT_FONTS)
		{
			throw std::invalid_argument("Invalid font index");
		}
		return FontPaths[static_cast<size_t>(index)];
	}
};