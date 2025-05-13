#include "TestCommons.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

namespace fs = std::filesystem;

void ClearFileContent(const std::string& path) {
	std::ofstream file(path, std::ios::trunc);

	if (!file.is_open()) {
		throw std::runtime_error("파일을 열 수 없습니다: " + path);
	}
	file.close();
}

void RemoveFile(const std::string& path) {
	if (fs::exists(path)) {
		if (fs::remove(path)) {
		}
	}
}

std::string ReadFileContent(const std::string& path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		throw std::runtime_error("파일을 열 수 없습니다: " + path);
	}

	std::string line;
	std::getline(file, line);
	file.close();

	return line;
}

bool ContainsStringInFile(const std::string& filePath, const std::string& keyword) {
	std::ifstream file(filePath);
	if (!file.is_open()) {
		throw std::runtime_error("파일을 열 수 없습니다: " + filePath);
	}

	std::string line;
	while (std::getline(file, line)) {
		if (line.find(keyword) != std::string::npos) {
			return true;
		}
	}

	return false;
}

int countFilesInBuffer() {
	int count = 0;
	for (const auto& entry : std::filesystem::directory_iterator("buffer")) {
		if (std::filesystem::is_regular_file(entry)) {
			++count;
		}
	}
	return count;
}

std::vector<std::string> GetFilesInDirectory(const std::string& directoryPath) {
	std::vector<std::string> fileList;

	try {
		if (fs::exists(directoryPath) && fs::is_directory(directoryPath)) {
			for (const auto& entry : fs::directory_iterator(directoryPath)) {
				if (fs::is_regular_file(entry.status())) {
					fileList.push_back(entry.path().filename().string());
				}
			}
		}
	}
	catch (const fs::filesystem_error& e) {
#ifdef _DEBUG
		std::cerr << "파일 목록 검사 중 오류 발생: " << e.what() << '\n';
#endif
	}

	return fileList;

}

void RemoveDirectoryAndRecreate(const std::string& dirPath) {
	if (std::filesystem::exists(dirPath))
		std::filesystem::remove_all(dirPath);

	if (!fs::exists(dirPath)) {
		fs::create_directory(dirPath);
	}
}

void MakeBufferFiles(
	const std::vector<std::string>& bufferNames,
	const std::string& bufferDirPath) {
	for (const auto& bufferName : bufferNames) {
		std::ofstream outFile(bufferDirPath + "/" + bufferName);
		outFile.close();
	}
}