#include "command_buffer.h"
#include "util.h"

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
	std::vector<std::string> commandBuffers = ReadBuffers();
	RemoveBufferDirectory();
	InitBuffers();

	return commandBuffers;
}

std::string CommandBuffer::FastRead(int targetAddress) {
	std::vector<std::string> commandsInBuffer = ReadBuffers();

	for (const auto& command : commandsInBuffer) {
		auto tokens = BEST_UTILS::StringTokenizer(command);
		std::string command = tokens[COMMAND_PARAM_INDEX_WRITE::COMMAND_NAME];
		int addressInCommand = std::stoi(tokens[COMMAND_PARAM_INDEX_WRITE::ADDRESS]);
		if (command == WRITE_COMMAND_NAME && targetAddress == addressInCommand) {
			return tokens[COMMAND_PARAM_INDEX_WRITE::VALUE];
		}	
	}

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

	std::vector<std::string> buffer = ApplyIgnoreStrategy(command);
	if (buffer.back() == command && buffer.size() > 1) {
		buffer = ApplyMergeStrategy(buffer, command);
	}
	ClearBuffer();
	for (auto cmd : buffer) {
		std::string emptyBuffer = GetFirstEmptyBuffer();
		WriteCommandToBuffer(emptyBuffer, cmd);
	}
}

std::vector<std::string> CommandBuffer::SplitValuesFromCommand(const std::string& command) {
	// E와 W만 가능
	std::istringstream iss(command);
	std::string cmd, lba, arg1;
	if (!(iss >> cmd >> lba >> arg1))
		throw std::invalid_argument("Command format is incorrect or missing parameters.");
	std::vector<std::string> values;
	values.push_back(cmd);
	values.push_back(lba);
	values.push_back(arg1);
	return values;
}

std::vector<std::string> CommandBuffer::ApplyIgnoreStrategy(const std::string& command) {
	std::vector<std::string> buffer = ReadBuffers();
	std::vector<std::string> values = SplitValuesFromCommand(command);
	std::string cmd= values[0], arg1= values[2];
	int lba = std::stoi(values[1]);

	for (auto it = buffer.rbegin(); it != buffer.rend(); ) {
		std::vector<std::string> values = SplitValuesFromCommand(*it);
		std::string bufCmd = values[0], bufArg1 = values[2];
		int bufLba = std::stoi(values[1]);

		if (cmd == "W" && CanBeRemovedWhenWrite(lba, bufCmd, bufLba, bufArg1)
			||	cmd == "E" && CanBeRemovedWhenErase(lba, arg1, bufCmd, bufLba, bufArg1)) {
			it = RemoveFromBack(buffer, it);
		}
		else if (cmd == "E" && ContainsRange(lba, arg1, bufCmd, bufLba, bufArg1)) return buffer;
		else ++it;
	}
	buffer.push_back(command);
	return buffer;
}

std::vector<std::string> CommandBuffer::ApplyMergeStrategy(std::vector<std::string>& buffer, const std::string& command) {
	std::vector<std::string> values = SplitValuesFromCommand(command);
	std::string cmd = values[0], arg1 = values[2];
	int lba = std::stoi(values[1]);
	if (cmd != "E") return buffer;

	auto it = buffer.rbegin() + 1;
	values = SplitValuesFromCommand(*it);
	std::string bufCmd = values[0], bufArg1 = values[2];
	int bufLba = std::stoi(values[1]);
	if (bufCmd != "E") return buffer;

	auto [unifiedRangeStart, unifiedRangeEnd] = getMinMax(lba, lba + std::stoi(arg1) - 1, bufLba, bufLba + std::stoi(bufArg1) - 1);
	if (unifiedRangeEnd - unifiedRangeStart + 1 > 10) return buffer;
	buffer.pop_back();
	buffer.pop_back();
	buffer.push_back("E "+ std::to_string(unifiedRangeStart)+" "+ std::to_string(unifiedRangeEnd - unifiedRangeStart + 1));
	return buffer;
}

std::pair<int, int> CommandBuffer::getMinMax(int range1Start, int range1End, int range2Start, int range2end) {
	std::vector<int> vec = { range1Start, range1End, range2Start, range2end };
	std::sort(vec.begin(), vec.end());
	return { vec[0], vec[3] };
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

void CommandBuffer::ClearBuffer() {
	if (std::filesystem::exists(BUFFER_DIR_PATH))
		std::filesystem::remove_all(BUFFER_DIR_PATH);
	InitBuffers();
}