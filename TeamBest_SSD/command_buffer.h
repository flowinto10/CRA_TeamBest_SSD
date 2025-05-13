#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

class CommandBuffer{
public:
	CommandBuffer();
	~CommandBuffer() {}

	bool IsFull();
	std::vector<std::string> Flush();
	std::string FastRead();

private:
	void InitBuffers();
	std::vector<std::string> ReadBuffers();
	void AppendCommand(std::string);
	void ApplyIgnoreStrategy();
	void ApplyMergeStrategy();

	bool IsEmptyBuffer(std::string bufferName);
	std::string GetBufferContent(std::string bufferName);


private:
	std::vector<std::string> buffers;

	inline static constexpr int MAX_BUFFER_SIZE = 5;
	inline static const std::string BUFFER_DIR_PATH{ "buffer" };
	inline static const std::string EMPTY_BUFFER_NAME{ "empty" };


};

