#include "command_buffer.h"
#include <string>
#include <vector>

CommandBuffer::CommandBuffer() {
	InitBuffers();
}

bool CommandBuffer::IsFull() {
	return {};
}

std::vector<std::string> CommandBuffer::Flush() {
	return {};
}

std::string CommandBuffer::FastRead() {
	return {};
}

void CommandBuffer::InitBuffers() {
	// buffer 폴더가 없으면 생성
	if (!std::filesystem::exists("buffer")) {
		std::filesystem::create_directory("buffer");
	}
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