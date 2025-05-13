#include "command_buffer.h"
#include <string>
#include <vector>
#include <iostream>

CommandBuffer::CommandBuffer() {
	InitBuffers();
}

bool CommandBuffer::IsFull() {
	return {};
}

bool CommandBuffer::BufferExist() {
	return std::filesystem::exists(BUFFER_DIR_PATH);
}

std::vector<std::string> CommandBuffer::Flush() {
	return {};
}

std::string CommandBuffer::FastRead() {
	return {};
}

int CommandBuffer::CountFilesInBuffer() {
	int fileCount = 0;
	for (const auto& entry : std::filesystem::directory_iterator(BUFFER_DIR_PATH)) {
		if (std::filesystem::is_regular_file(entry)) {
			fileCount++;
		}
	}
	return fileCount;
}

void CommandBuffer::MakeEmptyFiles() {
	int fileCount = CountFilesInBuffer();
	for (int i = fileCount + 1; i <= MAX_BUFFER_SIZE; ++i) {
		std::string fileName = BUFFER_DIR_PATH + "/" + std::to_string(i) + "_" + EMPTY_BUFFER_NAME;
		std::ofstream outFile(fileName);
		if (outFile) outFile.close();
		else {
			std::cerr << "파일을 생성하는 데 실패했습니다!" << fileName << '\n';
		}
	}
}

void CommandBuffer::InitBuffers() {
	if (!BufferExist())	std::filesystem::create_directory(BUFFER_DIR_PATH);
	MakeEmptyFiles();
}

std::vector<std::string> CommandBuffer::ReadBuffers() {
	return {};
}

void CommandBuffer::AppendCommand(std::string) {

}

void CommandBuffer::ApplyIgnoreStrategy() {

}

void CommandBuffer::ApplyMergeStrategy() {

}

bool CommandBuffer::IsEmptyBuffer(std::string bufferName) {
	return {};
}

std::string CommandBuffer::GetBufferContent(std::string bufferName) {
	return {};
}