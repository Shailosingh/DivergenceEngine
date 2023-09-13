#pragma once
#include <cstdint>
#include <string>

namespace DivergenceEngine
{
	class WAVFileReader
	{
	public:
		struct FMTChunk
		{
			uint16_t AudioFormat;
			uint16_t NumChannels;
			uint32_t SampleRate;
			uint32_t ByteRate;
			uint16_t BlockAlign;
			uint16_t BitsPerSample;
		};
		
		struct WAVFileInfo
		{
			std::wstring FilePath;
			FMTChunk FormatChunk;
			uint32_t DataChunkSize;
			size_t DataChunkOffsetInFile;
		};

		static WAVFileInfo ReadWAVFile(const std::wstring& filePath);

	private:
		//Helpers
		static bool IsCorrectFourCC(const char* chunkFourCC, const char* correctFourCC);
		
		//Internal structures for WAV reading
		static inline const char RIFF_HEADER[4] = { 'R', 'I', 'F', 'F' };
		static inline const char WAVE_FORMAT[4] = { 'W', 'A', 'V', 'E' };
		struct RIFFHeader
		{
			char Header[4];
			uint32_t RemainingFileSize;
			char Format[4];
		};

		struct SubchunkHeader
		{
			char Header[4];
			uint32_t ChunkSize;
		};

		static inline const char FMT_CHUNK[4] = { 'f', 'm', 't', ' ' };
		static const uint16_t PCM_FORMAT = 1;

		static inline const char DATA_CHUNK[4] = { 'd', 'a', 't', 'a' };
	};
}
