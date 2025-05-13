#include "command_buffer.h"
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>


CommandBuffer::CommandBuffer() {
	InitBuffers();
}

bool CommandBuffer::IsFull() {
	std::vector<std::string> commands = ReadBuffers();
	return commands.size() == MAX_BUFFER_SIZE;
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
#ifdef _DEBUG
			std::cerr << "파일을 생성하는 데 실패했습니다!" << fileName << '\n';
#endif	
		}
	}
}

void CommandBuffer::InitBuffers() {
	if (!BufferDirectoryExist())	std::filesystem::create_directory(BUFFER_DIR_PATH);
	MakeEmptyFiles();
}

std::string CommandBuffer::removeIndex(const std::string& fileName) {
	std::string command;
	size_t pos = fileName.find(DELIMITER);
	if (pos != std::string::npos) command = fileName.substr(pos + 1);
	else {
#ifdef _DEBUG
		std::cerr << "Invalid fileName: " << fileName << '\n';
#endif	
	}
	return command;
}

std::string CommandBuffer::MakeCommandFromFile(const std::filesystem::directory_entry& file) {
	std::string fileName = "";
	if (std::filesystem::is_regular_file(file)) {
		fileName = file.path().filename().string();
		if (!IsEmptyBuffer(fileName)) {
			fileName = removeIndex(fileName);
			return fileName;
		}
	}
	return "";
}

std::vector<std::string> CommandBuffer::ReadBuffers() {
	std::vector<std::string> commandList;
	std::string command;
	if (BufferDirectoryExist()) {
		for (const auto& file : std::filesystem::directory_iterator(BUFFER_DIR_PATH)) {
			command = MakeCommandFromFile(file);
			if (command == "") continue;
			commandList.push_back(command);
		}
	}
	return commandList;
}

void CommandBuffer::AppendCommand(const std::string& command) {
	if (IsFull()) return;
	std::string emptyBuffer = GetFirstEmptyBuffer();

#ifdef _DEBUG
	std::cout << "Empty Buffer : " << emptyBuffer << std::endl;
#endif

	WriteCommandToBuffer(emptyBuffer, command);
}

void CommandBuffer::ApplyIgnoreStrategy() {

}

void CommandBuffer::ApplyMergeStrategy() {

}

std::string CommandBuffer::GetBufferContent(std::string bufferName) {
	return {};
}

std::string CommandBuffer::GetFirstEmptyBuffer() {
	std::string emptyBuffer{ "" };

	if (!BufferDirectoryExist()) {
#ifdef _DEBUG
		std::cout << "Buffer Directory is Not Exist\n";
#endif
		return emptyBuffer;
	}

	namespace fs = std::filesystem;
	try {		
		for (const auto& entry : fs::directory_iterator(BUFFER_DIR_PATH)) {
			if (fs::is_regular_file(entry.status())) {
				std::string fileName = entry.path().filename().string();
				if (IsEmptyBuffer(fileName)) return fileName;
			}
		}		
	}
	catch (const fs::filesystem_error& e) {
#ifdef _DEBUG
		std::cerr << "파일 목록 검사 중 오류 발생: " << e.what() << '\n';
#endif
	}

	return emptyBuffer;
}

void CommandBuffer::WriteCommandToBuffer(
	const std::string& emptyBuffer,
	const std::string& command) {

	std::string oldBufferPath = GetBufferFullPath(emptyBuffer);
	std::string newBufferName = GetBufferIndexAtBufferName(emptyBuffer) + command;
	std::string newBufferPath = GetBufferFullPath(newBufferName);

	namespace fs = std::filesystem;
	try {
		fs::rename(oldBufferPath, newBufferPath);
	}
	catch (const fs::filesystem_error& e) {
#ifdef _DEBUG
		std::cerr << "Buffer 이름 변경 실패: " << e.what() << '\n';
#endif
	}
}
