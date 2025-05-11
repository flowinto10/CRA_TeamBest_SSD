#include "gmock/gmock.h"
#include "ssd_controller.h"

#include <vector>
#include <string>

using namespace std;
using namespace testing;

class MockSSD {
public:
	MOCK_METHOD(void, Write, (int lba, string data), ());
	MOCK_METHOD(void, Read, (int lba), ());
};

TEST(TestSSDController, ContructorTest) {
	EXPECT_NO_THROW(std::shared_ptr<SSDController> ssd 
		= std::make_shared<SSDController>());
}

TEST(TestSSDController, NonExistCommand) {
	vector<string> commands = {
		"sSd r 1",
		"ss w 1",
		"ssdr w 1",
		"ssd r 1",
		"ssd w 1",
		"ssd RR 1",
		"ssd A 1",
		"ssd ! 1"
	};

	SSDController ssdController;
	for (const auto& command : commands) {
		bool result = ssdController.Run(command);
		EXPECT_EQ(false, result);
	}
}

TEST(TestSSDController, InvalidParametersCount) {
	vector<string> commands = {
		"ssd R",
		"ssd R 1 2 3",
		"ssd W",
		"ssd W 1 2 3 4"
	};

	SSDController ssdController;
	for (const auto& command : commands) {
		bool result = ssdController.Run(command);
		EXPECT_EQ(false, result);
	}
}

TEST(TestSSDController, ValidReadCommand) {
	MockSSD mockSSD;
	SSDController ssdController;

	string command = { "ssd R 1" };
	EXPECT_CALL(mockSSD, Read(1)).Times(1);
	
	bool result = ssdController.Run(command);
	EXPECT_EQ(true, result);
}

TEST(TestSSDController, ValidWriteCommand) {
	MockSSD mockSSD;
	SSDController ssdController;

	string command = { "ssd W 1 0xFFFFFF" };
	EXPECT_CALL(mockSSD, Write(1, "0xFFFFFF")).Times(1);

	bool result = ssdController.Run(command);
	EXPECT_EQ(true, result);
}