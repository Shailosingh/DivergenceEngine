#include "Audio//OGGSimpleSoundEffect.h"
#include "Logger/Logger.h"
#include <filesystem>
#include <memory>

namespace fs = std::filesystem;

namespace DivergenceEngine
{
	OGGSimpleSoundEffect::OGGSimpleSoundEffect(DirectX::AudioEngine* engine, const std::wstring& filePath)
	{
		//Validate arguments
		if (engine == nullptr)
		{
			throw std::invalid_argument("OGGSimpleSoundEffect::OGGSimpleSoundEffect() - engine is nullptr");
		}

		//Try to open the file C style
		FILE* fileObjectPointer = nullptr;
		errno_t fileError = _wfopen_s(&fileObjectPointer, filePath.c_str(), L"rb");
		if (fileError != 0)
		{
			throw std::invalid_argument("OGGSimpleSoundEffect::OGGSimpleSoundEffect() - filePath cannot be opened");
		}
		FilePath = filePath;

		//Open the file as a Vorbis file
		int vorbisError = ov_open_callbacks(fileObjectPointer, &VorbisFileObject, nullptr, 0, OV_CALLBACKS_DEFAULT);
		if (vorbisError != 0)
		{
			throw std::invalid_argument("OGGSimpleSoundEffect::OGGSimpleSoundEffect() - filePath is not a valid Vorbis file");
		}

		//Ensure that it only had one logical stream
		if (ov_streams(&VorbisFileObject) != 1)
		{
			throw std::invalid_argument("OGGAudioInstance::OGGAudioInstance() - filePath has more than one logical stream");
		}

		//Get the file information
		VorbisInfo = ov_info(&VorbisFileObject, -1);

		//Calculate the block align
		BlockAlign = VorbisInfo->channels * BIT_DEPTH / 8;

		//Calculate the target uncompressed size
		uint64_t targetUncompressedSize = ov_pcm_total(&VorbisFileObject, -1) * BlockAlign;

		//Get the uncompressed data
		std::vector<uint8_t> audioData;
		audioData.resize(targetUncompressedSize+1);
		long totalBytesRead = 0;
		while (true)
		{
			//https://stackoverflow.com/questions/8653670/vorbis-finding-decompressed-size-of-file
			//According to the guy in the link above, sometimes ov_pcm_total() returns a value that is too small by about a KB so, if this happens, I reallocate my one KB. This is why I use a vector instead of just an ordinary C array I convert this into below
			if (totalBytesRead >= audioData.size())
			{
				audioData.resize(audioData.size() + 1024);
			}
			long currentBytesRead = ov_read(&VorbisFileObject, reinterpret_cast<char*>(&audioData[totalBytesRead]), audioData.size() - totalBytesRead, 0, BIT_DEPTH / 8, 1, nullptr);

			if (currentBytesRead == 0)
			{
				break;
			}

			else if (currentBytesRead > 0)
			{
				totalBytesRead += currentBytesRead;
			}
		}

		//Shave off the excess allocated memory
		audioData.resize(totalBytesRead);

		//Create the wave data unique ptr buffer and copy the data from the vector into it
		WaveData = std::make_unique<uint8_t[]>(audioData.size() + sizeof(WAVEFORMATEX));
		uint8_t* startAudio = WaveData.get() + sizeof(WAVEFORMATEX);
		memcpy(startAudio, audioData.data(), audioData.size());

		//Construct the WAVEFORMATEX structure
		WAVEFORMATEX* waveFormat = reinterpret_cast<WAVEFORMATEX*>(WaveData.get());;
		waveFormat->wFormatTag = WAVE_FORMAT_PCM;
		waveFormat->nChannels = VorbisInfo->channels;
		waveFormat->nSamplesPerSec = VorbisInfo->rate;
		waveFormat->nAvgBytesPerSec = VorbisInfo->rate * BlockAlign;
		waveFormat->nBlockAlign = BlockAlign;
		waveFormat->wBitsPerSample = BIT_DEPTH;
		waveFormat->cbSize = 0;

		//Create the sound effect
		SimpleSoundEffect = std::make_unique<DirectX::SoundEffect>(engine, WaveData, waveFormat, startAudio, totalBytesRead);

		Logger::Log(std::format(L"Loaded {}", FilePath));
	}
	
	OGGSimpleSoundEffect::~OGGSimpleSoundEffect()
	{
		ov_clear(&VorbisFileObject);
		Logger::Log(std::format(L"Destroyed {}", FilePath));
	}

	void OGGSimpleSoundEffect::Play(float volume)
	{
		if (volume < 0 || volume > 1)
		{
			throw std::invalid_argument("OGGSimpleSoundEffect::Play() - volume is not in range [0, 1]");
		}

		SimpleSoundEffect->Play(volume, 0, 0);
	}
}