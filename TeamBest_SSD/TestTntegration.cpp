#include "gmock/gmock.h"
#include "TestCommons.h"

#include "ssd.h"
#include "command_buffer.h"
#include "ssd_controller.h"

using namespace std;
namespace fs = std::filesystem;


TEST(TestIntegration, FlushTestWhenWriteBufferExist) {
	const std::string BUFFER_DIR = "buffer";
	const std::string SSD_NAND_FILENAME = "ssd_nand.txt";
	const std::string SSD_OUTPUT_FILENAME = "ssd_output.txt";

	RemoveFile(SSD_NAND_FILENAME);
	std::shared_ptr<ISSD> ssd = std::make_shared<SSD>();
	SSDController ssdController{ ssd };

	vector<string> writeParams = {
		{"1 0x12345678" },
		{"2 0xFAFAFAFA" },
		{"3 0x1A2B3C3D" },
		{"4 0x98765432" },
		{"5 0x01020304" }
	};
	
	std::vector<std::string> bufferNames = {
		{"1_W 1 0x12345678" },
		{"2_W 2 0xFAFAFAFA" },
		{"3_W 3 0x1A2B3C3D" },
		{"4_W 4 0x98765432" },
		{"5_W 5 0x01020304" }
	};
	
	for (const auto& expected : writeParams) {
		EXPECT_EQ(false, ContainsStringInFile(SSD_NAND_FILENAME, expected));
	}	

	RemoveDirectoryAndRecreate(BUFFER_DIR);
	MakeBufferFiles(bufferNames, BUFFER_DIR);
		
	ssdController.Run("F");
	for (const auto& expected : writeParams) {
		EXPECT_EQ(true, ContainsStringInFile(SSD_NAND_FILENAME, expected));
	}
}


TEST(TestIntegration, FlushTestWhenEraseBufferExist) {
	const std::string BUFFER_DIR = "buffer";
	const std::string SSD_NAND_FILENAME = "ssd_nand.txt";
	const std::string SSD_OUTPUT_FILENAME = "ssd_output.txt";

	RemoveFile(SSD_NAND_FILENAME);
	std::shared_ptr<ISSD> ssd = std::make_shared<SSD>();
	SSDController ssdController{ ssd };

	vector<string> writeParams = {
		{"1 0x12345678" },
		{"2 0xFAFAFAFA" },
		{"3 0x1A2B3C3D" },
		{"4 0x98765432" },
		{"5 0x01020304" }
	};

	std::vector<std::string> wirteBufferNames = {
		{"1_W 1 0x12345678" },
		{"2_W 2 0xFAFAFAFA" },
		{"3_W 3 0x1A2B3C3D" },
		{"4_W 4 0x98765432" },
		{"5_W 5 0x01020304" }
	};

	RemoveDirectoryAndRecreate(BUFFER_DIR);
	MakeBufferFiles(wirteBufferNames, BUFFER_DIR);
	
	ssdController.Run("F");
	for (const auto& expected : writeParams) {
		EXPECT_EQ(true, ContainsStringInFile(SSD_NAND_FILENAME, expected));
	}

	std::vector<std::string> eraseBufferNames = {
		{"1_E 1 1" },
		{"2_W 2 0xFAFAFAFA" },
		{"3_E 3 1" },
		{"4_W 4 0x98765432" },
		{"5_E 5 1" }
	};
	RemoveDirectoryAndRecreate(BUFFER_DIR);
	MakeBufferFiles(eraseBufferNames, BUFFER_DIR);
	
	ssdController.Run("F");
	for (int i = 0; i < writeParams.size(); ++i) {
		bool answer = false;
		if(i%2 == 1) answer = true;
		EXPECT_EQ(answer, ContainsStringInFile(SSD_NAND_FILENAME, writeParams[i]));
	}
}