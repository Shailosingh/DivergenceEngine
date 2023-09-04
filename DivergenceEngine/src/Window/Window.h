#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>
#include <memory>
#include <list>
#include "Keyboard.h"
#include "Mouse.h"
#include "Application/StepTimer.h"
#include "Graphics/Graphics.h"
#include "IDrawable.h"
#include "IPage.h"

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

		//Private datafields
		HWND WindowHandle;
		uint16_t ClientWidth;
		uint16_t ClientHeight;
		std::wstring WindowTitle;
		bool IsFullscreen = false;

		//Constants
		const DWORD WINDOWED_WINDOW_STYLE = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
		const DWORD FULLSCREEN_WINDOW_STYLE = WS_POPUP;

		//WinProc Functions
		static LRESULT CALLBACK HandleMessageSetup(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK HandleMessageThunk(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		//Rendering
		std::vector<std::list<std::shared_ptr<IDrawable>>> LayersOfDrawableComponents;
		void RenderWindow();

		//Layer mouse event handler functions
		void DispatchMouseEvents();
		void HandleMousePressReleaseEvent(DivergenceEngine::Mouse::Event newEvent);
		void HandleMouseOverEvents();

		//Page
		std::unique_ptr<IPage> PageReference;
		bool OnWindowDestructionRequest();
		void UpdateWindow(const DX::StepTimer& timer);
		

	public:
		Window(uint16_t clientWidth, uint16_t clientHeight, const wchar_t* windowTitle, std::unique_ptr<IPage>&& page);
		~Window();
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		//Datafields
		Keyboard KeyboardObject;
		Mouse MouseObject;
		std::shared_ptr<Graphics> GraphicsController;

		//Move constructor
		Window(Window&& otherWindow) = delete;
		Window& operator=(Window&& otherWindow) = delete;

		//Public functions
		void UpdateAndDraw(const DX::StepTimer& timer);

		//Rendering functions
		void AddDrawableComponent(std::shared_ptr<IDrawable> drawableComponent, size_t layer);
		void RemoveDrawableComponent(std::shared_ptr<IDrawable> drawableComponent, size_t layer);
		void ClearLayer(size_t layer);
		void ClearAllLayers();

		//Helpers
		bool IsEqualHandle(HWND windowHandle) const noexcept;
		HWND GetHandle() const noexcept;
		std::wstring GetWindowTitle()const noexcept;
		void SetWindowTitle(const std::wstring& windowTitle) noexcept;
		void ToggleFullscreen() noexcept;
		void SetPage(std::unique_ptr<IPage>&& newPage) noexcept;
	};
}