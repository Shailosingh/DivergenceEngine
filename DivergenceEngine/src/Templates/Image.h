#pragma once
#include "Templates/UnclickableDrawable.h"
#include "Graphics/Graphics.h"
#include <string>

namespace DivergenceEngine::Templates
{
	class Image : public DivergenceEngine::UnclickableDrawable
	{
	private:
		//Datafields
		std::wstring FilePath;
		std::weak_ptr<Graphics> WindowGraphicsController;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ImageTexture;
		DirectX::SimpleMath::Vector2 Position;
		DirectX::SimpleMath::Vector2 Size;
		
	public:
		//Constructors and Destructors
		Image(std::wstring filePath, std::weak_ptr<Graphics> graphicsController, DirectX::SimpleMath::Vector2 position);
		Image(std::wstring filePath, std::weak_ptr<Graphics> graphicsController, DirectX::SimpleMath::Vector2 position, DirectX::SimpleMath::Vector2 size);

		//Overriden functions
		void Draw() override;
		bool IsCoordInObject(DirectX::XMINT2 mousePos) override;
	};
}
