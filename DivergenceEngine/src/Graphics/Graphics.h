#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <cstdint>
#include <wrl.h>
#include <SimpleMath.h>
#include <string>
#include <SpriteBatch.h> //TODO: Implement functions to use this
#include <CommonStates.h>

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

		//Texture loaders
		void LoadTexture(std::wstring filePath, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& texture, CD3D11_TEXTURE2D_DESC&  textureDescription);
		void LoadTexture(std::wstring filePath, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& texture);
	};
}