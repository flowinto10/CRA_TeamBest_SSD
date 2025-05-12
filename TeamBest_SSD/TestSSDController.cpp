#include "gmock/gmock.h"
#include "ssd_controller.h"
#include "ssd.h"

#include <vector>
#include <string>
#include <memory>

using namespace std;
using namespace testing;

class MockSSD : public ISSD{
public:
	MOCK_METHOD(void, Write, (int, const std::string&), (override));
	MOCK_METHOD(void, Read, (int), (override));
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
		"W 1 2 3 4"
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