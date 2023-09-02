#pragma once
#include <SimpleMath.h>

namespace DivergenceEngine
{
	class IDrawable
	{	
	public:
		
		/// <summary>
		/// Code that draws the object to the screen.
		/// </summary>
		virtual void Draw() = 0;

		/// <summary>
		/// Checks if the given coordinates are within the bounds of the object.
		/// </summary>
		/// <param name="mousePos">The coordinate of the mouse on the entire screen buffer</param>
		/// <returns>True if the coordinates are within the object/returns>
		virtual bool IsCoordInObject(DirectX::XMINT2 mousePos) = 0;

		/// <summary>
		/// The code that runs when the object is pressed down with the left button
		/// </summary>
		/// <param name="mousePos">The coordinate of the mouse on the entire screen buffer</param>
		/// <returns>Returns true if the object handled the press, false if not</returns>
		virtual bool OnLeftPress(DirectX::XMINT2 mousePos) = 0;

		/// <summary>
		/// The code that runs when the object is released with the left button
		/// </summary>
		/// <param name="mousePos"></param>
		/// <returns></returns>
		virtual bool OnLeftRelease(DirectX::XMINT2 mousePos) = 0;

		/// <summary>
		/// The code that runs when the object is pressed down with the middle button
		/// </summary>
		/// <param name="mousePos">The coordinate of the mouse on the entire screen buffer</param>
		/// <returns></returns>
		virtual bool OnMiddlePress(DirectX::XMINT2 mousePos) = 0;

		/// <summary>
		/// The code that runs when the object is released with the middle button
		/// </summary>
		/// <param name="mousePos">The coordinate of the mouse on the entire screen buffer</param>
		/// <returns></returns>
		virtual bool OnMiddleRelease(DirectX::XMINT2 mousePos) = 0;

		/// <summary>
		/// The code that runs when the object is pressed down with the right button
		/// </summary>
		/// <param name="mousePos">The coordinate of the mouse on the entire screen buffer</param>
		/// <returns></returns>
		virtual bool OnRightPress(DirectX::XMINT2 mousePos) = 0;
		
		/// <summary>
		/// The code that runs when the object is released with the right button
		/// </summary>
		/// <param name="mousePos">The coordinate of the mouse on the entire screen buffer</param>
		/// <returns></returns>
		virtual bool OnRightRelease(DirectX::XMINT2 mousePos) = 0;
	};
}