#pragma once
#include <string>
#include <filesystem>
#include <fstream>
#include <utility>
#include <string_view>
#include <regex>

class ISSD {
public:
	virtual ~ISSD() {};

	virtual void Read(int lba) = 0;
	virtual void Write(int lba, const std::string& value) = 0;
	virtual void Erase(int lba, int size) = 0;
	virtual void WriteValueToOutputFile(const std::string& value) = 0;

};

class SSD : public ISSD{
public:
	SSD();

	void Read(int lba) override;
	void Write(int lba, const std::string& value) override;
	void Erase(int lba, int size) override;
	void WriteValueToOutputFile(const std::string& value) override;

	bool IsValidAddress(int address);
	bool IsValidValue(const std::string& value);
	bool IsValidSIze(int lba, int size);

	void SetAddressRange(int addressMin, int addressMax);
	std::pair<int, int> GetAddressRange( );

private:
	void Initialize(const std::string& fileName);

	bool IsNandFileExist();
	bool IsOutputFileExist();
	
	void UpdateValueAtAddress(int lba, const std::string& value);
	std::vector<std::string> ReadDataForEntireAddress();
	void WriteDataToEntireAddress(std::vector<std::string>& values);

	size_t GetEntireAddressSize();
	std::string MakeDataAsSaveFormat(int address, const std::string& data);

	std::pair<int, std::string> SplitLineToLbaAndValue(const std::string& line);
	void ReadValueAtAddress(int lba);

private:
	int addressMin{ ADDRESS_MIN_LIMIT };
	int addressMax{ ADDRESS_MAX_LIMIT };

	static constexpr int ADDRESS_MIN_LIMIT = 0;
	static constexpr int ADDRESS_MAX_LIMIT = 99;
	static constexpr int ERASE_SIZE_LIMIT = 10;

	inline static const std::string NAND_FILE_PATH{"ssd_nand.txt"};
	inline static const std::string OUTPUT_FILE_PATH{ "ssd_output.txt" };

	inline static const std::regex VALID_DATA_PATTERN{ "^0x[0-9A-F]{8}$" };
	inline static const std::string ERROR_MESSAGE{ "ERROR" };

};


inline void SSD::SetAddressRange(int addrMin, int addrMax) {
	if (addrMax < addrMin) return;

	this->addressMin = std::max(addrMin, ADDRESS_MIN_LIMIT);
	this->addressMax = std::min(addrMax, ADDRESS_MAX_LIMIT);
}

inline std::pair<int, int> SSD::GetAddressRange() {
	return { addressMin, addressMax };
}

inline size_t SSD::GetEntireAddressSize() {
	return static_cast<size_t>(addressMax - addressMin + 1);
}

inline std::string SSD::MakeDataAsSaveFormat(int address, const std::string& data) {
	return std::to_string(address) + " " + data;
}