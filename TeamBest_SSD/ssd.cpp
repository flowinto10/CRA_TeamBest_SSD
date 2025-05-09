#include "ssd.h"

void SSD::Initialize(const std::string& fileName) {
    std::ofstream outFile(fileName);
    if (outFile.is_open()) {
        outFile << "example content\n";
        outFile.close();
    }
}

void SSD::Read(int lba) {

}

void SSD::Write(int lba, const std::string& value) {

}