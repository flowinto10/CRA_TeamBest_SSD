#include "command_buffer.h"
#include <string>
#include <vector>

CommandBuffer::CommandBuffer() {

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