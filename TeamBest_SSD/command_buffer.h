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
	

private:
	void InitBuffers();
	std::vector<std::string> ApplyIgnoreStrategy(const std::string& command);
	std::vector<std::string> ApplyMergeStrategy(std::vector<std::string>& buffer, const std::string& command);

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
	std::vector<std::string> SplitValuesFromCommand(const std::string& command);

	bool IsWriteAtSameLBA(int lba, std::string bufCmd, int bufLba);
	bool IsEraseAtSameLBAAndSize1(int lba, std::string bufCmd, int bufLba, std::string bufArg1);
	bool CanBeRemovedWhenWrite(int lba, std::string bufCmd, int bufLba, std::string bufArg1);
	
	bool IsWriteAtLBAIncluded(int lba, std::string arg1, std::string bufCmd, int bufLba, std::string bufArg1);
	bool IsEraseFromIncludedRange(int lba, std::string arg1, std::string bufCmd, int bufLba, std::string bufArg1);
	bool CanBeRemovedWhenErase(int lba, std::string arg1, std::string bufCmd, int bufLba, std::string bufArg1);

	bool ContainsRange(int lba, std::string arg1, std::string bufCmd, int bufLba, std::string bufArg1);

	std::vector<std::string>::reverse_iterator RemoveFromBack(std::vector<std::string>& buffer, std::vector<std::string>::reverse_iterator it);

	void ClearBuffer();
	std::pair<int, int> getUnifiedLBAAndSize(int lba, std::string arg1, int bufLba, std::string bufArg1);
	inline std::string MakeCommand(std::string cmd, int lba, std::string arg1);
	bool CanApplyMerge(const std::vector<std::string>& buffer, const std::string& command);
	void WriteAllBufferToFiles(const std::vector<std::string>& buffer);


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

inline bool CommandBuffer::IsWriteAtSameLBA(int lba, std::string bufCmd, int bufLba) {
	return bufCmd == "W" && bufLba == lba;
}

inline bool CommandBuffer::IsEraseAtSameLBAAndSize1(int lba, std::string bufCmd, int bufLba, std::string bufArg1) {
	return bufCmd == "E" && bufLba == lba && bufArg1 == "1";
}

inline bool CommandBuffer::CanBeRemovedWhenWrite(int lba, std::string bufCmd, int bufLba, std::string bufArg1) {
	return IsWriteAtSameLBA(lba, bufCmd, bufLba)
		|| IsEraseAtSameLBAAndSize1(lba, bufCmd, bufLba, bufArg1);
}

inline bool CommandBuffer::IsWriteAtLBAIncluded(int lba, std::string arg1, std::string bufCmd, int bufLba, std::string bufArg1) {
	return bufCmd == "W" && bufLba >= lba && bufLba <= lba + std::stoi(arg1) - 1;
}

inline bool CommandBuffer::IsEraseFromIncludedRange(int lba, std::string arg1, std::string bufCmd, int bufLba, std::string bufArg1) {
	return bufCmd == "E" && bufLba >= lba && bufLba + std::stoi(bufArg1) - 1 <= lba + std::stoi(arg1) - 1;
}

inline bool CommandBuffer::CanBeRemovedWhenErase(int lba, std::string arg1, std::string bufCmd, int bufLba, std::string bufArg1) {
	return IsWriteAtLBAIncluded(lba, arg1, bufCmd, bufLba, bufArg1)
		|| IsEraseFromIncludedRange(lba, arg1, bufCmd, bufLba, bufArg1);
}

inline bool CommandBuffer::ContainsRange(int lba, std::string arg1, std::string bufCmd, int bufLba, std::string bufArg1) {
	return bufCmd == "E" &&	lba >= bufLba && lba + std::stoi(arg1) - 1 <= bufLba + std::stoi(bufArg1) - 1;
}

inline std::vector<std::string>::reverse_iterator CommandBuffer::RemoveFromBack(std::vector<std::string>& buffer, std::vector<std::string>::reverse_iterator it) {
	return decltype(it)(buffer.erase((it + 1).base()));;
}

inline std::string CommandBuffer::MakeCommand(std::string cmd, int lba, std::string arg1){
	return cmd + " " + std::to_string(lba) + " " + arg1;
}

inline bool CommandBuffer::CanApplyMerge(const std::vector<std::string>& buffer, const std::string& command) {
	return buffer.back() == command && buffer.size() > 1;
}