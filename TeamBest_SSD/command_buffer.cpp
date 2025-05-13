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
	// buffer 폴더 안에 있는 파일들의 개수를 셈
	int fileCount = 0;
	for (const auto& entry : std::filesystem::directory_iterator("buffer")) {
		if (std::filesystem::is_regular_file(entry)) {
			fileCount++;
		}
	}
	std::cout << "fileCount: " << fileCount << std::endl;
	for (int i = fileCount+1; i <= 5; ++i) {
		std::string fileName = "buffer/" + std::to_string(i) + "_empty";
		std::cout << "fileName: " << fileName << std::endl;
		std::ofstream outFile(fileName);
		if (outFile) {
			//outFile << "This is example file " << i << std::endl;
			outFile.close();
			std::cout << fileName << " 파일을 생성했습니다.\n";
		}
		else {
			std::cout << fileName << " 파일을 생성하는 데 실패했습니다.\n";
		}
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