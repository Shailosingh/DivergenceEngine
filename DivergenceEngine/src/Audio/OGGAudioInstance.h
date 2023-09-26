#pragma once
#include "Audio/IAudioInstance.h"
#include "vorbis/vorbisfile.h"
#include <array>
#include <vector>
#include <mutex>
#include <Windows.h>
#include <atomic>

namespace DivergenceEngine
{
	class OGGAudioInstance : public IAudioInstance
	{
	private:
		//General Datafields
		DirectX::AudioEngine* EnginePointer;
		std::unique_ptr<DirectX::DynamicSoundEffectInstance> SoundEffectInstance;
		uint8_t PlaybackSpeedMultiplier;
		bool IsLoop;

		//Buffer variables
		const static uint32_t MAX_BUFFERS = 5;
		const static long MAX_BUFFER_SIZE = 4 * 1024;
		const static uint32_t NUMBER_OF_BANKS = 2;
		const static uint32_t NUMBER_OF_EVENTS = NUMBER_OF_BANKS + 1;
		const static uint32_t THREAD_EXIT_EVENT_INDEX = NUMBER_OF_EVENTS - 1;
		const static long MAX_BANK_SIZE = 1024 * 1024;
		uint32_t CurrentBankIndex = 0;
		long CurrentBankDataIndex = 0;
		bool ThreadIsRunning;
		bool StopLoadingBuffers = false;
		std::array<std::array<uint8_t, MAX_BANK_SIZE>, NUMBER_OF_BANKS> BankArray;
		std::array<long, NUMBER_OF_BANKS> TrueBankSizeArray;
		std::array<std::mutex, NUMBER_OF_BANKS> BankMutexArray;
		std::array<std::atomic<bool>, NUMBER_OF_EVENTS> BankLoadEventArray;
		std::thread BankLoadingThreadObject;

		//Buffer functions
		void BufferNeeded(DirectX::DynamicSoundEffectInstance* instance);
		void BankLoadingThread();
		void LoadBank(uint32_t bankIndex);

		//OGG variables
		std::wstring FilePath;
		OggVorbis_File VorbisFileObject;
		vorbis_info* VorbisInfo;
		ogg_int64_t TotalSamples;
		const int BIT_DEPTH = 16;
		uint64_t BlockAlign;

	public:
		//Constructors and Destructors
		OGGAudioInstance(DirectX::AudioEngine* engine, std::wstring filePath, uint8_t initialPlaybackSpeedMultiplier = 1, float initialVolume = 1);
		~OGGAudioInstance();

		//Overriden functions
		void Play(bool isLoop = true) override;
		void Stop() override;
		void Pause() override;
		void Resume() override;
		void SetVolume(float newVolume) override;
		void SetPlaybackSpeedMultiplier(uint8_t newPlaybackSpeedMultiplier) override;
	};
}