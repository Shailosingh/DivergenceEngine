#pragma once
#include "Audio/IAudioInstance.h"
#include <memory>
#include <string>
#include <array>
#include <vector>
#include <fstream>
#include "Audio/WAVFileReader.h"

//http://soundfile.sapp.org/doc/WaveFormat/
//https://en.wikipedia.org/wiki/WAV
//https://github.com/microsoft/DirectXTK/wiki/Audio

namespace DivergenceEngine
{
	class WAVAudioInstance : public IAudioInstance
	{
	private:
		//General Datafields
		DirectX::AudioEngine* EnginePointer;
		std::unique_ptr<DirectX::DynamicSoundEffectInstance> SoundEffectInstance;
		uint8_t PlaybackSpeedMultiplier;
		bool IsLoop;

		//Buffer variables
		const int32_t MAX_BUFFERS = 5;
		uint32_t DataChunkCurrentIndex;
		bool StopLoadingBuffers = false; //This is necessary so, if the callback calls again after this is set to true, it does not try to load buffers again

		//Memory mapped file datafields
		HANDLE FileHandle;
		HANDLE FileMappingHandle;
		uint8_t* OriginalFileMappingPointer;
		uint8_t* DataChunkFileMappingPointer;

		//WAV Datafields
		WAVFileReader::WAVFileInfo FileInfo;

		//Buffer functions
		void BufferNeeded(DirectX::DynamicSoundEffectInstance* instance);
		
	public:
		//Constructors and destructors
		WAVAudioInstance(DirectX::AudioEngine* engine, std::wstring filePath, uint8_t initialPlaybackSpeedMultiplier = 1, float initialVolume = 1);
		~WAVAudioInstance();

		//Overriden functions
		void Play(bool isLoop = true) override;
		void Stop() override;
		void Pause() override;
		void Resume() override;
		void SetVolume(float newVolume) override;
		void SetPlaybackSpeedMultiplier(uint8_t newPlaybackSpeedMultiplier) override;
	};
}