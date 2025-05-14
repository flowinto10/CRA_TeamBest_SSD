#include "gmock/gmock.h"
#include "TestCommons.h"
#include "ssd_controller.h"
#include "ssd.h"
#include "command_buffer.h"

#include <vector>
#include <string>
#include <memory>

using namespace std;
using namespace testing;
namespace fs = std::filesystem;

class MockSSD : public ISSD{
public:
	MOCK_METHOD(void, Write, (int, const std::string&), (override));
	MOCK_METHOD(void, Read, (int), (override));
	MOCK_METHOD(void, Erase, (int, int), (override));
	MOCK_METHOD(void, WriteValueToOutputFile, (const std::string&), (override));
};

TEST(TestSSDController, ContructorTest) {
	std::shared_ptr<MockSSD> mockSSD = std::make_shared<MockSSD>();
	EXPECT_NO_THROW(std::shared_ptr<SSDController> ssd 
		= std::make_shared<SSDController>(mockSSD));
}

TEST(TestSSDController, NonExistCommand) {
	vector<string> commands = {
		"RR 1",
		"A 1",
		"! 1"
	};

	std::shared_ptr<MockSSD> mockSSD = std::make_shared<MockSSD>();
	SSDController ssdController{ mockSSD };
	for (const auto& command : commands) {
		bool result = ssdController.Run(command);
		EXPECT_EQ(false, result);
	}
}

TEST(TestSSDController, InvalidParametersCount) {
	vector<string> commands = {
		"R",
		"R 1 2 3",
		"W",
		"W 1 2 3 4",
		"E",
		"E 1 2 3 4",
		"F 1 2"
	};

	std::shared_ptr<MockSSD> mockSSD = std::make_shared<MockSSD>();
	SSDController ssdController{ mockSSD };
	for (const auto& command : commands) {
		bool result = ssdController.Run(command);
		EXPECT_EQ(false, result);
	}
}

TEST(TestSSDController, ValidReadCommand) {
	std::shared_ptr<MockSSD> mockSSD = std::make_shared<MockSSD>();
	SSDController ssdController{ mockSSD };
	
	vector<string> commands = { 
		"R 1",
		"r 1"
	};

	for (const auto& command : commands) {
		EXPECT_CALL(*mockSSD, Read(1)).Times(1);

		bool result = ssdController.Run(command);
		EXPECT_EQ(true, result);
	}	
}

TEST(TestSSDController, ValidWriteCommand) {
	const std::string BUFFER_DIR = "buffer";
	RemoveDirectoryAndRecreate(BUFFER_DIR);

	std::shared_ptr<ISSD> ssd = std::make_shared<SSD>();
	SSDController ssdController{ ssd };

	vector<pair<string, string>> commandsAndExpected = {
		{"W 1 0x1234ABCD", "1_W 1 0x1234ABCD"}
	};	

	for (const auto& [command, expectdBufferName]: commandsAndExpected) {
		bool result = ssdController.Run(command);

		std::vector<std::string> files = GetFilesInDirectory(BUFFER_DIR);
		bool addCommandCheck = (std::find(files.begin(), files.end(), 
			expectdBufferName) != files.end());
		EXPECT_EQ(true, addCommandCheck);
	}
}

TEST(TestSSDController, ValidEraseCommand) {
	const std::string BUFFER_DIR = "buffer";
	RemoveDirectoryAndRecreate(BUFFER_DIR);

	std::shared_ptr<ISSD> ssd = std::make_shared<SSD>();
	SSDController ssdController{ ssd };

	vector<pair<string, string>> commandsAndExpected = {
		{"E 1 5", "1_E 1 5"}
	};

	for (const auto& [command, expectdBufferName] : commandsAndExpected) {
		bool result = ssdController.Run(command);

		std::vector<std::string> files = GetFilesInDirectory(BUFFER_DIR);
		bool addCommandCheck = (std::find(files.begin(), files.end(),
			expectdBufferName) != files.end());
		EXPECT_EQ(true, addCommandCheck);
	}
}

TEST(TestSSDController, ValidFlushCommand) {
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
		{"3_W 1 0xABCDEF98"},
		{"4_E 30 1"},
		{"5_E 40 1"}
	};
	for (const auto& bufferName : bufferNames) {
		std::ofstream outFile(BUFFER_DIR + "/" + bufferName);
		outFile.close();
	}

	std::shared_ptr<MockSSD> mockSSD = std::make_shared<MockSSD>();	
	SSDController ssdController{ mockSSD };
	string command = {"F"};

	std::vector<std::string> files = GetFilesInDirectory(BUFFER_DIR);
	for (const auto& expectdBufferName : bufferNames) {		
		bool addCommandCheck = (std::find(files.begin(), files.end(),
			expectdBufferName) != files.end());
		EXPECT_EQ(true, addCommandCheck);
	}
}