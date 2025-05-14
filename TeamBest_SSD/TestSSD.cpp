#include "gmock/gmock.h"
#include "TestCommons.h"
#include "ssd.h"

#include <memory>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <utility>

using namespace testing;

TEST(TestSSD, ContructorTest) {
	EXPECT_NO_THROW(std::shared_ptr<SSD> ssd = std::make_shared<SSD>());
}

TEST(TestSSD, NandFileExistsAfterInit) {
    const std::string filename = "ssd_nand.txt";

    // 사전 정리: 파일이 있으면 삭제
    if (std::filesystem::exists(filename)) {
        std::filesystem::remove(filename);
    }

    SSD app;

    // 검증: 파일이 생성되었는지 확인
    EXPECT_TRUE(std::filesystem::exists(filename));
}

TEST(TestSSD, OutputFileExistsAfterInit) {
    const std::string filename = "ssd_output.txt";

    // 사전 정리: 파일이 있으면 삭제
    if (std::filesystem::exists(filename)) {
        std::filesystem::remove(filename);
    }

    SSD app;

    // 검증: 파일이 생성되었는지 확인
    EXPECT_TRUE(std::filesystem::exists(filename));
}

TEST(TestSSD, TestGetAddressRange) {
    
    SSD ssd;
    const int ADDRESS_MIN_LIMIT = 0;
    const int ADDRESS_MAX_LIMIT = 99;
    ssd.SetAddressRange(ADDRESS_MIN_LIMIT, ADDRESS_MAX_LIMIT);
    
    std::pair<int, int> addressRange = ssd.GetAddressRange();
    
    EXPECT_EQ(ADDRESS_MIN_LIMIT, addressRange.first);
    EXPECT_EQ(ADDRESS_MAX_LIMIT, addressRange.second);
}

TEST(TestSSD, TestWriteWhenNormalCase) {
    const std::string nandFilePath = "ssd_nand.txt";

    SSD ssd;
    std::pair<int, int> addressRange = ssd.GetAddressRange();

    std::vector<int> lbas = { 
        addressRange.first,
        static_cast<int>((addressRange.first + addressRange.second)*0.5),
        addressRange.second };
    std::string value = {"0xFFFFFFFF" };    
    
    for (int lba : lbas) {
        ssd.Write(lba, value);
        std::string expected = std::to_string(lba) + " " + value;
        EXPECT_EQ(true, ContainsStringInFile(nandFilePath, expected));
    }
}

TEST(TestSSD, TestWriteWhenNandFileNotExist) {
    namespace fs = std::filesystem;
    
    const std::string nandFilePath = "ssd_nand.txt";
    
    if (fs::exists(nandFilePath)) {
        if (fs::remove(nandFilePath)) {
        }        
    }

    SSD ssd;
    int lba{ 1 };
    const std::string value{ "0xFFFFFFFF" };
    ssd.Write(lba, value);

    EXPECT_EQ(true, fs::exists(nandFilePath));
}

TEST(TestSSD, TestWriteWhenInvalidLbaOverMaxLimit) {
    const std::string outputFilePath = "ssd_output.txt";
    ClearFileContent(outputFilePath);

    SSD ssd;
    std::pair<int, int> addressRange = ssd.GetAddressRange();
    const int ADDRESS_MAX_LIMIT = addressRange.second;
    int lba{ ADDRESS_MAX_LIMIT+1 };
    const std::string value{ "0xFFFFFFFF" };
    ssd.Write(lba, value);
      
    std::string line = ReadFileContent( outputFilePath );
    EXPECT_EQ("ERROR", line);
}

TEST(TestSSD, TestWriteWhenInvalidLbaUnderMinLimit) {
    const std::string outputFilePath = "ssd_output.txt";
    ClearFileContent(outputFilePath);

    SSD ssd;
    std::pair<int, int> addressRange = ssd.GetAddressRange();
    const int ADDRESS_MIN_LIMIT = addressRange.first;
    int lba{ ADDRESS_MIN_LIMIT - 1 };
    const std::string value{ "0xFFFFFFFF" };
    ssd.Write(lba, value);

    std::string line = ReadFileContent(outputFilePath);
    EXPECT_EQ("ERROR", line);
}

TEST(TestSSD, TestWriteWhenInvalidValue) {
    const std::string outputFilePath = "ssd_output.txt";
    
    std::vector<std::string> invalidValues = {
        "FFFFFFFFFF",
        "0xFFFFFF",
        "0xFFFFFFFFFF",
        "",
        "0xF$$FFFFF",
        "0xFF FFFFF",
        "0XFFFFFFFF",
        "OxFFFFFFFF",
        "0x000O0000"
    };
    
    SSD ssd;
    int lba{ 0 };
    for (auto& value : invalidValues) {
        ClearFileContent(outputFilePath);
        ssd.Write(lba, value);
        std::string line = ReadFileContent(outputFilePath);
        EXPECT_EQ("ERROR", line);
    }    
}

TEST(TestSSD, TestReadWhenInvalidLBA) {
    SSD ssd;
    const std::string outputFilePath = "ssd_output.txt";
    ClearFileContent(outputFilePath);
    ssd.Read(-1);
    std::string line = ReadFileContent(outputFilePath);
    EXPECT_EQ("ERROR", line);
}

TEST(TestSSD, TestReadWhenValidLBA) {
    SSD ssd;
    const std::string outputFilePath = "ssd_output.txt";
    ClearFileContent(outputFilePath);
    ssd.Read(0);
    std::string line = ReadFileContent(outputFilePath);
    EXPECT_EQ("0x00000000", line);
}

TEST(TestSSD, TestReadWhenValidLBA2) {
    SSD ssd;
    const std::string outputFilePath = "ssd_output.txt";
    ClearFileContent(outputFilePath);
    ssd.Write(1, "0x12345678");
    ssd.Read(1);
    std::string line = ReadFileContent(outputFilePath);
    EXPECT_EQ("0x12345678", line);
}

TEST(TestSSD, TestEraseWhenValidLBAAndValidSize) {
    SSD ssd;
    const std::string outputFilePath = "ssd_output.txt";
    ClearFileContent(outputFilePath);
    int lba = 0, size = 10;
    for (int i = 0; i <= 99; ++i) {
        ssd.Write(i, "0x11111111");
    }
    ssd.Erase(lba, size);
    for (int i = lba; i < lba + size; ++i) {
        ssd.Read(i);
        std::string line = ReadFileContent(outputFilePath);
        EXPECT_EQ("0x00000000", line);
    }

}

TEST(TestSSD, TestEraseWhenInvalidLBAAndValidSize) {
    SSD ssd;
    const std::string outputFilePath = "ssd_output.txt";
    ClearFileContent(outputFilePath);
    int lba = -1, size = 10;
    ssd.Erase(lba, size);
    std::string line = ReadFileContent(outputFilePath);
    EXPECT_EQ("ERROR", line);
}

TEST(TestSSD, TestEraseWhenValidLBAAndInvalidSize) {
    SSD ssd;
    const std::string outputFilePath = "ssd_output.txt";
    ClearFileContent(outputFilePath);
    int lba = 0, size = 11;
    ssd.Erase(lba, size);
    std::string line = ReadFileContent(outputFilePath);
    EXPECT_EQ("ERROR", line);
}

TEST(TestSSD, TestEraseWhenInvalidLBAAndInvalidSize) {
    SSD ssd;
    const std::string outputFilePath = "ssd_output.txt";
    ClearFileContent(outputFilePath);
    int lba = -1, size = 11;
    ssd.Erase(lba, size);
    std::string line = ReadFileContent(outputFilePath);
    EXPECT_EQ("ERROR", line);
}

TEST(TestSSD, TestEraseWhenValidLBAAndValidSizeButFail) {
    SSD ssd;
    const std::string outputFilePath = "ssd_output.txt";
    ClearFileContent(outputFilePath);
    int lba = 91, size = 10;
    ssd.Erase(lba, size);
    std::string line = ReadFileContent(outputFilePath);
    EXPECT_EQ("ERROR", line);
}