#include "Keyboard.h"

namespace DivergenceEngine
{
	//Event implementation----------------------------------------------------------------------------
	Keyboard::Event::Event(Type type, uint8_t code) noexcept
		:
		EventType(type),
		KeyCode(code)
	{}

	bool Keyboard::Event::IsPress() const noexcept
	{
		return EventType == Type::Press;
	}

	bool Keyboard::Event::IsRelease() const noexcept
	{
		return EventType == Type::Release;
	}

	uint8_t Keyboard::Event::GetCode() const noexcept
	{
		return KeyCode;
	}

	//Keyboard Public Implementation---------------------------------------------------------------
	bool Keyboard::IsKeyPressed(uint8_t keyCode) const noexcept
	{
		return KeyStates[keyCode];
	}

	std::optional<Keyboard::Event> Keyboard::ReadKey() noexcept
	{
		if (!KeyQueue.empty())
		{
			Keyboard::Event frontEvent = KeyQueue.front();
			KeyQueue.pop();
			return frontEvent;
		}
		
		return {};
	}

	bool Keyboard::KeyQueueIsEmpty() const noexcept
	{
		return KeyQueue.empty();
	}

	void Keyboard::FlushKeyQueue() noexcept
	{
		//Reset the key queue
		KeyQueue = std::queue<Event>();
	}

	std::optional<wchar_t> Keyboard::ReadChar() noexcept
	{
		if (!CharQueue.empty())
		{
			wchar_t frontChar = CharQueue.front();
			CharQueue.pop();
			return frontChar;
		}

		return {};
	}

	bool Keyboard::CharQueueIsEmpty() const noexcept
	{
		return CharQueue.empty();
	}
	
	void Keyboard::FlushCharQueue() noexcept
	{
		//Reset the char queue
		CharQueue = std::queue<wchar_t>();
	}
	
	void Keyboard::ToggleKeyAutoRepeat(bool autoRepeatState) noexcept
	{
		AutoRepeatEnabled = autoRepeatState;
	}

	bool Keyboard::AutoRepeatIsEnabled() const noexcept
	{
		return AutoRepeatEnabled;
	}
	
	//WinProc functions----------------------------------------------------------------------------
	void Keyboard::OnKeyPressed(uint8_t keyCode) noexcept
	{
		KeyStates[keyCode] = true;
		KeyQueue.push(Keyboard::Event(Keyboard::Event::Type::Press, keyCode));
		TrimQueue(KeyQueue);
	}

	void Keyboard::OnKeyReleased(uint8_t keyCode) noexcept
	{
		KeyStates[keyCode] = false;
		KeyQueue.push(Keyboard::Event(Keyboard::Event::Type::Release, keyCode));
		TrimQueue(KeyQueue);
	}

	void Keyboard::OnChar(wchar_t character) noexcept
	{
		CharQueue.push(character);
		TrimQueue(CharQueue);
	}

	void Keyboard::ClearKeyStates() noexcept
	{
		KeyStates.reset();
	}

	//Helpers--------------------------------------------------------------------------------------
	template<typename T>
	void Keyboard::TrimQueue(std::queue<T>& queue) noexcept
	{
		while (queue.size() > MAX_QUEUE_SIZE)
		{
			queue.pop();
		}
	}
}