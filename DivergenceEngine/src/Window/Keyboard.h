#pragma once
#include <cstdint>
#include <queue>
#include <bitset>
#include <optional>

namespace DivergenceEngine
{
	class Keyboard
	{
		//Allow Window class to access private members for the WinProc
		friend class Window;
		
	public:
		//Event class
		class Event
		{
		public:
			enum class Type
			{
				Press,
				Release,
				Invalid
			};

			Event(Type type, uint8_t code) noexcept;
			
			bool IsPress() const noexcept;
			bool IsRelease() const noexcept;
			uint8_t GetCode() const noexcept;			
			
		private:
			Type EventType;
			uint8_t KeyCode;
		};
		
		Keyboard() = default;
		Keyboard(const Keyboard&) = delete;
		Keyboard& operator=(const Keyboard&) = delete;

		//Key stuff
		bool IsKeyPressed(uint8_t keyCode) const noexcept;
		std::optional<Event> ReadKey() noexcept;
		bool KeyQueueIsEmpty() const noexcept;
		void FlushKeyQueue() noexcept;
		
		//Character stuff
		std::optional<wchar_t> ReadChar() noexcept;
		bool CharQueueIsEmpty() const noexcept;
		void FlushCharQueue() noexcept;

		//Autorepeat control
		void ToggleKeyAutoRepeat(bool autoRepeatState) noexcept;
		bool AutoRepeatIsEnabled() const noexcept;

	private:
		//Constants
		static constexpr size_t NUM_KEYSTATES = 256;
		static constexpr size_t MAX_QUEUE_SIZE = 64;
		
		//Datafields
		bool AutoRepeatEnabled = false;
		std::bitset<NUM_KEYSTATES> KeyStates;
		std::queue<Event> KeyQueue;
		std::queue<wchar_t> CharQueue;

		//Functions for use in WinProc
		void OnKeyPressed(uint8_t keyCode) noexcept;  //WM_KEYUP and WM_SYSKEYUP
		void OnKeyReleased(uint8_t keyCode) noexcept; //WM_KEYDOWN and WM_SYSKEYDOWN
		void OnChar(wchar_t character) noexcept; //WM_CHAR
		void ClearKeyStates() noexcept; //WM_KILLFOCUS

		//Helpers
		template<typename T>
		static void TrimQueue(std::queue<T>& queue) noexcept;
	};
}