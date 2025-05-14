#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <regex>

namespace COMMAND_PARAM_INDEX_WRITE {
	enum COMMAND_PARAM_INDEX_WRITE {
		COMMAND_NAME = 0,
		ADDRESS,
		VALUE
	};
}

namespace COMMAND_PARAM_INDEX_ERASE {
	enum COMMAND_PARAM_INDEX_ERASE {
		COMMAND_NAME = 0,
		ADDRESS,
		SIZE
	};
}

class CommandBuffer{
public:
	CommandBuffer();
	~CommandBuffer() {}

	bool IsFull();
	std::vector<std::string> Flush();
	std::string FastRead(int address);
	void AppendCommand(const std::string& command);
	std::vector<std::string> ReadBuffers();
	std::vector<std::string> ApplyIgnoreStrategy(const std::string& command);

private:
	void InitBuffers();
	
	void ApplyMergeStrategy();

	bool IsEmptyBuffer(const std::string& bufferName);
	std::string GetBufferContent(std::string bufferName);

	int CountFilesInBuffer();
	void MakeEmptyFiles();

	std::string GetFirstEmptyBuffer();
	void WriteCommandToBuffer(
		const std::string& emptyBuffer, 
		const std::string& command);
	bool BufferDirectoryExist();
	std::string GetBufferFullPath(const std::string & buffer);
	std::string GetBufferIndexAtBufferName(const std::string& bufferName);
	std::string MakeCommandFromFile(const std::filesystem::directory_entry& file);
	std::string removeIndex(const std::string& fileName);
	void RemoveBufferDirectory();


private:
	std::vector<std::string> buffers;

	inline static constexpr int MAX_BUFFER_SIZE = 5;
	inline static const std::string BUFFER_DIR_PATH{ "buffer" };
	inline static const std::string EMPTY_BUFFER_NAME{ "empty" };
	inline static const std::string DELIMITER{ "_" };

	inline static const std::string WRITE_COMMAND_NAME = { "W" };


};


inline bool CommandBuffer::BufferDirectoryExist() {
	namespace fs = std::filesystem;

	return fs::exists(BUFFER_DIR_PATH) 
		&& fs::is_directory(BUFFER_DIR_PATH);
}

inline std::string CommandBuffer::GetBufferFullPath(const std::string& buffer) {
	return BUFFER_DIR_PATH + "/" + buffer;
}

inline bool CommandBuffer::IsEmptyBuffer(const std::string& bufferName) {
	return bufferName.find(EMPTY_BUFFER_NAME) != std::string::npos;
}

inline std::string CommandBuffer::GetBufferIndexAtBufferName(const std::string& bufferName) {
	std::size_t pos = bufferName.find('_');
	if (pos != std::string::npos) {
		return bufferName.substr(0, pos + 1);
	}
	return "";		
}

inline void CommandBuffer::RemoveBufferDirectory() {
	std::filesystem::remove_all(BUFFER_DIR_PATH);
}