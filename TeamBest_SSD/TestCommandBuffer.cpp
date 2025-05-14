#include "gmock/gmock.h"
#include "command_buffer.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>


using namespace testing;
namespace fs = std::filesystem;


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

void RemoveDirectoryAndRecreate( const std::string& dirPath ) {
	if (std::filesystem::exists(dirPath))
		std::filesystem::remove_all(dirPath);

	if (!fs::exists(dirPath)) {
		fs::create_directory(dirPath);
	}
}

void MakeBufferFiles(
	const std::vector<std::string>& bufferNames, 
	const std::string & bufferDirPath) {
	for (const auto& bufferName : bufferNames) {
		std::ofstream outFile(bufferDirPath + "/" + bufferName);
		outFile.close();
	}
}


TEST(TestCommandBuffer, ContructorTest) {
	EXPECT_NO_THROW(std::shared_ptr<CommandBuffer> buffer 
		= std::make_shared<CommandBuffer>());
}


TEST(TestCommandBuffer, BufferExistsAfterInit){
	const std::string dirName = "buffer";
	if (std::filesystem::exists(dirName))
		std::filesystem::remove_all(dirName);
	CommandBuffer buffer;
	EXPECT_TRUE(std::filesystem::exists(dirName));
}

TEST(TestCommandBuffer, FilesExistAfterInit) {
	const std::string dirName = "buffer";
	CommandBuffer buffer;
	int cnt = countFilesInBuffer();
	EXPECT_EQ(cnt, 5);
}

TEST(TestCommandBuffer, TestAppendCommand) {
	std::string BUFFER_DIR = "buffer";

	RemoveDirectoryAndRecreate(BUFFER_DIR);

	std::vector<std::string> bufferNames = {
		{"1_ABCDEF"},
		{"4_empty"},
		{"3_empty"},
		{"5_empty"},
		{"2_FEDCBA"}
	};
	
	MakeBufferFiles(bufferNames, BUFFER_DIR);	

	CommandBuffer buffers;
	std::string newCommand{"W 1 0xABCDEF89"};
	std::string newBufferName = "3_" + newCommand;
	buffers.AppendCommand(newCommand);

	std::vector<std::string> files = GetFilesInDirectory(BUFFER_DIR);
	bool addCommandCheck = (std::find(files.begin(), files.end(), newBufferName) != files.end());
	EXPECT_EQ(true, addCommandCheck);
}


TEST(TestCommandBuffer, TestRead5Files) {
	CommandBuffer buffer;
	std::string BUFFER_DIR = "buffer";

	if (std::filesystem::exists(BUFFER_DIR))
		std::filesystem::remove_all(BUFFER_DIR);

	if (!fs::exists(BUFFER_DIR)) {
		fs::create_directory(BUFFER_DIR);
	}
	std::vector<std::string> bufferNames = {
		{"1_W 0 0x12345678"},
		{"2_E 3 4"},
		{"3_E 20 1"},
		{"4_E 30 1"},
		{"5_E 40 1"}
	};
	for (const auto& bufferName : bufferNames) {
		std::ofstream outFile(BUFFER_DIR + "/" + bufferName);
		outFile.close();
	}
	std::vector<std::string> files = buffer.ReadBuffers();
	std::vector<std::string> expectedFiles = { 
		"W 0 0x12345678", 
		"E 3 4",
		"E 20 1",
		"E 30 1",
		"E 40 1"
	};
	EXPECT_EQ(files, expectedFiles);
}

TEST(TestCommandBuffer, TestRead2Files) {
	CommandBuffer buffer;
	std::string BUFFER_DIR = "buffer";

	if (std::filesystem::exists(BUFFER_DIR))
		std::filesystem::remove_all(BUFFER_DIR);

	if (!fs::exists(BUFFER_DIR)) {
		fs::create_directory(BUFFER_DIR);
	}
	std::vector<std::string> bufferNames = {
		{"1_W 0 0x12345678"},
		{"2_E 3 4"},
		{"3_empty"},
		{"4_empty"},
		{"5_empty"}
	};
	for (const auto& bufferName : bufferNames) {
		std::ofstream outFile(BUFFER_DIR + "/" + bufferName);
		outFile.close();
	}
	std::vector<std::string> files = buffer.ReadBuffers();
	std::vector<std::string> expectedFiles = { 
		"W 0 0x12345678",
		"E 3 4" };
	EXPECT_EQ(files, expectedFiles);
}

TEST(TestCommandBuffer, TestReadNoFiles) {
	CommandBuffer buffer;
	std::string BUFFER_DIR = "buffer";

	if (std::filesystem::exists(BUFFER_DIR))
		std::filesystem::remove_all(BUFFER_DIR);

	if (!fs::exists(BUFFER_DIR)) {
		fs::create_directory(BUFFER_DIR);
	}
	std::vector<std::string> bufferNames = {
		{"1_empty"},
		{"2_empty"},
		{"3_empty"},
		{"4_empty"},
		{"5_empty"}
	};
	for (const auto& bufferName : bufferNames) {
		std::ofstream outFile(BUFFER_DIR + "/" + bufferName);
		outFile.close();
	}
	std::vector<std::string> files = buffer.ReadBuffers();
	std::vector<std::string> expectedFiles = { };
	EXPECT_EQ(files, expectedFiles);
}

TEST(TestCommandBuffer, TestBufferIsNotFull) {
	CommandBuffer buffer;
	std::string BUFFER_DIR = "buffer";

	if (std::filesystem::exists(BUFFER_DIR))
		std::filesystem::remove_all(BUFFER_DIR);

	if (!fs::exists(BUFFER_DIR)) {
		fs::create_directory(BUFFER_DIR);
	}
	std::vector<std::string> bufferNames = {
		{"1_W 0 0x12345678"},
		{"2_E 3 4"},
		{"3_empty"},
		{"4_empty"},
		{"5_empty"}
	};
	for (const auto& bufferName : bufferNames) {
		std::ofstream outFile(BUFFER_DIR + "/" + bufferName);
		outFile.close();
	}
	std::vector<std::string> files = buffer.ReadBuffers();
	bool rst = buffer.IsFull();
	EXPECT_EQ(false, rst);
}

TEST(TestCommandBuffer, TestBufferIsFull) {
	CommandBuffer buffer;
	std::string BUFFER_DIR = "buffer";

	if (std::filesystem::exists(BUFFER_DIR))
		std::filesystem::remove_all(BUFFER_DIR);

	if (!fs::exists(BUFFER_DIR)) {
		fs::create_directory(BUFFER_DIR);
	}
	std::vector<std::string> bufferNames = {
		{"1_W 0 0x12345678"},
		{"2_E 3 4"},
		{"3_E 20 1"},
		{"4_E 30 1"},
		{"5_E 40 1"}
	};
	for (const auto& bufferName : bufferNames) {
		std::ofstream outFile(BUFFER_DIR + "/" + bufferName);
		outFile.close();
	}
	std::vector<std::string> files = buffer.ReadBuffers();
	bool rst = buffer.IsFull();
	EXPECT_EQ(true, rst);
}

TEST(TestCommandBuffer, TestFastReadWhenBufferNotAvailable) {
	std::string BUFFER_DIR = "buffer";

	RemoveDirectoryAndRecreate(BUFFER_DIR);

	std::vector<std::string> bufferNames = {
		{"1_E 3 4"},
		{"2_W 72 0x12345678" },
		{"3_empty"},
		{"4_empty"},
		{"5_empty"}
	};
	MakeBufferFiles(bufferNames, BUFFER_DIR);

	CommandBuffer buffers;
	int targetAddress = 71;
	std::string value = buffers.FastRead(targetAddress);

	EXPECT_EQ("", value);
}

TEST(TestCommandBuffer, TestFastReadWhenBufferAvailable) {
	std::string BUFFER_DIR = "buffer";

	RemoveDirectoryAndRecreate(BUFFER_DIR);

	std::vector<std::string> bufferNames = {
		{"1_E 3 4"},
		{"2_W 72 0x12345678" },
		{"3_empty"},
		{"4_empty"},
		{"5_empty"}
	};
	MakeBufferFiles(bufferNames, BUFFER_DIR);

	CommandBuffer buffers;
	int targetAddress = 72;
	std::string value = buffers.FastRead(targetAddress);

	std::string expected{ "0x12345678" };
	EXPECT_EQ(expected, value);
}