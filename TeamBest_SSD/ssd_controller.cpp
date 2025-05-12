#include "ssd_controller.h"
#include <string>
#include <vector>

SSDController::SSDController(std::shared_ptr<ISSD> ssdExecutor)
    : ssd{ ssdExecutor } {
}


bool SSDController::Run(const std::string& command) {
	std::vector<std::string> tokens = CommandParser(command);
    ConvertCommandToUpperCase(tokens[SSD_COMMAND_PARAM_INDEX::COMMAND_NAME]);
    if (!IsValidCommand(tokens)) return false;

    Execute(tokens);
    return true;
}

std::vector<std::string> SSDController::CommandParser(
	const std::string& command, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;

    for (char ch : command) {
        if (ch == delimiter) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        }
        else {
            token += ch;
        }
    }

    if (!token.empty()) {
        tokens.push_back(token);
    }

    return tokens;
}

bool SSDController::IsValidCommand(const std::vector<std::string>& commandTokens) {
    const std::string& commandName = 
        BEST_UTILS::ToUpper(commandTokens[SSD_COMMAND_PARAM_INDEX::COMMAND_NAME]);
    if (commandName != READ_COMMAND_NAME &&
        commandName != WRITE_COMMAND_NAME ) return false;

    size_t commandParamCount = commandTokens.size() - 1;
    if (commandName == READ_COMMAND_NAME
        && commandParamCount != READ_COMMAND_PARAM_COUNT) return false;

    if (commandName == WRITE_COMMAND_NAME
        && commandParamCount != WRITE_COMMAND_PARAM_COUNT) return false;

    return true;
}

void SSDController::Execute(const std::vector<std::string>& commandTokens) {
    const std::string& commandName 
        = commandTokens[SSD_COMMAND_PARAM_INDEX::COMMAND_NAME];
    if (commandName == READ_COMMAND_NAME) {
        ssd->Read(std::stoi(commandTokens[SSD_COMMAND_PARAM_INDEX::ADDRESS]));
    }
    else if (commandName == WRITE_COMMAND_NAME) {
        ssd->Write(std::stoi(commandTokens[SSD_COMMAND_PARAM_INDEX::ADDRESS]),
                   commandTokens[SSD_COMMAND_PARAM_INDEX::VALUE]);
    }
}


