#pragma once
#include "Application/StepTimer.h"
#include "Window/Window.h"

namespace DivergenceEngine
{
	class IPage
	{
	public:

		/// <summary>
		/// Constructor for the page.
		/// </summary>
		/// <param name="windowReference">Pointer to the Window that owns the page</param>
		virtual void Initialize(Window* windowReference) = 0;

		/// <summary>
		/// Called when the Window is requested to be closed but, does not have to close. 
		/// </summary>
		/// <returns>Return true if you wish for the window to close, return false if the window should stay open</returns>
		virtual bool OnWindowDestructionRequest() = 0;

		/// <summary>
		/// Called in the message loop whenever the window has handled all its messages and needs to be updated.
		/// Here, update all the window's states and prepare it for rendering the next frame
		/// </summary>
		virtual void UpdateWindow(const DX::StepTimer& timer) = 0;
	};
}