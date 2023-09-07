#pragma once
#include <Audio.h>

//https://github.com/microsoft/DirectXTK/issues/266
//https://github.com/microsoft/DirectXTK/wiki/Audio

namespace DivergenceEngine
{
	class AudioProcessor
	{
	private:
		
	public:
		//Constructors
		AudioProcessor();
		~AudioProcessor();

		//Volume controls
		void SetMasterVolume(float volume);
		float GetMasterVolume() const;
		void SetMusicVolume(float volume);
		float GetMusicVolume() const;
		void SetSFXVolume(float volume);
		float GetSFXVolume() const;
		void SetVoiceVolume(float volume);
		float GetVoiceVolume() const;

		//Audio adders
		void AddMusic(std::wstring path, bool isLoop = true);
		void AddSFX(std::wstring path, bool isLoop = false);
		void AddVoice(std::wstring path, bool isLoop = false);

		//Audio removers
		void RemoveMusic(std::wstring path);
		void RemoveSFX(std::wstring path);
		void RemoveVoice(std::wstring path);
		void ClearAllSounds();
	};
}