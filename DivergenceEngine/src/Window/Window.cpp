#include "Window.h"
#include "Logger/Logger.h"
#include <Windows.h>
#include <format>
#include "Application/Application.h"

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
	Window::Window(uint16_t clientWidth, uint16_t clientHeight, const wchar_t* windowTitle, std::unique_ptr<IPage>&& page) :
		ClientWidth(clientWidth),
		ClientHeight(clientHeight),
		WindowTitle(windowTitle),
		RetryAudio(false),
		PageReference(std::move(page))
	{	
		//Create window rect from client size
		RECT windowRect;
		windowRect.left = 0;
		windowRect.right = clientWidth;
		windowRect.top = 0;
		windowRect.bottom = clientHeight;
		AdjustWindowRect(&windowRect, WINDOWED_WINDOW_STYLE, FALSE);

		//Create window
		WindowHandle = CreateWindowEx(
			0,
			WindowClass::GetName(),
			windowTitle,
			WINDOWED_WINDOW_STYLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			nullptr,
			nullptr,
			GetModuleHandle(nullptr),
			this
		);
		if (WindowHandle == NULL)
		{
			throw std::exception("Failed to create window");
		}

		//Display the window (it will be hidden until you do this)
		ShowWindow(WindowHandle, SW_SHOWDEFAULT);
		
		//Initialize the graphics controller
		GraphicsController = std::make_shared<Graphics>(Application::GetFrameRate(), WindowHandle, clientWidth, clientHeight);

		//Initialize the audio
		DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_Default;
#ifdef _DEBUG
		eflags |= DirectX::AudioEngine_Debug;
#endif
		AudioController = std::make_unique<DirectX::AudioEngine>(eflags);

		//Start up audio thread
		IsAudioControllerUpdating = true;
		AudioUpdateThread = std::thread(&Window::AudioUpdateThreadFunction, this);

		//Initialize size for three layers of drawables
		LayersOfDrawableComponents.reserve(3);
		
		//Initialize the page, with the window pointer
		PageReference->Initialize(this);

		Logger::Log(std::format(L"Window '{}' Constructed", WindowTitle));
	}

	Window::~Window()
	{
		//Don't destroy the moved window
		if (WindowHandle == nullptr)
		{
			return;
		}
		
		//Suspend audio on window close
		if (AudioController)
		{
			AudioController->Suspend();
		}

		//Close up the audio update thread
		IsAudioControllerUpdating = false;
		AudioUpdateThread.join();
		
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
		//General----------------------------------------------------------------------------------
		case WM_CLOSE:
			if (OnWindowDestructionRequest())
			{
				Application::RemoveWindow(hWnd);
			}
			break;

		case WM_SIZE:
			//Set the new client size
			ClientWidth = LOWORD(lParam);
			ClientHeight = HIWORD(lParam);
			
			if (GraphicsController != nullptr)
			{
				GraphicsController->ResetRenderTargetAndViewport(ClientWidth, ClientHeight);
			}
			break;

		case WM_KILLFOCUS:
			KeyboardObject.ClearKeyStates();
			break;

		//Keyboard---------------------------------------------------------------------------------
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
			KeyboardObject.OnChar(static_cast<wchar_t>(wParam));
			break;

		//Mouse------------------------------------------------------------------------------------
		case WM_MOUSEMOVE: //Braces are needed because of the variable declaration
		{
			POINTS mousePosition = MAKEPOINTS(lParam);

			//Scale the mousePosition to the internal graphics buffer, to make collision of mouse to objects easier
			DirectX::XMINT2 bufferSize = GraphicsController->GetBufferSize();
			double xScaleRatio = static_cast<double>(bufferSize.x) / static_cast<double>(ClientWidth);
			double yScaleRatio = static_cast<double>(bufferSize.y) / static_cast<double>(ClientHeight);
			mousePosition.x = static_cast<short>(mousePosition.x * xScaleRatio);
			mousePosition.y = static_cast<short>(mousePosition.y * yScaleRatio);

			MouseObject.OnMouseMove(mousePosition.x, mousePosition.y);
			break;
		}

		case WM_MOUSEWHEEL:
			MouseObject.OnWheelScroll(GET_WHEEL_DELTA_WPARAM(wParam));
			break;

		case WM_LBUTTONDOWN:
			MouseObject.OnLeftPressed();
			break;

		case WM_LBUTTONUP:
			MouseObject.OnLeftReleased();
			break;

		case WM_RBUTTONDOWN:
			MouseObject.OnRightPressed();
			break;
			
		case WM_RBUTTONUP:
			MouseObject.OnRightReleased();
			break;

		case WM_MBUTTONDOWN:
			MouseObject.OnMiddlePressed();
			break;
			
		case WM_MBUTTONUP:
			MouseObject.OnMiddleReleased();
			break;

		//Default----------------------------------------------------------------------------------
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
			break;
		}

		return 0;
	}

	void Window::RenderWindow()
	{
		//Clear the screen
		GraphicsController->ClearFrame(0.5f, 0.0f, 0.9f);

		//Cycle through the layers and render them
		for (auto& layer : LayersOfDrawableComponents)
		{
			for (auto& component : layer)
			{
				component->Draw();
			}
		}
		
		//Present frame
		GraphicsController->Present();
	}
	
	void Window::UpdateAndDraw(const DX::StepTimer& timer)
	{
		UpdateWindow(timer);
		RenderWindow();

		//Swap out page if user requests
		if (QueuedPage != nullptr)
		{
			//Clear out all remaining drawables old page on screen
			ClearAllLayers();

			//Reset the audio
			AudioController->Reset();
			
			PageReference = std::move(QueuedPage);
			QueuedPage = nullptr;

			PageReference->Initialize(this);
		}
	}

	void Window::AddDrawableComponent(std::shared_ptr<IDrawable> drawableComponent, size_t layer)
	{
		//If the number of layers is less than the layer specified, resize the layer vector (NOTE: layer is a 0-based index)
		if (LayersOfDrawableComponents.size() <= layer)
		{
			LayersOfDrawableComponents.resize(layer + 1);
		}

		//Add the drawable component to the specified layer
		LayersOfDrawableComponents[layer].push_back(drawableComponent);
	}

	void Window::RemoveDrawableComponent(std::shared_ptr<IDrawable> drawableComponent, size_t layer)
	{
		//If the number of layers is less than the layer specified, the layer does not exist and the function can return, doing nothing
		if (LayersOfDrawableComponents.size() <= layer)
		{
			return;
		}

		//Search within the specified layer for the drawable component and remove it from the layers
		for (auto it = LayersOfDrawableComponents[layer].begin(); it != LayersOfDrawableComponents[layer].end(); ++it)
		{
			if (*it == drawableComponent)
			{
				LayersOfDrawableComponents[layer].erase(it);
				return;
			}
		}
	}

	void Window::ClearLayer(size_t layer)
	{
		//If the layer index is out of range, return
		if (LayersOfDrawableComponents.size() <= layer)
		{
			return;
		}

		//Clear the layer
		LayersOfDrawableComponents[layer].clear();
	}

	void Window::ClearAllLayers()
	{
		//Clear all layers
		LayersOfDrawableComponents.clear();
	}

	void Window::DispatchMouseEvents()
	{
		HandleMouseOverEvents();
		
		while (!MouseObject.IsQueueEmpty())
		{
			//Get the mouse event (will not be empty because it only is if the queue is empty, which is checked in the while loop)
			DivergenceEngine::Mouse::Event currentEvent = *MouseObject.GetNextMouseEvent();

			switch (currentEvent.GetType())
			{
			case DivergenceEngine::Mouse::Event::Type::WheelScroll:
				PageReference->HandleScroll(currentEvent.GetScrollTicks());
				break;

			case DivergenceEngine::Mouse::Event::Type::Move:
				PageReference->HandleMouseMove(DirectX::XMINT2(currentEvent.GetPosX(), currentEvent.GetPosY()));
				break;

			default:
				HandleMousePressReleaseEvent(currentEvent);
				break;
					
			}
		}
	}

	void Window::HandleMousePressReleaseEvent(DivergenceEngine::Mouse::Event newEvent)
	{
		//Iterate through the layers and check if the mouse is over any of the drawables. If it is, try to trigger the appropriate event. When an event is successful, break out
		for (auto& layer : LayersOfDrawableComponents)
		{
			for (auto& component : layer)
			{
				if (component->IsCoordInObject(newEvent.GetPos()))
				{
					switch (newEvent.GetType())
					{
					case DivergenceEngine::Mouse::Event::Type::LPress:
						if (component->OnLeftPress(newEvent.GetPos()))
						{
							return;
						}
						break;

					case DivergenceEngine::Mouse::Event::Type::LRelease:
						if (component->OnLeftRelease(newEvent.GetPos()))
						{
							return;
						}
						break;

					case DivergenceEngine::Mouse::Event::Type::MPress:
						if (component->OnMiddlePress(newEvent.GetPos()))
						{
							return;
						}
						break;

					case DivergenceEngine::Mouse::Event::Type::MRelease:
						if (component->OnMiddleRelease(newEvent.GetPos()))
						{
							return;
						}
						break;
						
					case DivergenceEngine::Mouse::Event::Type::RPress:
						if (component->OnRightPress(newEvent.GetPos()))
						{
							return;
						}
						break;

					case DivergenceEngine::Mouse::Event::Type::RRelease:
						if (component->OnRightRelease(newEvent.GetPos()))
						{
							return;
						}
						break;
					}
				}
			}
		}
	}

	void Window::HandleMouseOverEvents()
	{
		//Get the current mouse position
		DirectX::XMINT2 mousePosition = MouseObject.GetPos();

		//Iterate through every object starting from layer 0, to see if the mouse is over it. When the mouse is over an object, call OnMouseOver. After that, call OnMouseNotOver for rest without checking
		bool mouseFoundObject = false;
		for (auto& layer : LayersOfDrawableComponents)
		{
			for (auto& component : layer)
			{
				if (!mouseFoundObject || component->IsCoordInObject(mousePosition))
				{
					component->OnMouseOver(mousePosition);
					mouseFoundObject = true;
				}

				else
				{
					component->OnMouseNotOver();
				}
			}
		}
	}

	//Page implemented functions-------------------------------------------------------------------
	bool Window::OnWindowDestructionRequest()
	{
		return PageReference->OnWindowDestructionRequest();
	}

	void Window::UpdateWindow(const DX::StepTimer& timer)
	{
		DispatchMouseEvents();
		PageReference->UpdatePage(timer);
	}

	//Audio----------------------------------------------------------------------------------------
	void Window::AudioUpdateThreadFunction()
	{
		while (IsAudioControllerUpdating)
		{
			UpdateAudio();
			Sleep(15);
		}
	}

	void Window::UpdateAudio()
	{
		if (RetryAudio)
		{
			RetryAudio = false;
			AudioController->Reset();
		}
		else if (!AudioController->Update())
		{
			if (AudioController->IsCriticalError())
			{
				RetryAudio = true;
			}
		}
	}
	
	//Helpers--------------------------------------------------------------------------------------
	bool Window::IsEqualHandle(HWND windowHandle) const noexcept
	{
		return WindowHandle == windowHandle;
	}
	
	HWND Window::GetHandle() const noexcept
	{
		return WindowHandle;
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

	void Window::ToggleFullscreen() noexcept
	{	
		//Get the buffer size
		DirectX::XMINT2 bufferSize = GraphicsController->GetBufferSize();

		//Get the current window style
		LONG_PTR windowStyle = GetWindowLongPtr(WindowHandle, GWL_STYLE);

		//If the window is currently fullscreen, set the window back to windowed
		if (IsFullscreen)
		{
			SetWindowLongPtr(WindowHandle, GWL_STYLE, WINDOWED_WINDOW_STYLE);

			//Update styles and size
			SetWindowPos(WindowHandle, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
			GraphicsController->ResizeWindow(bufferSize.x, bufferSize.y);
			ShowWindow(WindowHandle, SW_NORMAL);
		}

		//If the window is currently windowed, set it to fullscreen
		else
		{
			SetWindowLongPtr(WindowHandle, GWL_STYLE, FULLSCREEN_WINDOW_STYLE);
			
			//Update styles and size
			SetWindowPos(WindowHandle, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
			ShowWindow(WindowHandle, SW_SHOWMAXIMIZED);
		}
		
		//Update fullscreen flag
		IsFullscreen = !IsFullscreen;
	}

	void Window::QueueNewPage(std::unique_ptr<IPage>&& newPage) noexcept
	{
		QueuedPage = std::move(newPage);
	}
}