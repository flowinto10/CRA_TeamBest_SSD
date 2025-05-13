#pragma once

#include <string>
#include <vector>

void ClearFileContent(const std::string& path);
void RemoveFile(const std::string& path);
std::string ReadFileContent(const std::string& path);
bool ContainsStringInFile(const std::string& filePath, const std::string& keyword);
int countFilesInBuffer();
std::vector<std::string> GetFilesInDirectory(const std::string& directoryPath);
void RemoveDirectoryAndRecreate(const std::string& dirPath);
void MakeBufferFiles(const std::vector<std::string>& bufferNames,
	const std::string& bufferDirPath);