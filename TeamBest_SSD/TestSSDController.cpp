#include "gmock/gmock.h"
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
	std::shared_ptr<MockSSD> mockSSD = std::make_shared<MockSSD>();
	SSDController ssdController{ mockSSD };

	vector<string> commands = {
		"W 1 0xFFFFFFFF",
		"w 1 0xFFFFFFFF"
	};

	for (const auto& command : commands) {
		EXPECT_CALL(*mockSSD, Write(1, "0xFFFFFFFF")).Times(1);

		bool result = ssdController.Run(command);
		EXPECT_EQ(true, result);
	}

	string command = { "w 1 0xffffffff" };
	EXPECT_CALL(*mockSSD, Write(1, "0xffffffff")).Times(1);

	bool result = ssdController.Run(command);
	EXPECT_EQ(true, result);
}

TEST(TestSSDController, ValidEraseCommand) {
	std::shared_ptr<MockSSD> mockSSD = std::make_shared<MockSSD>();
	SSDController ssdController{ mockSSD };

	vector<string> commands = {
		"E 1 5",
		"e 1 5"
	};

	EXPECT_CALL(*mockSSD, Erase(1, 5)).Times(commands.size());
	for (const auto& command : commands) {
		bool result = ssdController.Run(command);
		EXPECT_EQ(true, result);
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

	EXPECT_CALL(*mockSSD, Write(_, _)).Times(2);
	EXPECT_CALL(*mockSSD, Erase(_, _)).Times(3);
	ssdController.Run(command);
}