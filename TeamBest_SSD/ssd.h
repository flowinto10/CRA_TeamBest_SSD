#pragma once
#include <string>
#include <filesystem>
#include <fstream>

class SSD{
public:
	SSD() = default;

	void Initialize(const std::string& fileName);
	void Read(int lba);
	void Write(int lba, const std::string& value);

private:
};

