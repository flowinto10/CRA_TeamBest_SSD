#include "ssd_controller.h"
#include <string>
#include <vector>

SSDController::SSDController(std::shared_ptr<ISSD> ssdExecutor)
    : ssd{ ssdExecutor } {
    cmdBuffers = std::make_shared<CommandBuffer>();
}


bool SSDController::Run(const std::string& command) {
	std::vector<std::string> tokens = BEST_UTILS::StringTokenizer(command);
    ConvertCommandToUpperCase(tokens[SSD_COMMAND_PARAM_INDEX::COMMAND]);
    if (!IsValidCommand(tokens)) return false;

    Execute(tokens);
    return true;
}

bool SSDController::IsValidCommand(const std::vector<std::string>& commandTokens) {
    const std::string& commandName = 
        BEST_UTILS::ToUpper(commandTokens[SSD_COMMAND_PARAM_INDEX::COMMAND]);

    if (!IsSupportedCommandName(commandName)) return false;

    size_t commandParamCount = commandTokens.size() - 1;
    for (size_t i = 0; i < VALID_COMMAND_LIST.size(); ++i) {
        if (commandName == VALID_COMMAND_LIST[i]
            && commandParamCount != COMMAND_PRAM_COUNT[i]) return false;
    }

    return true;
}

void SSDController::Execute(const std::vector<std::string>& commandTokens) {
    const std::string& commandName 
        = commandTokens[SSD_COMMAND_PARAM_INDEX::COMMAND];

    if (commandName == VALID_COMMAND_LIST[SSD_COMMAND_TYPES::READ]) {
        ssd->Read(std::stoi(commandTokens[SSD_COMMAND_PARAM_INDEX::ADDRESS]));
    }
    else if (commandName == VALID_COMMAND_LIST[SSD_COMMAND_TYPES::WRITE]) {
        ssd->Write(std::stoi(commandTokens[SSD_COMMAND_PARAM_INDEX::ADDRESS]),
                   commandTokens[SSD_COMMAND_PARAM_INDEX::VALUE]);
    }
    else if (commandName == VALID_COMMAND_LIST[SSD_COMMAND_TYPES::ERASE]) {
        ssd->Erase(std::stoi(commandTokens[SSD_COMMAND_PARAM_INDEX::ADDRESS]),
            std::stoi(commandTokens[SSD_COMMAND_PARAM_INDEX::SIZE]));
    }
    else if (commandName == VALID_COMMAND_LIST[SSD_COMMAND_TYPES::FLUSH]) {
        std::vector<std::string> commandInBuffers = cmdBuffers->Flush();
        for (const auto& command : commandInBuffers) {
#ifdef _DEBUG
            std::cout << "Command In Buffers : " << command << std::endl;
#endif
            Run(command);
        }
    }
}


