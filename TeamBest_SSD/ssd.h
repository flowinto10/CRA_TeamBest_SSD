#pragma once
#include <string>

class SSD{
public:
	SSD() = default;

	void Read(int lba);
	void Write(int lba, const std::string& value);

private:
};

