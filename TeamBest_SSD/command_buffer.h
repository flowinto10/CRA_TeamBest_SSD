#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>

class CommandBuffer{
public:
	CommandBuffer();
	~CommandBuffer() {}

	bool IsFull();
	std::vector<std::string> Flush();
	std::string FastRead();
	void AppendCommand(const std::string& command);

private:
	void InitBuffers();
	std::vector<std::string> ReadBuffers();	
	void ApplyIgnoreStrategy();
	void ApplyMergeStrategy();

	bool IsEmptyBuffer(const std::string& bufferName);
	std::string GetBufferContent(std::string bufferName);
	std::string GetFirstEmptyBuffer();
	void WriteCommandToBuffer(
		const std::string& emptyBuffer, 
		const std::string& command);
	bool BufferDirectoryExist();
	std::string GetBufferFullPath(const std::string & buffer);
	std::string GetBufferIndexAtBufferName(const std::string& bufferName);

private:
	std::vector<std::string> buffers;

	inline static constexpr int MAX_BUFFER_SIZE = 5;
	inline static const std::string BUFFER_DIR_PATH{ "buffer" };
	inline static const std::string EMPTY_BUFFER_NAME{ "empty" };


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
