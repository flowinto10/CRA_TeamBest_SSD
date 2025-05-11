#pragma once
#include <string>
#include <filesystem>
#include <fstream>

class ISSD {
public:
	virtual ~ISSD() {};

	virtual void Read(int lba) = 0;
	virtual void Write(int lba, const std::string& value) = 0;

};

class SSD : public ISSD{
public:
	SSD() = default;

	void Initialize(const std::string& fileName);
	void Read(int lba) override;
	void Write(int lba, const std::string& value) override;

private:
};

