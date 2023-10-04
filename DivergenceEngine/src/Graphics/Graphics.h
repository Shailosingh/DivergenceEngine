#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <cstdint>
#include <wrl.h>
#include <SimpleMath.h>
#include <string>
#include <SpriteBatch.h> 
#include <CommonStates.h>
#include <SpriteFont.h>
#include "Fonts/DefaultFonts.h"

/*
Video playback links:

https://github.com/yabadabu/dx11_video_texture
https://github.com/microsoft/Windows-classic-samples/tree/main/Samples/DX11VideoRenderer
https://learn.microsoft.com/en-us/windows/win32/medfound/direct3d-11-video-apis
*/


namespace DivergenceEngine
{
	class Graphics
	{
	private:
		//Datafields
		uint16_t BufferWidth = 800;
		uint16_t BufferHeight = 450;
		uint32_t FrameRate = 60;
		HWND WindowHandle = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Device> DevicePointer;
		Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChainPointer;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> DeviceContextPointer;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetPointer;
		std::unique_ptr<DirectX::SpriteBatch> SpriteBatchPointer;
		std::unique_ptr<DirectX::CommonStates> SpriteBatchStatesPointer;
		bool IsSpriteBatchDrawing = false;

		//Helpers
		void BeginSpriteBatch() noexcept;
		void EndSpriteBatch() noexcept;
		
	public:
		//Constructors and destructors
		Graphics(uint32_t frameRate, HWND windowHandle, uint16_t bufferWidth, uint16_t bufferHeight);

		//Deleted stuff
		Graphics(const Graphics&) = delete;
		Graphics& operator=(const Graphics&) = delete;

		//Public functions
		void Present();
		void ClearFrame(float red, float green, float blue) noexcept;

		//Maintenance functions
		void ResetRenderTargetAndViewport(uint16_t clientWidth, uint16_t clientHeight);
		void ResizeWindow(uint16_t clientWidth, uint16_t clientHeight);

		//Sprite batch functions
		void DrawFullSprite(ID3D11ShaderResourceView* texture, DirectX::SimpleMath::Vector2 position);
		void DrawSizedSprite(ID3D11ShaderResourceView* texture, DirectX::SimpleMath::Vector2 position, DirectX::SimpleMath::Vector2 size);

		//Font drawing functions
		void DrawString_TopLeftCoordinate(DirectX::SpriteFont* spriteFont, std::wstring text, DirectX::SimpleMath::Vector2 topLeftPositionCoord, DirectX::FXMVECTOR colour, bool dropShadow = false);
		void DrawString_CentreCoordinate(DirectX::SpriteFont* spriteFont, std::wstring text, DirectX::SimpleMath::Vector2 centrePositionCoord, DirectX::FXMVECTOR colour, bool dropShadow = false);

		void DrawString_CentredInRow(DirectX::SpriteFont* spriteFont, std::wstring text, float rowCoord, DirectX::FXMVECTOR colour, bool dropShadow = false);

		//Texture loaders
		void LoadTexture(std::wstring filePath, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& texture, CD3D11_TEXTURE2D_DESC&  textureDescription);
		void LoadTexture(std::wstring filePath, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& texture);

		//Font loaders
		void LoadFont(std::wstring spriteFontPath, std::unique_ptr<DirectX::SpriteFont>& spriteFont);
		void LoadFont(DefaultFonts::DefaultFontIndices defaultFontIndex, std::unique_ptr<DirectX::SpriteFont>& spriteFont);

		//Getters
		DirectX::XMINT2 GetBufferSize() const noexcept;
	};
}