#include "Audio/WAVFileReader.h"
#include <filesystem>
#include <fstream>
#include "StringConverter.h"
#include <cassert>

namespace fs = std::filesystem;

namespace DivergenceEngine
{
	WAVFileReader::WAVFileInfo WAVFileReader::ReadWAVFile(const std::wstring& filePath)
	{
		WAVFileInfo wavFileInfo;
		
		if (!fs::exists(filePath))
		{
			throw std::invalid_argument("WAVAudioInstance::WAVAudioInstance() - filePath does not exist");
		}
		wavFileInfo.FilePath = filePath;

		//Open up the file for binary reading
		std::ifstream fileInputReader;
		fileInputReader.open(wavFileInfo.FilePath, std::ios::binary);
		fileInputReader >> std::noskipws;
		if (!fileInputReader)
		{
			assert(false);
			throw std::runtime_error(DivergenceEngine::StringConverter::ConvertWideStringToANSI(std::format(L"WAVAudioInstance::WAVAudioInstance() - failed to open '{}' for reading", wavFileInfo.FilePath)));
		}
		
		//Read the RIFF header
		RIFFHeader riffHeader;
		fileInputReader.read(reinterpret_cast<char*>(&riffHeader), sizeof(RIFFHeader));

		//Check if the RIFF header is correct
		size_t correctRemainingFileSize = fs::file_size(wavFileInfo.FilePath) - 8;
		bool isCorrectRemainingFileSize = riffHeader.RemainingFileSize == correctRemainingFileSize;
		if (!(IsCorrectFourCC(riffHeader.Header, RIFF_HEADER) && IsCorrectFourCC(riffHeader.Format, WAVE_FORMAT) && isCorrectRemainingFileSize))
		{
			assert(false);
			throw std::runtime_error(DivergenceEngine::StringConverter::ConvertWideStringToANSI(std::format(L"WAVAudioInstance::WAVAudioInstance() - '{}' is not a valid RIFF file", wavFileInfo.FilePath)));
		}

		//Search for the FMT chunk
		SubchunkHeader subchunkHeader;
		bool isFoundFmtChunk = false;
		while (fileInputReader.read(reinterpret_cast<char*>(&subchunkHeader), sizeof(SubchunkHeader)))
		{
			if (IsCorrectFourCC(subchunkHeader.Header, FMT_CHUNK))
			{
				isFoundFmtChunk = true;
				break;
			}
			else
			{
				fileInputReader.seekg(subchunkHeader.ChunkSize, std::ios::cur);
			}
		}

		//Check if the FMT chunk was found
		if (!isFoundFmtChunk)
		{
			assert(false);
			throw std::runtime_error(DivergenceEngine::StringConverter::ConvertWideStringToANSI(std::format(L"WAVAudioInstance::WAVAudioInstance() - '{}' does not contain a FMT chunk", wavFileInfo.FilePath)));
		}

		//Read the FMT chunk
		fileInputReader.read(reinterpret_cast<char*>(&wavFileInfo.FormatChunk), sizeof(FMTChunk));

		//Ensure the data will be PCM
		if (wavFileInfo.FormatChunk.AudioFormat != PCM_FORMAT)
		{
			assert(false);
			throw std::runtime_error(DivergenceEngine::StringConverter::ConvertWideStringToANSI(std::format(L"WAVAudioInstance::WAVAudioInstance() - '{}' is not PCM", wavFileInfo.FilePath)));
		}

		//Search for data chunk
		bool isFoundDataChunk = false;
		while (fileInputReader.read(reinterpret_cast<char*>(&subchunkHeader), sizeof(SubchunkHeader)))
		{
			if (IsCorrectFourCC(subchunkHeader.Header, DATA_CHUNK))
			{
				isFoundDataChunk = true;
				break;
			}
			else
			{
				fileInputReader.seekg(subchunkHeader.ChunkSize, std::ios::cur);
			}
		}

		//Check if the data chunk was found
		if (!isFoundDataChunk)
		{
			assert(false);
			throw std::runtime_error(DivergenceEngine::StringConverter::ConvertWideStringToANSI(std::format(L"WAVAudioInstance::WAVAudioInstance() - '{}' does not contain a DATA chunk", wavFileInfo.FilePath)));
		}

		//Read the data chunk size
		wavFileInfo.DataChunkSize = subchunkHeader.ChunkSize;

		//Record the current byte offset
		wavFileInfo.DataChunkOffsetInFile = fileInputReader.tellg();

		//Close up the file reader
		fileInputReader.close();

		return wavFileInfo;
	}
	
	bool WAVFileReader::IsCorrectFourCC(const char* chunkFourCC, const char* correctFourCC)
	{
		for (int index = 0; index < 4; index++)
		{
			if (chunkFourCC[index] != correctFourCC[index])
			{
				return false;
			}
		}

		return true;
	}
}