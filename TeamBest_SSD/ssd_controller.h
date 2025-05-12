#pragma once
#include <string>
#include <vector>
#include <memory>

#include "ssd.h"
#include "util.h"

enum SSD_COMMAND_PARAM_INDEX {
	COMMAND_NAME = 0,
	ADDRESS,
	VALUE
};

class SSDController{
public:
	SSDController(std::shared_ptr<ISSD> ssdExecutor);
	bool Run(const std::string& command);

private:
	std::vector<std::string> CommandParser(const std::string& command, char delimiter = ' ');
	bool IsValidCommand(const std::vector<std::string>& commandTokens);
	void Execute(const std::vector<std::string>& commandTokens);	
	void SetExecutor(std::shared_ptr<ISSD> ssdExecutor);
	void ConvertCommandToUpperCase(std::string & command);

private:
	std::shared_ptr<ISSD> ssd;

	inline static const std::string READ_COMMAND_NAME = {"R"};
	inline static const std::string WRITE_COMMAND_NAME = { "W" };
	static constexpr size_t READ_COMMAND_PARAM_COUNT = 1;
	static constexpr size_t WRITE_COMMAND_PARAM_COUNT = 2;
};

inline void SSDController::SetExecutor(std::shared_ptr<ISSD> ssdExecutor) {
	ssd = ssdExecutor;
}

inline void SSDController::ConvertCommandToUpperCase(std::string& command) {
	command = BEST_UTILS::ToUpper(command);
}