#include "Graphics.h"
#include "DXComErrorHandler.h"

namespace wrl = Microsoft::WRL;

#pragma comment(lib, "d3d11.lib")

namespace DivergenceEngine
{
	Graphics::Graphics(uint32_t frameRate, HWND windowHandle, uint16_t bufferWidth, uint16_t bufferHeight):
		FrameRate(frameRate), 
		WindowHandle(windowHandle),
		BufferWidth(bufferWidth),
		BufferHeight(bufferHeight)
	{
		//Create device, swap chain, and device context
		DXGI_SWAP_CHAIN_DESC swapChainDescription = {};
		swapChainDescription.BufferDesc.Width = bufferWidth;
		swapChainDescription.BufferDesc.Height = bufferHeight;
		swapChainDescription.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDescription.BufferDesc.RefreshRate.Denominator = 0;
		swapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDescription.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDescription.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
		swapChainDescription.SampleDesc.Count = 1;
		swapChainDescription.SampleDesc.Quality = 0;
		swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDescription.BufferCount = 1;
		swapChainDescription.OutputWindow = windowHandle;
		swapChainDescription.Windowed = TRUE;
		swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDescription.Flags = 0;

		//Check if debug mode
		UINT swapCreateFlags = 0u;
#ifndef NDEBUG
		swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		HRESULT hr = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			swapCreateFlags,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&swapChainDescription,
			&SwapChainPointer,
			&DevicePointer,
			nullptr,
			&DeviceContextPointer
		);
		DX::ThrowIfFailed(hr);

		//Get texture resource from swap chain (back buffer)
		wrl::ComPtr<ID3D11Texture2D> backBufferPointer;
		hr = SwapChainPointer->GetBuffer(0, _uuidof(ID3D11Texture2D), &backBufferPointer);
		DX::ThrowIfFailed(hr);
		
		//Create render target
		DevicePointer->CreateRenderTargetView(
			backBufferPointer.Get(),
			nullptr,
			&RenderTargetPointer
		);
	}

	void Graphics::Present()
	{
		HRESULT hr = SwapChainPointer->Present(1u, 0u);
		DX::ThrowIfFailed(hr);

		//TODO: Handle device removed and device reset someday...
	}

	void Graphics::ClearFrame(float red, float green, float blue) noexcept
	{
		const float colour[] = { red, green, blue, 1.0f };
		DeviceContextPointer->ClearRenderTargetView(RenderTargetPointer.Get(), colour);
	}
}