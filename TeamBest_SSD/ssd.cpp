#include "ssd.h"

void SSD::Initialize(const std::string& fileName) {
    std::ofstream outFile(fileName);
    if (outFile.is_open()) {
        for (int i = 0; i <= 99; ++i) {
            outFile << i << " 0x00000000\n";
        }
        outFile.close();
    }
}

void SSD::Read(int lba) {

}

void SSD::Write(int lba, const std::string& value) {

}