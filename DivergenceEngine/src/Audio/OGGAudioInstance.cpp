#include "Audio/IAudioInstance.h"
#include "Audio/OGGAudioInstance.h"
#include "Logger/Logger.h"
#include <stdexcept>
#include <stdio.h>
#include <format>

//https://xiph.org/vorbis/doc/vorbisfile/overview.html
//https://github.com/edubart/minivorbis

namespace DivergenceEngine
{
	OGGAudioInstance::OGGAudioInstance(DirectX::AudioEngine* engine, std::wstring filePath, uint8_t initialPlaybackSpeedMultiplier, float initialVolume)
	{
		//Handle invalid parameters
		if (engine == nullptr)
		{
			throw std::invalid_argument("OGGAudioInstance::OGGAudioInstance() - engine cannot be nullptr");
		}
		EnginePointer = engine;

		if (initialPlaybackSpeedMultiplier == 0)
		{
			throw std::invalid_argument("OGGAudioInstance::OGGAudioInstance() - initialPlaybackSpeedMultiplier cannot be 0");
		}
		PlaybackSpeedMultiplier = initialPlaybackSpeedMultiplier;

		if (initialVolume > 1 || initialVolume < 0)
		{
			throw std::invalid_argument("OGGAudioInstance::OGGAudioInstance() - initialVolume must be between 0 and 1");
		}

		//Try to open the file C style
		FILE* fileObjectPointer = nullptr;
		errno_t fileError = _wfopen_s(&fileObjectPointer, filePath.c_str(), L"rb");
		if (fileError != 0)
		{
			throw std::invalid_argument("OGGAudioInstance::OGGAudioInstance() - filePath cannot be opened");
		}
		FilePath = filePath;

		//Open the file as a Vorbis file
		int vorbisError = ov_open_callbacks(fileObjectPointer, &VorbisFileObject, nullptr, 0, OV_CALLBACKS_DEFAULT);
		if(vorbisError != 0)
		{
			throw std::invalid_argument("OGGAudioInstance::OGGAudioInstance() - filePath is not a valid Vorbis file");
		}

		//Ensure that it only had one logical stream
		if (ov_streams(&VorbisFileObject) != 1)
		{
			throw std::invalid_argument("OGGAudioInstance::OGGAudioInstance() - filePath has more than one logical stream");
		}

		//Ensure that the file is seekable
		if (ov_seekable(&VorbisFileObject) == 0)
		{
			throw std::invalid_argument("OGGAudioInstance::OGGAudioInstance() - filePath is not seekable");
		}

		//Get the file information
		VorbisInfo = ov_info(&VorbisFileObject, -1);

		//Get the total number of PCM samples
		TotalSamples = ov_pcm_total(&VorbisFileObject, -1);

		//Calculate the block align
		BlockAlign = VorbisInfo->channels * BIT_DEPTH / 8;

		//Create the sound instance
		SoundEffectInstance = std::make_unique<DirectX::DynamicSoundEffectInstance>
			(
				EnginePointer,
				std::bind(&OGGAudioInstance::BufferNeeded, this, std::placeholders::_1),
				VorbisInfo->rate * PlaybackSpeedMultiplier,
				VorbisInfo->channels,
				BIT_DEPTH
			);

		//Set the volume
		SoundEffectInstance->SetVolume(initialVolume);

		Logger::Log(std::format(L"Loaded {}", FilePath));
	}

	OGGAudioInstance::~OGGAudioInstance()
	{
		SoundEffectInstance->Pause();
		ov_clear(&VorbisFileObject);
		Logger::Log(std::format(L"Destroyed {}", FilePath));
	}

	void OGGAudioInstance::Play(bool isLoop)
	{
		IsLoop = isLoop;
		SoundEffectInstance->Play();
	}

	void OGGAudioInstance::Stop()
	{
		SoundEffectInstance->Stop();
		ov_pcm_seek(&VorbisFileObject, 0);
	}

	void OGGAudioInstance::Pause()
	{
		SoundEffectInstance->Pause();
	}

	void OGGAudioInstance::Resume()
	{
		SoundEffectInstance->Resume();
	}

	void OGGAudioInstance::SetVolume(float volume)
	{
		if (volume > 1 || volume < 0)
		{
			throw std::invalid_argument("WAVAudioInstance::SetVolume() - volume must be between 0 and 1");
		}

		SoundEffectInstance->SetVolume(volume);
	}

	void OGGAudioInstance::SetPlaybackSpeedMultiplier(uint8_t newPlaybackSpeedMultiplier)
	{
		//Ensure that the new playback speed multiplier is not 0
		if (newPlaybackSpeedMultiplier == 0)
		{
			throw std::invalid_argument("OGGAudioInstance::SetPlaybackSpeedMultiplier() - newPlaybackSpeedMultiplier cannot be 0");
		}

		//If the new playback speed is unchanged, do nothing
		if (PlaybackSpeedMultiplier == newPlaybackSpeedMultiplier)
		{
			return;
		}

		//Check if the song is already playing and if it was looping
		bool isPlaying = SoundEffectInstance->GetState() == DirectX::SoundState::PLAYING;

		//Reset the instance with the altered sample rate based on the new playback speed multiplier
		this->Pause();
		PlaybackSpeedMultiplier = newPlaybackSpeedMultiplier;
		SoundEffectInstance = std::make_unique<DirectX::DynamicSoundEffectInstance>
			(
				EnginePointer,
				std::bind(&OGGAudioInstance::BufferNeeded, this, std::placeholders::_1),
				VorbisInfo->rate * PlaybackSpeedMultiplier,
				VorbisInfo->channels,
				BIT_DEPTH
			);
		this->Play(IsLoop);

		if (!isPlaying)
		{
			this->Pause();
		}
	}

	void OGGAudioInstance::BufferNeeded(DirectX::DynamicSoundEffectInstance* instance)
	{
		uint32_t targetBufferSize = 32768 * PlaybackSpeedMultiplier;
		int logicalBitstream = 0;

		//https://irrlicht.sourceforge.io/forum/viewtopic.php?p=302464
		//https://xiph.org/vorbis/doc/vorbisfile/decoding.html (Use loop to iteratively fill buffer until it is max)
		//LOAD 1 MiB (1,048,576 byte) BANKS OF DATA AT A TIME AND THEN FEED 4096 BYTE CHUNKS OF IT TO THE INSTANCE SLOWLY. Have two banks, one that is being filled in a worker thread and one that is being used.
		while (!StopLoadingBuffers && instance->GetState() == DirectX::PLAYING && instance->GetPendingBufferCount() < MAX_BUFFERS)
		{
			//Get the number of new buffers to fill
			uint32_t numBuffersToFill = MAX_BUFFERS - instance->GetPendingBufferCount();

			//Cycle through each of the eaten buffers and fill them
			for (size_t index = 0; index < numBuffersToFill; index++)
			{
				Buffers[index].resize(targetBufferSize);

				//Fill in buffer
				long actualBytesRead = 0;
				while (actualBytesRead < targetBufferSize)
				{
					long currentByteRead = ov_read(&VorbisFileObject, reinterpret_cast<char*>(Buffers[index].data()) + actualBytesRead, targetBufferSize - actualBytesRead, 0, BIT_DEPTH / 8, 1, &logicalBitstream);
					actualBytesRead += currentByteRead;

					//If the actual bytes read is 0, then the end of the file has been reached
					if (currentByteRead == 0)
					{
						//If the song is looping, seek to the beginning of the file
						if (IsLoop)
						{
							ov_pcm_seek(&VorbisFileObject, 0);
						}

						else
						{
							StopLoadingBuffers = true;
							break;
						}
					}
				}

				//Submit the buffer
				instance->SubmitBuffer(Buffers[index].data(), actualBytesRead);

				if (StopLoadingBuffers)
				{
					break;
				}

				/*
				//Fill in buffer
				Buffers[index].resize(targetBufferSize);
				long actualBytesRead = ov_read(&VorbisFileObject, reinterpret_cast<char*>(Buffers[index].data()), targetBufferSize, 0, BIT_DEPTH / 8, 1, &logicalBitstream);

				//Submit the buffer
				instance->SubmitBuffer(Buffers[index].data(), actualBytesRead);

				//If the actual bytes read is 0, then the end of the file has been reached
				if (actualBytesRead == 0)
				{
					//If the song is looping, seek to the beginning of the file
					if (IsLoop)
					{
						ov_pcm_seek(&VorbisFileObject, 0);
					}

					else
					{
						StopLoadingBuffers = true;
						break;
					}
				}
				*/
			}
		}

		//If the song has stopped loading buffers and there are no new buffers, stop the song
		if (StopLoadingBuffers && instance->GetPendingBufferCount() == 0)
		{
			this->Stop();
		}
	}
}