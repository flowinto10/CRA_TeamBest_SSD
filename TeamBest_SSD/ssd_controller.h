#pragma once
#include <string>
#include <vector>
#include <memory>

#include "ssd.h"
#include "command_buffer.h"
#include "util.h"

namespace SSD_COMMAND_TYPES {
	enum SSD_COMMAND_TYPES {
		READ = 0,
		WRITE,
		ERASE,
		FLUSH
	};
}

namespace SSD_COMMAND_PARAM_INDEX {
	constexpr int COMMAND = 0;
	constexpr int ADDRESS = 1;
	constexpr int VALUE = 2;
	constexpr int SIZE = 2;
}

class SSDController{
public:
	SSDController(std::shared_ptr<ISSD> ssdExecutor);
	bool Run(const std::string& command);

private:
	bool IsValidCommand(const std::vector<std::string>& commandTokens);
	void Execute(
		const std::vector<std::string>& commandTokens,
		const std::string & originCommand);	
	void SetExecutor(std::shared_ptr<ISSD> ssdExecutor);
	void ConvertCommandToUpperCase(std::string & command);
	void ConvertFirstLetterToUpperCase(std::string& command);
	bool IsSupportedCommandName(const std::string& command);

	void ExcuteReadCommand(const std::vector<std::string>& commandTokens);
	void ExcuteWriteCommand(
		const std::vector<std::string>& commandTokens,
		const std::string& originCommand);
	void ExcuteEraseCommand(
		const std::vector<std::string>& commandTokens,
		const std::string& originCommand);
	void ExcuteFlushCommand();

private:
	std::shared_ptr<ISSD> ssd;
	std::shared_ptr<CommandBuffer> cmdBuffers;

	inline static const std::vector<std::string> VALID_COMMAND_LIST = {
		{"R"},
		{"W"},
		{"E"},
		{"F"}
	};
	inline static const std::vector<size_t> COMMAND_PRAM_COUNT = {
		1,
		2,
		2,
		0
	};
	const std::string ERROR_MESSAGE{ "ERROR" };
};

inline void SSDController::SetExecutor(std::shared_ptr<ISSD> ssdExecutor) {
	ssd = ssdExecutor;
}

inline void SSDController::ConvertCommandToUpperCase(std::string& command) {
	command = BEST_UTILS::ToUpper(command);
}

inline void SSDController::ConvertFirstLetterToUpperCase(std::string& command){
	if(!command.empty()) BEST_UTILS::ToUpper(command[0]);
}

inline bool SSDController::IsSupportedCommandName(const std::string& command) {
	return std::find(VALID_COMMAND_LIST.begin(), VALID_COMMAND_LIST.end(), command) 
		!= VALID_COMMAND_LIST.end();
}