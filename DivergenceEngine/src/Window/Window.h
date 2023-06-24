#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>
#include <memory>
#include "Keyboard.h"
#include "Application/StepTimer.h"
#include "Graphics/Graphics.h"

namespace DivergenceEngine
{
	//Function pointer definitions
	typedef void(*SignalWindowDestructionFunction)(HWND);

	class Window
	{
	private:

		//Singleton class. Only one instance of this class can exist.
		class WindowClass
		{
		private:
			WindowClass();
			~WindowClass();

			WindowClass(const WindowClass&) = delete;
			WindowClass& operator=(const WindowClass&) = delete;

			static WindowClass& GetCurrentInstance();
			static constexpr const wchar_t* WindowClassName = L"DivergenceEngineWindowClass";

		public:
			static const wchar_t* GetName();
		};
		
		//Private datafields
		SignalWindowDestructionFunction SignalFunction;

		//WinProc Functions
		static LRESULT CALLBACK HandleMessageSetup(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK HandleMessageThunk(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		void RenderWindow();

	protected: 
		//Protected Window Datafields
		HWND WindowHandle;
		uint16_t InternalClientWidth;
		uint16_t InternalClientHeight;
		std::wstring WindowTitle;
		std::unique_ptr<Graphics> GraphicsController;

		//Overridable functions

		/// <summary>
		/// Called when the Window is requested to be closed but, does not have to close. 
		/// </summary>
		/// <returns>Return true if you wish for the window to close, return false if the window should stay open</returns>
		virtual bool OnWindowDestructionRequest();

		/// <summary>
		/// Called in the message loop whenever the window has handled all its messages and needs to be updated.
		/// Here, update all the window's states and prepare it for rendering the next frame
		/// </summary>
		virtual void UpdateWindow(const DX::StepTimer& timer) = 0;

	public:
		Window(uint16_t clientWidth, uint16_t clientHeight, const wchar_t* windowTitle, SignalWindowDestructionFunction signalFunction);
		~Window();
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		//Datafields
		Keyboard KeyboardObject;

		//Move constructor
		Window(Window&& otherWindow) noexcept;
		Window& operator=(Window&& otherWindow) noexcept;

		//Public functions
		void UpdateAndDraw(const DX::StepTimer& timer);

		//Helpers
		bool IsEqualHandle(HWND windowHandle) const noexcept;
		std::wstring GetWindowTitle()const noexcept;
		void SetWindowTitle(const std::wstring& windowTitle) noexcept;
	};
}