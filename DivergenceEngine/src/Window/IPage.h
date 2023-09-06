#pragma once
#include "Application/StepTimer.h"
#include "Window/Window.h"
#include <SimpleMath.h>

namespace DivergenceEngine
{
	class IPage
	{
	public:
		
		virtual ~IPage() {};

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
		/// Here, update all the page's states and prepare it for rendering the next frame
		/// </summary>
		/// <param name="timer">Allows function to see how much time elapsed between last tick and in total</param>
		virtual void UpdatePage(const DX::StepTimer& timer) = 0;

		/// <summary>
		/// Handles mouse scrolls events
		/// </summary>
		/// <param name="scrollDelta">Positive deltas are scrolls up, negatives are scrolls down. A full tick is WHEEL_DELTA</param>
		virtual void HandleScroll(int scrollDelta) = 0;

		/// <summary>
		/// Handles mouse movement events.
		/// </summary>
		/// <param name="newMousePos">The new coordinates of the mouse</param>
		virtual void HandleMouseMove(DirectX::XMINT2 newMousePos) = 0;
	};
}