#include "ssd.h"
#include <iostream>
#include <regex>

const int MAX_LBA = 99;

SSD::SSD() {
    Initialize(NAND_FILE_PATH);
    Initialize(OUTPUT_FILE_PATH);
}

void SSD::Initialize(const std::string& fileName) {
    if (fileName == NAND_FILE_PATH) {
        if (IsNandFileExist()) return;
        std::ofstream outFile(fileName);
        if (outFile.is_open()) {
            for (int i = 0; i <= MAX_LBA; ++i) {
                outFile << i << " 0x00000000\n";
            }
            outFile.close();
        }        
    }
    else {
        if (IsOutputFileExist()) return;
        std::ofstream outFile(fileName);
    }
    
}

void SSD::Read(int lba) {
    if (!IsValidAddress(lba)) return WriteValueToOutputFile(ERROR_MESSAGE);
    return ReadValueAtAddress(lba);
}

void SSD::Write(int lba, const std::string& value) {    
    if (!IsValidAddress(lba)) return WriteValueToOutputFile(ERROR_MESSAGE);
    if (!IsValidValue(value)) return WriteValueToOutputFile(ERROR_MESSAGE);
    if (!IsNandFileExist()) Initialize(NAND_FILE_PATH);

    UpdateValueAtAddress(lba, value);
}

void SSD::Erase(int lba, int size) {
    if(size < 0 || size > 10) WriteValueToOutputFile(ERROR_MESSAGE);
    for (int i = lba; i < lba + size; ++i) {
        Write(i, "0x00000000");
    }
}

bool SSD::IsValidAddress(int address) {
    return (address >= addressMin) && (address <= addressMax);
}

bool SSD::IsValidValue(const std::string& value) {
    return std::regex_match(value, VALID_DATA_PATTERN);
}

bool SSD::IsNandFileExist() {
    return std::filesystem::exists(NAND_FILE_PATH);
}

bool SSD::IsOutputFileExist() {
    return std::filesystem::exists(OUTPUT_FILE_PATH);
}

std::pair<int, std::string> SSD::SplitLineToLbaAndValue(const std::string& line) {
    size_t space_pos = line.find(' ');
    if (space_pos == std::string::npos) {
        throw std::invalid_argument("입력 문자열에 공백이 없습니다.\n");
    }
    std::string readLba = line.substr(0, space_pos);
    std::string readValue = line.substr(space_pos + 1);
    return { std::stoi(readLba), readValue };
}

void  SSD::ReadValueAtAddress(int lba) {
    std::ifstream file(NAND_FILE_PATH);
    if (!file.is_open()) {
        std::cerr << "파일을 열 수 없습니다!" << NAND_FILE_PATH << '\n';
        return WriteValueToOutputFile(ERROR_MESSAGE);
    }

    std::string line, readValue;
    int line_number = 0, readLba;

    while (std::getline(file, line)) {
        if (line_number == lba) {
            std::pair convertedLine = SplitLineToLbaAndValue(line);
            readLba = convertedLine.first;
            readValue = convertedLine.second;
            if (readLba != lba) {
                std::cout << "파일에 해당 주소가 없습니다.: " << lba << "\n";
                return WriteValueToOutputFile(ERROR_MESSAGE);
            }
            break;
        }
        line_number++;
    } 

    file.close();  // 파일 닫기
    return WriteValueToOutputFile(readValue);
}

void SSD::WriteValueToOutputFile(const std::string& value) {
    std::ofstream outFile(OUTPUT_FILE_PATH, std::ios::trunc);
    if (!outFile) {
        std::cerr << "파일을 열 수 없습니다: " << OUTPUT_FILE_PATH << '\n';
        return;
    }

    outFile << value << '\n';
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