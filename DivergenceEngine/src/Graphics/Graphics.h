#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <cstdint>
#include <wrl.h>

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

		
	public:
		//Constructors and destructors
		Graphics(uint32_t frameRate, HWND windowHandle, uint16_t bufferWidth, uint16_t bufferHeight);

		//Deleted stuff
		Graphics(const Graphics&) = delete;
		Graphics& operator=(const Graphics&) = delete;

		//Public functions
		void Present();
		void ClearFrame(float red, float green, float blue) noexcept;
	};
}