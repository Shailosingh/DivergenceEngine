#include "Window.h"
#include "Logger/Logger.h"
#include <Windows.h>
#include <format>

namespace DivergenceEngine
{
	//WindowClass implementation-------------------------------------------------------------------
	Window::WindowClass::WindowClass()
	{
		//TODO: Register class
		WNDCLASSEX windowClass = { 0 };
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_OWNDC;
		windowClass.lpfnWndProc = HandleMessageSetup;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = GetModuleHandle(nullptr);
		windowClass.hIcon = nullptr;
		windowClass.hCursor = nullptr;
		windowClass.hbrBackground = nullptr;
		windowClass.lpszMenuName = nullptr;
		windowClass.lpszClassName = WindowClassName;
		windowClass.hIconSm = nullptr;
		RegisterClassEx(&windowClass);

		Logger::Log(std::format(L"WindowClass '{}' Constructed", WindowClassName));
	}

	Window::WindowClass::~WindowClass()
	{
		UnregisterClass(GetName(), GetModuleHandle(nullptr));
		Logger::Log(std::format(L"WindowClass '{}' Destructed", WindowClassName));
	}

	Window::WindowClass& Window::WindowClass::GetCurrentInstance()
	{
		static WindowClass instance;
		return instance;
	}

	const wchar_t* Window::WindowClass::GetName()
	{
		return GetCurrentInstance().WindowClassName;
	}
	
	//Window Implementation------------------------------------------------------------------------
	Window::Window(uint16_t clientWidth, uint16_t clientHeight, const wchar_t* windowTitle)
	{
		//Initialize Datafields
		ClientWidth = clientWidth;
		ClientHeight = clientHeight;
		WindowTitle = windowTitle;
		
		//Create window rect from client size
		DWORD windowStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
		RECT windowRect;
		windowRect.left = 0;
		windowRect.right = clientWidth;
		windowRect.top = 0;
		windowRect.bottom = clientHeight;
		AdjustWindowRect(&windowRect, windowStyle, FALSE);

		//Create window
		WindowHandle = CreateWindowEx(
			0,
			WindowClass::GetName(),
			windowTitle,
			windowStyle,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			nullptr,
			nullptr,
			GetModuleHandle(nullptr),
			this
		);

		//Show window
		ShowWindow(WindowHandle, SW_SHOWDEFAULT);
		
		Logger::Log(std::format(L"Window '{}' Constructed", WindowTitle));
	}

	Window::~Window()
	{
		OnWindowDestruction();
		DestroyWindow(WindowHandle);
		Logger::Log(std::format(L"Window '{}' Destructed", WindowTitle));
	}

	LRESULT Window::HandleMessageSetup(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		//Gets the first parameter of the Window when it is created
		if (uMsg == WM_NCCREATE)
		{
			//Get the pointer to the Window object, through the lpCreateParams (this)
			const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
			Window* const pWindow = static_cast<Window*>(pCreate->lpCreateParams);

			//Set the pointer to the Window object as the user data of the window
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));

			//Set the new message proc as the HandleMessageThunk function
			Logger::Log(L"Set non-static class message proc to Window");
			SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMessageThunk));

			//Forward the message to the actual message handler function
			return pWindow->HandleMessage(hWnd, uMsg, wParam, lParam);
		}

		//If the message is not WM_NCCREATE, then the Window hasn't been created yet
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	
	LRESULT Window::HandleMessageThunk(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		//Get the pointer to the Window object, through the user data of the window
		Window* const pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

		//Forward the message to the actual message handler function
		return pWindow->HandleMessage(hWnd, uMsg, wParam, lParam);
	}

	LRESULT Window::HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_CLOSE:
			//TODO: Create function that signals application that window must be closed
			PostQuitMessage(0);
			return 0;
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	void Window::OnWindowDestruction()
	{
		
	}
}