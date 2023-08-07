#pragma once

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
		/// The code that runs when the object is left-clicked
		/// </summary>
		/// <returns>Returns true if the object handled the click, false if not</returns>
		virtual bool OnLeftClick() = 0;
	};
}