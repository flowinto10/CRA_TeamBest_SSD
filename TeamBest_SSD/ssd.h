#pragma once
#include <string>
#include <filesystem>
#include <fstream>
#include <utility>
#include <string_view>
#include <regex>

class SSD{
public:
	SSD() = default;

	void Initialize(const std::string& fileName);
	void Read(int lba);
	void Write(int lba, const std::string& value);

	void SetAddressRange(int addressMin, int addressMax);
	std::pair<int, int> GetAddressRange( );

private:
	bool IsNandFileExist();
	bool IsValidAddress(int address);
	bool IsValidValue(const std::string& value);	
	
	void UpdateValueAtAddress(int lba, const std::string& value);
	std::vector<std::string> ReadDataForEntireAddress();
	void WriteDataToEntireAddress(std::vector<std::string>& values);

	void WriteErrorMessageToOutputFile();
	size_t GetEntireAddressSize();
	std::string MakeDataAsSaveFormat(int address, const std::string& data);

private:
	int addressMin{ ADDRESS_MIN_LIMIT };
	int addressMax{ ADDRESS_MAX_LIMIT };

	static constexpr int ADDRESS_MIN_LIMIT = 0;
	static constexpr int ADDRESS_MAX_LIMIT = 99;

	inline static const std::string NAND_FILE_PATH{"ssd_nand.txt"};
	inline static const std::string OUTPUT_FILE_PATH{ "ssd_output.txt" };

	const std::regex VALID_PATTERN{ "^0x[0-9A-F]{8}$" };;
	inline static const std::string WRITE_ERROR_MESSAGE{ "ERROR" };

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