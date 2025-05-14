#include "gmock/gmock.h"
#include "TestCommons.h"
#include "command_buffer.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>


using namespace testing;
namespace fs = std::filesystem;

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

TEST(TestCommandBuffer, TestFlushWhenBufferEmpty) {
	std::string BUFFER_DIR = "buffer";

	RemoveDirectoryAndRecreate(BUFFER_DIR);

	CommandBuffer buffers;	
	std::vector<std::string> commandBuffers = buffers.Flush();

	std::vector<std::string> expected{};
	EXPECT_EQ(expected, commandBuffers);
}

TEST(TestCommandBuffer, TestFlushWhenBufferNotEmpty) {
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
	std::vector<std::string> commandBuffers = buffers.Flush();

	std::vector<std::string> expected{
		{"E 3 4"},
		{"W 72 0x12345678" }
	};
	EXPECT_EQ(expected, commandBuffers);
}

TEST(TestCommandBuffer, TesIgnoreWhenWriteAtSameLBA) {
	std::string BUFFER_DIR = "buffer";
	RemoveDirectoryAndRecreate(BUFFER_DIR);
	std::vector<std::string> bufferNames = {
		{"1_E 3 4"},
		{"2_W 72 0x12345678" },
		{"3_W 0 0x12345678"},
		{"4_empty"},
		{"5_empty"}
	};
	MakeBufferFiles(bufferNames, BUFFER_DIR);

	CommandBuffer buffer;
	const std::string& command = "W 72 0xAAAAAAAA";
	std::vector<std::string> cmds = buffer.ApplyIgnoreStrategy(command);

	std::vector<std::string> expectedCmds = {
		"E 3 4",
		"W 0 0x12345678",
		"W 72 0xAAAAAAAA"
	};
	EXPECT_EQ(cmds, expectedCmds);
}

TEST(TestCommandBuffer, TesIgnoreWhenEraseAtSameLBAAndSize1) {
	std::string BUFFER_DIR = "buffer";
	RemoveDirectoryAndRecreate(BUFFER_DIR);
	std::vector<std::string> bufferNames = {
		{"1_E 3 4"},
		{"2_E 72 1" },
		{"3_W 0 0x12345678"},
		{"4_empty"},
		{"5_empty"}
	};
	MakeBufferFiles(bufferNames, BUFFER_DIR);

	CommandBuffer buffer;
	const std::string& command = "W 72 0xAAAAAAAA";
	std::vector<std::string> cmds = buffer.ApplyIgnoreStrategy(command);

	std::vector<std::string> expectedCmds = {
		"E 3 4",
		"W 0 0x12345678",
		"W 72 0xAAAAAAAA"
	};
	EXPECT_EQ(cmds, expectedCmds);
}

TEST(TestCommandBuffer, TestIgnorehenWriteAtLBAIncluded) {
	std::string BUFFER_DIR = "buffer";
	RemoveDirectoryAndRecreate(BUFFER_DIR);
	std::vector<std::string> bufferNames = {
		{"1_E 3 4"},
		{"2_W 74 0xAAAAAAAA" },
		{"3_W 0 0x12345678"},
		{"4_empty"},
		{"5_empty"}
	};
	MakeBufferFiles(bufferNames, BUFFER_DIR);

	CommandBuffer buffer;
	const std::string& command = "E 72 5";
	std::vector<std::string> cmds = buffer.ApplyIgnoreStrategy(command);

	std::vector<std::string> expectedCmds = {
		"E 3 4",
		"W 0 0x12345678",
		"E 72 5"
	};
	EXPECT_EQ(cmds, expectedCmds);
}

TEST(TestCommandBuffer, TestIgnoreWhenEraseFromIncludedRange) {
	std::string BUFFER_DIR = "buffer";
	RemoveDirectoryAndRecreate(BUFFER_DIR);
	std::vector<std::string> bufferNames = {
		{"1_E 3 4"},
		{"2_E 74 3" },
		{"3_W 0 0x12345678"},
		{"4_empty"},
		{"5_empty"}
	};
	MakeBufferFiles(bufferNames, BUFFER_DIR);

	CommandBuffer buffer;
	const std::string& command = "E 72 5";
	std::vector<std::string> cmds = buffer.ApplyIgnoreStrategy(command);

	std::vector<std::string> expectedCmds = {
		"E 3 4",
		"W 0 0x12345678",
		"E 72 5"
	};
	EXPECT_EQ(cmds, expectedCmds);
}

TEST(TestCommandBuffer, TestEraseCommandIsIgnoredWhenRangeIsIncluded) {
	std::string BUFFER_DIR = "buffer";
	RemoveDirectoryAndRecreate(BUFFER_DIR);
	std::vector<std::string> bufferNames = {
		{"1_E 3 4"},
		{"2_E 71 6" },
		{"3_W 0 0x12345678"},
		{"4_empty"},
		{"5_empty"}
	};
	MakeBufferFiles(bufferNames, BUFFER_DIR);

	CommandBuffer buffer;
	const std::string& command = "E 72 5";
	std::vector<std::string> cmds = buffer.ApplyIgnoreStrategy(command);

	std::vector<std::string> expectedCmds = {
		"E 3 4",
		"E 71 6",
		"W 0 0x12345678"		
	};
	EXPECT_EQ(cmds, expectedCmds);
}

TEST(TestCommandBuffer, TestEraseCommandIsIgnoredWhenRangeIsIncluded2) {
	std::string BUFFER_DIR = "buffer";
	RemoveDirectoryAndRecreate(BUFFER_DIR);
	std::vector<std::string> bufferNames = {
		{"1_E 3 4"},
		{"2_E 71 6" },
		{"3_W 73 0x12345678"},
		{"4_empty"},
		{"5_empty"}
	};
	MakeBufferFiles(bufferNames, BUFFER_DIR);

	CommandBuffer buffer;
	const std::string& command = "E 72 5";
	std::vector<std::string> cmds = buffer.ApplyIgnoreStrategy(command);

	std::vector<std::string> expectedCmds = {
		"E 3 4",
		"E 71 6"
	};
	EXPECT_EQ(cmds, expectedCmds);
}