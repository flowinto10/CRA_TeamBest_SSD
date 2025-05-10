#include "ssd.h"
#include <iostream>
#include <regex>

const int FILESIZE = 99;

void SSD::Initialize(const std::string& fileName) {
    std::ofstream outFile(fileName);
    if (fileName == "ssd_nand.txt") {
        if (outFile.is_open()) {
            for (int i = 0; i <= FILESIZE; ++i) {
                outFile << i << " 0x00000000\n";
            }
            outFile.close();
        }
    }
}

void SSD::Read(int lba) {

}

void SSD::Write(int lba, const std::string& value) {    
    if (!IsValidAddress(lba)) return WriteErrorMessageToOutputFile();
    if (!IsValidValue(value)) return WriteErrorMessageToOutputFile();
    if (!IsNandFileExist()) Initialize(NAND_FILE_PATH);

    UpdateValueAtAddress(lba, value);
}

bool SSD::IsValidAddress(int address) {
    return (address >= addressMin) && (address <= addressMax);
}

bool SSD::IsValidValue(const std::string& value) {
    return std::regex_match(value, VALID_PATTERN);
}

bool SSD::IsNandFileExist() {
    return std::filesystem::exists(NAND_FILE_PATH);
}

void SSD::WriteErrorMessageToOutputFile() {
    std::ofstream outFile(OUTPUT_FILE_PATH, std::ios::trunc);
    if (!outFile) {
        std::cerr << "파일을 열 수 없습니다: " << OUTPUT_FILE_PATH << '\n';
        return;
    }

    outFile << WRITE_ERROR_MESSAGE << '\n';
    outFile.close();
}

void SSD::UpdateValueAtAddress(int lba, const std::string& value) {
    std::vector<std::string> entireData = ReadDataForEntireAddress();
    if (entireData.size() != GetEntireAddressSize()) {
        std::cerr << "메모리 크기와 읽은 데이터의 수가 다릅니다!\n";
        return;
    }

    std::string newDataAsSaveFormat = MakeDataAsSaveFormat(lba, value);
    entireData[lba] = newDataAsSaveFormat;

    WriteDataToEntireAddress(entireData);
}

std::vector<std::string> SSD::ReadDataForEntireAddress() {
    std::vector<std::string> entireData;    

    std::ifstream inFile(NAND_FILE_PATH);
    if (!inFile) {
        std::cerr << "파일 열기 실패: " << NAND_FILE_PATH << '\n';
        return entireData;
    }

    entireData.reserve(GetEntireAddressSize());
    std::string line;
    while (std::getline(inFile, line)) {
        entireData.push_back(line);
    }
    inFile.close();

    return entireData;
}

void SSD::WriteDataToEntireAddress(std::vector<std::string>& values) {
    std::ofstream outFile(NAND_FILE_PATH);
    
    if (outFile.is_open()) {
        for (const auto& value : values) {
            outFile << value << '\n';
        }        
        outFile.close();
    }    
}