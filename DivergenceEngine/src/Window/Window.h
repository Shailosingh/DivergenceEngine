#pragma once
#include <cstdint>
#include <Windows.h>
#include <string>

namespace DivergenceEngine
{
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
		
		//Window Datafields
		HWND WindowHandle;
		uint16_t ClientWidth;
		uint16_t ClientHeight;
		std::wstring WindowTitle;
		SignalWindowDestructionFunction SignalFunction;

		//WinProc Functions
		static LRESULT CALLBACK HandleMessageSetup(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK HandleMessageThunk(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	protected:
		//Functions that can be overridden by the programmer to make the window have different functionality
		/// <summary>
		/// Called when the Window is requested to be closed but, does not have to close.
		/// Return true if you wish for the system to close, return false if the window should not close.
		/// </summary>
		virtual bool OnWindowDestructionRequest();

	public:
		Window(uint16_t clientWidth, uint16_t clientHeight, const wchar_t* windowTitle, SignalWindowDestructionFunction SignalFunction);
		~Window();
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		//Move constructor
		Window(Window&& otherWindow) noexcept;
		Window& operator=(Window&& otherWindow) noexcept;

		//Helpers
		bool IsEqualHandle(HWND windowHandle) const noexcept;
	};
}