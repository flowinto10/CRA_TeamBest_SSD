#include "ssd_controller.h"
#include <string>
#include <vector>

SSDController::SSDController(std::shared_ptr<ISSD> ssdExecutor)
    : ssd{ ssdExecutor } {
    cmdBuffers = std::make_shared<CommandBuffer>();
}


bool SSDController::Run(const std::string& orgCommand) {
    std::string command = orgCommand;
    ConvertFirstLetterToUpperCase(command);
	std::vector<std::string> tokens = BEST_UTILS::StringTokenizer(command);
    if (tokens.empty()) return false;

    if (!IsValidCommand(tokens)) return false;

    Execute(tokens, command);
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

void SSDController::Execute(
    const std::vector<std::string>& commandTokens,
    const std::string& originCommand) {
    const std::string& commandName 
        = commandTokens[SSD_COMMAND_PARAM_INDEX::COMMAND];

    if (commandName == VALID_COMMAND_LIST[SSD_COMMAND_TYPES::READ]) {
        ExcuteReadCommand(commandTokens);        
    }
    else if (commandName == VALID_COMMAND_LIST[SSD_COMMAND_TYPES::WRITE]) {
        ExcuteWriteCommand(commandTokens, originCommand);
    }
    else if (commandName == VALID_COMMAND_LIST[SSD_COMMAND_TYPES::ERASE]) {
        ExcuteEraseCommand(commandTokens, originCommand);
    }
    else if (commandName == VALID_COMMAND_LIST[SSD_COMMAND_TYPES::FLUSH]) {
        ExcuteFlushCommand();
    }
}

void SSDController::ExcuteReadCommand(const std::vector<std::string>& commandTokens) {
    int targetAddress = std::stoi(commandTokens[SSD_COMMAND_PARAM_INDEX::ADDRESS]);
    std::string fastReadResult = cmdBuffers->FastRead(targetAddress);
    if (fastReadResult.empty()) {
        ssd->Read(targetAddress);
    }
    else {
        ssd->WriteValueToOutputFile(fastReadResult);
    }
}

void SSDController::ExcuteWriteCommand(
    const std::vector<std::string>& commandTokens,
    const std::string& originCommand) {
    if (cmdBuffers->IsFull()) {
        ExcuteFlushCommand();
    }
    cmdBuffers->AppendCommand(originCommand);
}

void SSDController::ExcuteEraseCommand(
    const std::vector<std::string>& commandTokens,
    const std::string& originCommand) {
    if (cmdBuffers->IsFull()) {
        ExcuteFlushCommand();
    }
    cmdBuffers->AppendCommand(originCommand);
}

void SSDController::ExcuteFlushCommand() {
    std::vector<std::string> commandInBuffers = cmdBuffers->Flush();
    for (const auto& command : commandInBuffers) {

        std::vector<std::string> commandTokens = BEST_UTILS::StringTokenizer(command);
        const std::string& commandName
            = commandTokens[SSD_COMMAND_PARAM_INDEX::COMMAND];

       if (commandName == VALID_COMMAND_LIST[SSD_COMMAND_TYPES::WRITE]) {
           ssd->Write(std::stoi(commandTokens[SSD_COMMAND_PARAM_INDEX::ADDRESS]),
               commandTokens[SSD_COMMAND_PARAM_INDEX::VALUE]);
        }
        else if (commandName == VALID_COMMAND_LIST[SSD_COMMAND_TYPES::ERASE]) {
           ssd->Erase(std::stoi(commandTokens[SSD_COMMAND_PARAM_INDEX::ADDRESS]),
               std::stoi(commandTokens[SSD_COMMAND_PARAM_INDEX::SIZE]));
        }
    }
}

