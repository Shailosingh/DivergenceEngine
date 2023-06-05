#pragma once
#include <cstdint>
#include <Windows.h>
#include <string>

namespace DivergenceEngine
{
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

		//WinProc Functions
		static LRESULT CALLBACK HandleMessageSetup(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK HandleMessageThunk(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		//Functions that can be overridden by the programmer to make the window have different functionality
		virtual void OnWindowDestruction();

	public:
		Window(uint16_t clientWidth, uint16_t clientHeight, const wchar_t* windowTitle);
		~Window();
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
	};
}