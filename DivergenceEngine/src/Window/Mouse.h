#pragma once
#include <queue>
#include <optional>
#include <directxmath.h>

namespace DivergenceEngine
{
	class Mouse
	{
		//Allow Window class to access private members for the WinProc
		friend class Window;
		
	public:
		//Constructors and operators
		Mouse() = default;
		Mouse(const Mouse&) = delete;
		Mouse& operator=(const Mouse&) = delete;

		//Getters
		DirectX::XMINT2 GetPos() const noexcept;
		int GetPosX() const noexcept;
		int GetPosY() const noexcept;

		//Status checkers
		bool IsLeftPressed() const noexcept;
		bool IsRightPressed() const noexcept;
		bool IsMiddlePressed() const noexcept;
		bool IsQueueEmpty() const noexcept;

	private:
		class Event //TODO: Make this and the queue private so the Window and not the Page handles this
		{
		public:
			enum class Type
			{
				LPress, //Mouse press and releases are directed to individual drawables and should be handled by the Window so it can be directed to drawable components
				LRelease,
				RPress,
				RRelease,
				MPress,
				MRelease,
				WheelScroll, //Scrolls and mouse moves are directed to the entire page and shall be handled by the virtual functions in the Page
				Move
			};

			Event(Type type, const Mouse& parent) noexcept;
			Type GetType() const noexcept;
			DirectX::XMINT2 GetPos() const noexcept;
			int GetPosX() const noexcept;
			int GetPosY() const noexcept;
			bool IsLeftPressed() const noexcept;
			bool IsRightPressed() const noexcept;
			bool IsMiddlePressed() const noexcept;
			int GetScrollTicks() const noexcept;

		private:
			//Event Datafields
			Type EventType;
			bool LeftIsPressed;
			bool RightIsPressed;
			bool MiddleIsPressed;
			int PosX;
			int PosY;
			int ScrollTicks;
		};

		static constexpr unsigned int MAX_QUEUE_SIZE = 64u;
		std::queue<Event> MouseQueue;
		std::optional<Mouse::Event> GetNextMouseEvent() noexcept;
		void FlushQueue() noexcept;
		
		int ScrollTicks = 0;
		int PosX = 0;
		int PosY = 0;
		bool LeftIsPressed = false;
		bool RightIsPressed = false;
		bool MiddleIsPressed = false;

		void OnMouseMove(int x, int y) noexcept;
		void OnLeftPressed() noexcept;
		void OnLeftReleased() noexcept;
		void OnRightPressed() noexcept;
		void OnRightReleased() noexcept;
		void OnMiddlePressed() noexcept;
		void OnMiddleReleased() noexcept;
		void OnWheelScroll(int wheelDelta) noexcept;
		void TrimQueue() noexcept;
	};
}
