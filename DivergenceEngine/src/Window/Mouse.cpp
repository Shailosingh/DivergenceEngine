#include "Mouse.h"
#include <Windows.h>

namespace DivergenceEngine
{
	//Event Implementation-------------------------------------------------------------------------
	Mouse::Event::Event(Type type, const Mouse& parent) noexcept
		:
		EventType(type),
		LeftIsPressed(parent.LeftIsPressed),
		RightIsPressed(parent.RightIsPressed),
		MiddleIsPressed(parent.MiddleIsPressed),
		ScrollTicks(parent.ScrollTicks),
		PosX(parent.PosX),
		PosY(parent.PosY)
	{}

	Mouse::Event::Type Mouse::Event::GetType() const noexcept
	{
		return EventType;
	}
	
	DirectX::XMINT2 Mouse::Event::GetPos() const noexcept
	{
		return DirectX::XMINT2(PosX, PosY);
	}
	
	int Mouse::Event::GetPosX() const noexcept
	{
		return PosX;
	}

	int Mouse::Event::GetPosY() const noexcept
	{
		return PosY;
	}

	bool Mouse::Event::IsLeftPressed() const noexcept
	{
		return LeftIsPressed;
	}
	
	bool Mouse::Event::IsRightPressed() const noexcept
	{
		return RightIsPressed;
	}

	bool Mouse::Event::IsMiddlePressed() const noexcept
	{
		return MiddleIsPressed;
	}

	int Mouse::Event::GetScrollTicks() const noexcept
	{
		return ScrollTicks;
	}

	//Mouse Public Implementation------------------------------------------------------------------

	DirectX::XMINT2 Mouse::GetPos() const noexcept
	{
		return DirectX::XMINT2(PosX, PosY);
	}

	int Mouse::GetPosX() const noexcept
	{
		return PosX;
	}

	int Mouse::GetPosY() const noexcept
	{
		return PosY;
	}
	
	std::optional<Mouse::Event> Mouse::GetNextMouseEvent() noexcept
	{
		if (MouseQueue.empty())
		{
			return {};
		}
		
		Mouse::Event e = MouseQueue.front();
		MouseQueue.pop();
		return e;
	}

	bool Mouse::IsLeftPressed() const noexcept
	{
		return LeftIsPressed;
	}
	
	bool Mouse::IsRightPressed() const noexcept
	{
		return RightIsPressed;
	}

	bool Mouse::IsMiddlePressed() const noexcept
	{
		return MiddleIsPressed;
	}

	bool Mouse::IsQueueEmpty() const noexcept
	{
		return MouseQueue.empty();
	}

	void Mouse::FlushQueue() noexcept
	{
		MouseQueue = std::queue<Event>();
	}
	
	//Mouse Private Implementation-----------------------------------------------------------------
	
	void Mouse::OnMouseMove(int x, int y) noexcept
	{
		PosX = x;
		PosY = y;
		
		MouseQueue.push(Mouse::Event(Mouse::Event::Type::Move, *this));
		TrimQueue();
	}
	
	void Mouse::OnLeftPressed() noexcept
	{
		LeftIsPressed = true;
		
		MouseQueue.push(Mouse::Event(Mouse::Event::Type::LPress, *this));
		TrimQueue();
	}

	void Mouse::OnLeftReleased() noexcept
	{
		LeftIsPressed = false;

		MouseQueue.push(Mouse::Event(Mouse::Event::Type::LRelease, *this));
		TrimQueue();
	}
	
	void Mouse::OnRightPressed() noexcept
	{
		RightIsPressed = true;

		MouseQueue.push(Mouse::Event(Mouse::Event::Type::RPress, *this));
		TrimQueue();
	}
	
	void Mouse::OnRightReleased() noexcept
	{
		RightIsPressed = false;

		MouseQueue.push(Mouse::Event(Mouse::Event::Type::RRelease, *this));
		TrimQueue();
	}

	void Mouse::OnMiddlePressed() noexcept
	{
		MiddleIsPressed = true;

		MouseQueue.push(Mouse::Event(Mouse::Event::Type::MPress, *this));
		TrimQueue();
	}
	
	void Mouse::OnMiddleReleased() noexcept
	{
		MiddleIsPressed = false;

		MouseQueue.push(Mouse::Event(Mouse::Event::Type::MRelease, *this));
		TrimQueue();
	}
	
	void Mouse::OnWheelScroll(int wheelDelta) noexcept
	{
		//Calculate number of wheel ticks
		ScrollTicks = wheelDelta / WHEEL_DELTA;
		
		MouseQueue.push(Mouse::Event(Mouse::Event::Type::WheelScroll, *this));
		TrimQueue();
	}

	void Mouse::TrimQueue() noexcept
	{
		while (MouseQueue.size() > MAX_QUEUE_SIZE)
		{
			MouseQueue.pop();
		}
	}
}