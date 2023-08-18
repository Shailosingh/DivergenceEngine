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
		class Event
		{
		public:
			enum class Type
			{
				LPress,
				LRelease,
				RPress,
				RRelease,
				MPress,
				MRelease,
				WheelScroll,
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

		//Constructors and operators
		Mouse() = default;
		Mouse(const Mouse&) = delete;
		Mouse& operator=(const Mouse&) = delete;

		//Getters
		DirectX::XMINT2 GetPos() const noexcept;
		int GetPosX() const noexcept;
		int GetPosY() const noexcept;
		std::optional<Mouse::Event> GetNextMouseEvent() noexcept;

		//Status checkers
		bool IsLeftPressed() const noexcept;
		bool IsRightPressed() const noexcept;
		bool IsMiddlePressed() const noexcept;
		bool IsQueueEmpty() const noexcept;

		void FlushQueue() noexcept; 
		
	private:
		static constexpr unsigned int MAX_QUEUE_SIZE = 64u;
		int ScrollTicks = 0;
		int PosX = 0;
		int PosY = 0;
		bool LeftIsPressed = false;
		bool RightIsPressed = false;
		bool MiddleIsPressed = false;
		std::queue<Event> MouseQueue;

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
