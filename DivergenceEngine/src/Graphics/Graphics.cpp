#include "Graphics.h"
#include <WICTextureLoader.h>
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
		swapChainDescription.BufferCount = 2;
		swapChainDescription.OutputWindow = windowHandle;
		swapChainDescription.Windowed = TRUE;
		swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
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
		hr = DevicePointer->CreateRenderTargetView(
			backBufferPointer.Get(),
			nullptr,
			&RenderTargetPointer
		);
		DX::ThrowIfFailed(hr);

		//Configure viewport
		D3D11_VIEWPORT viewPort;
		viewPort.Width = static_cast<float>(bufferWidth);
		viewPort.Height = static_cast<float>(bufferHeight);
		viewPort.MinDepth = 0.0f;
		viewPort.MaxDepth = 1.0f;
		viewPort.TopLeftX = 0.0f;
		viewPort.TopLeftY = 0.0f;
		DeviceContextPointer->RSSetViewports(1u, &viewPort);

		//Create the SpriteBatch
		SpriteBatchPointer = std::make_unique<DirectX::SpriteBatch>(DeviceContextPointer.Get());

		//Create the common states
		SpriteBatchStatesPointer = std::make_unique<DirectX::CommonStates>(DevicePointer.Get());
	}

	void Graphics::Present()
	{
		//Bind render target views
		DeviceContextPointer->OMSetRenderTargets(1u, RenderTargetPointer.GetAddressOf(), nullptr);
		
		//End any active sprite batches
		EndSpriteBatch();

		HRESULT hr = SwapChainPointer->Present(1u, 0u);
		DX::ThrowIfFailed(hr);

		//TODO: Handle device removed and device reset someday...
	}

	void Graphics::ClearFrame(float red, float green, float blue) noexcept
	{
		const float colour[] = { red, green, blue, 1.0f };
		DeviceContextPointer->ClearRenderTargetView(RenderTargetPointer.Get(), colour);
	}

	void Graphics::ResetRenderTargetAndViewport(uint16_t clientWidth, uint16_t clientHeight)
	{
		//Unbind the current render target and release references
		DeviceContextPointer->OMSetRenderTargets(0, 0, 0);
		RenderTargetPointer.Reset();

		//Flush all remaining commands on device context
		//DeviceContextPointer->Flush();
		
		//Get texture resource from swap chain (back buffer)
		wrl::ComPtr<ID3D11Texture2D> backBufferPointer;
		HRESULT hr = SwapChainPointer->GetBuffer(0, _uuidof(ID3D11Texture2D), &backBufferPointer);
		DX::ThrowIfFailed(hr);

		//Create render target
		hr = DevicePointer->CreateRenderTargetView(
			backBufferPointer.Get(),
			nullptr,
			&RenderTargetPointer
		);
		DX::ThrowIfFailed(hr);

		//Configure viewport
		D3D11_VIEWPORT viewPort;
		viewPort.Width = static_cast<float>(clientWidth);
		viewPort.Height = static_cast<float>(clientHeight);
		viewPort.MinDepth = 0.0f;
		viewPort.MaxDepth = 1.0f;
		viewPort.TopLeftX = 0.0f;
		viewPort.TopLeftY = 0.0f;
		DeviceContextPointer->RSSetViewports(1u, &viewPort);
	}

	void Graphics::ResizeWindow(uint16_t clientWidth, uint16_t clientHeight)
	{
		DXGI_MODE_DESC displayDescription = {};
		displayDescription.Width = clientWidth;
		displayDescription.Height = clientHeight;
		displayDescription.RefreshRate.Numerator = 0;
		displayDescription.RefreshRate.Denominator = 0;
		displayDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		displayDescription.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		displayDescription.Scaling = DXGI_MODE_SCALING_STRETCHED;
		
		//Resize the swap chain
		HRESULT hr = SwapChainPointer->ResizeTarget(&displayDescription);
		DX::ThrowIfFailed(hr);
	}
	
	//Sprite batch functions-----------------------------------------------------------------------
	void Graphics::DrawFullSprite(ID3D11ShaderResourceView* texture, DirectX::SimpleMath::Vector2 position)
	{
		BeginSpriteBatch();

		SpriteBatchPointer->Draw(texture, position);
	}

	void Graphics::DrawSizedSprite(ID3D11ShaderResourceView* texture, DirectX::SimpleMath::Vector2 position, DirectX::SimpleMath::Vector2 size)
	{
		BeginSpriteBatch();
		
		RECT positionRectangle;
		positionRectangle.left = static_cast<LONG>(position.x);
		positionRectangle.right = static_cast<LONG>(position.x + size.x);
		positionRectangle.top = static_cast<LONG>(position.y);
		positionRectangle.bottom = static_cast<LONG>(position.y + size.y);
		SpriteBatchPointer->Draw(texture, positionRectangle, nullptr, DirectX::Colors::White);
	}

	//Font drawing functions-----------------------------------------------------------------------
	void Graphics::DrawString(DirectX::SpriteFont* spriteFont, std::wstring text, DirectX::SimpleMath::Vector2 positionCoord, DirectX::SimpleMath::Vector2 origin, DirectX::SimpleMath::Color colour, bool dropShadow)
	{
		BeginSpriteBatch();

		if (dropShadow)
		{
			//Take the negation of the font colour and use that as its drop shadow colour
			DirectX::SimpleMath::Color shadowColour = DirectX::SimpleMath::Color(colour);
			shadowColour.Negate();

			spriteFont->DrawString(SpriteBatchPointer.get(), text.c_str(), positionCoord + DirectX::SimpleMath::Vector2(1, 1), shadowColour, 0, origin);
			spriteFont->DrawString(SpriteBatchPointer.get(), text.c_str(), positionCoord + DirectX::SimpleMath::Vector2(-1, 1), shadowColour, 0, origin);
			spriteFont->DrawString(SpriteBatchPointer.get(), text.c_str(), positionCoord + DirectX::SimpleMath::Vector2(-1, -1), shadowColour, 0, origin);
			spriteFont->DrawString(SpriteBatchPointer.get(), text.c_str(), positionCoord + DirectX::SimpleMath::Vector2(1, -1), shadowColour, 0, origin);
		}

		spriteFont->DrawString(SpriteBatchPointer.get(), text.c_str(), positionCoord, colour, 0, origin);
	}

	//Texture Loader-------------------------------------------------------------------------------
	
	//Designed for loading textures from files, when you need to know the size of the texture
	void Graphics::LoadTexture(const std::wstring& filePath, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& texture, CD3D11_TEXTURE2D_DESC& textureDescription)
	{
		//Load the texture and its resource info from the file
		wrl::ComPtr<ID3D11Resource> resource;
		DX::ThrowIfFailed(DirectX::CreateWICTextureFromFile(DevicePointer.Get(), DeviceContextPointer.Get(), filePath.c_str(), &resource, &texture));

		//Get the texture description from the resource info
		wrl::ComPtr<ID3D11Texture2D> texture2D;
		DX::ThrowIfFailed(resource.As(&texture2D));
		texture2D->GetDesc(&textureDescription);
	}

	//For loading textures from files, when you won't need the default size of the texture and it will be defined by the programmer
	void Graphics::LoadTexture(const std::wstring& filePath, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& texture)
	{
		DX::ThrowIfFailed(DirectX::CreateWICTextureFromFile(DevicePointer.Get(), filePath.c_str(), nullptr, &texture));
	}

	//Font Loader----------------------------------------------------------------------------------
	void Graphics::LoadFont(const std::wstring& spriteFontPath, std::weak_ptr<DirectX::SpriteFont>& spriteFont)
	{
		if (!FontMap.contains(spriteFontPath))
		{
			FontMap[spriteFontPath] = std::make_shared<DirectX::SpriteFont>(DevicePointer.Get(), spriteFontPath.c_str());
			DivergenceEngine::Logger::Log(std::format(L"Font loaded from file: {}", spriteFontPath));
		}

		spriteFont = FontMap[spriteFontPath];
	}

	//Helpers--------------------------------------------------------------------------------------
	void Graphics::BeginSpriteBatch() noexcept
	{
		if (!IsSpriteBatchDrawing)
		{
			SpriteBatchPointer->Begin(DirectX::SpriteSortMode_Deferred, SpriteBatchStatesPointer->NonPremultiplied());
			IsSpriteBatchDrawing = true;
		}
	}

	void Graphics::EndSpriteBatch() noexcept
	{
		if (IsSpriteBatchDrawing)
		{
			SpriteBatchPointer->End();
			IsSpriteBatchDrawing = false;
		}
	}

	//Getters--------------------------------------------------------------------------------------
	DirectX::XMINT2 Graphics::GetBufferSize() const noexcept
	{
		return DirectX::XMINT2(BufferWidth, BufferHeight);
	}
}