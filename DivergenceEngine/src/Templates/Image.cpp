#include "Image.h"
#include "DXComErrorHandler.h"
#include <wrl.h>

namespace wrl = Microsoft::WRL;

/*
* REMINDER: After you finish up testing out the Image class, revamp the Window class by putting all the virtual
* functions into a new interface called IPage, that shall be a friend class of Window and have all the missing methods
*/

namespace DivergenceEngine::Templates
{
	Image::Image(std::wstring filePath, std::weak_ptr<Graphics> graphicsController, DirectX::SimpleMath::Vector2 position):
		FilePath(filePath),
		WindowGraphicsController(graphicsController),
		Position(position)
	{
		//Load the texture
		CD3D11_TEXTURE2D_DESC textureDescription;
		WindowGraphicsController.lock()->LoadTexture(FilePath, ImageTexture, textureDescription);

		//Record the size of the texture
		Size.x = static_cast<float>(textureDescription.Width);
		Size.y = static_cast<float>(textureDescription.Height);
	}

	Image::Image(std::wstring filePath, std::weak_ptr<Graphics> graphicsController, DirectX::SimpleMath::Vector2 position, DirectX::SimpleMath::Vector2 size):
		FilePath(filePath),
		WindowGraphicsController(graphicsController),
		Position(position)
	{
		//Load the texture
		WindowGraphicsController.lock()->LoadTexture(FilePath, ImageTexture);

		//Record the size of the texture
		Size = size;
	}

	void Image::Draw()
	{
		WindowGraphicsController.lock()->DrawSizedSprite(ImageTexture.Get(), Position, Size);
	}
	
	bool Image::IsCoordInObject(DirectX::XMINT2 mousePos)
	{
		bool withinXBounds = (mousePos.x >= Position.x) && (mousePos.x <= (Position.x + Size.x));
		bool withinYBounds = (mousePos.y >= Position.y) && (mousePos.y <= (Position.y + Size.y));

		return withinXBounds && withinYBounds;
	}
}