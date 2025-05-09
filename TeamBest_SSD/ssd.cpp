#include "ssd.h"

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

}