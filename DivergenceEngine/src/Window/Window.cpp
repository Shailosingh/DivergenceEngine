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
	Window::Window(uint16_t clientWidth, uint16_t clientHeight, const wchar_t* windowTitle, SignalWindowDestructionFunction signalFunction)
	{
		//Initialize Datafields
		InternalClientWidth = clientWidth;
		InternalClientHeight = clientHeight;
		WindowTitle = windowTitle;
		SignalFunction = signalFunction;
		
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
		//Don't destroy the moved window
		if (WindowHandle == nullptr)
		{
			return;
		}
		
		DestroyWindow(WindowHandle);
		Logger::Log(std::format(L"Window '{}' Destructed", WindowTitle));
	}

	Window::Window(Window&& otherWindow) noexcept
	{
		//Copy otherWindow's datafields
		WindowHandle = otherWindow.WindowHandle;
		InternalClientWidth = otherWindow.InternalClientWidth;
		InternalClientHeight = otherWindow.InternalClientHeight;
		WindowTitle = otherWindow.WindowTitle;
		SignalFunction = otherWindow.SignalFunction;
		
		//Invalidate the otherWindow's handle
		otherWindow.WindowHandle = nullptr;

		//Set the new address of the window object
		SetWindowLongPtr(WindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		
		Logger::Log(std::format(L"Window '{}' Moved, via move constructor", WindowTitle));
	}

	Window& Window::operator=(Window&& otherWindow) noexcept
	{
		if (this->WindowHandle != otherWindow.WindowHandle)
		{
			//Destroy "this" window if it isn't null
			if (this->WindowHandle != nullptr)
			{
				DestroyWindow(WindowHandle);
				Logger::Log(std::format(L"Window '{}' Destructed", WindowTitle));
			}
			
			//Copy otherWindow's datafields
			WindowHandle = otherWindow.WindowHandle;
			InternalClientWidth = otherWindow.InternalClientWidth;
			InternalClientHeight = otherWindow.InternalClientHeight;
			WindowTitle = otherWindow.WindowTitle;
			SignalFunction = otherWindow.SignalFunction;

			//Invalidate the otherWindow's handle
			otherWindow.WindowHandle = nullptr;

			//Set the new address of the window object
			SetWindowLongPtr(WindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
			
			Logger::Log(std::format(L"Window '{}' Moved, via move assignment operator", WindowTitle));
		}
		
		return *this;
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
			SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMessageThunk));
			Logger::Log(std::format(L"Set non-static class message proc to Window '{}'", pWindow->WindowTitle));

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
		//Initialize variables
		bool keyPreviouslyPressed;
		
		switch (uMsg)
		{
		case WM_CLOSE:
			if (OnWindowDestructionRequest())
			{
				SignalFunction(hWnd);
			}
			return 0;
			break;

		case WM_KILLFOCUS:
			KeyboardObject.ClearKeyStates();
			break;

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			keyPreviouslyPressed = lParam & ((LPARAM)1 << 30);
			if (!keyPreviouslyPressed || KeyboardObject.AutoRepeatIsEnabled())
			{
				KeyboardObject.OnKeyPressed(static_cast<uint8_t>(wParam));
			}
			break;

		case WM_KEYUP:
		case WM_SYSKEYUP:
			KeyboardObject.OnKeyReleased(static_cast<uint8_t>(wParam));
			break;

		case WM_CHAR:
			KeyboardObject.OnChar(static_cast<uint8_t>(wParam));
			break;
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	void Window::RenderWindow()
	{
		//TODO
	}
	
	void Window::UpdateAndDraw()
	{
		UpdateWindow();
		RenderWindow();
	}

	//Overridable functions------------------------------------------------------------------------
	bool Window::OnWindowDestructionRequest()
	{
		return true;
	}
	
	//Helpers--------------------------------------------------------------------------------------
	bool Window::IsEqualHandle(HWND windowHandle) const noexcept
	{
		return WindowHandle == windowHandle;
	}
	
	std::wstring Window::GetWindowTitle() const noexcept
	{
		return WindowTitle;
	}

	void Window::SetWindowTitle(const std::wstring& windowTitle) noexcept
	{
		WindowTitle = windowTitle;
		SetWindowText(WindowHandle, WindowTitle.c_str());
	}
}