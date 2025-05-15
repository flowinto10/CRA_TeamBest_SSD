#include <iostream>

#ifdef _DEBUG
#include "gmock/gmock.h"
using namespace testing;
#else
#include "ssd_controller.h"
#include "ssd_factory.h"
#include "util.h"
#endif


int main(int argc, char* argv[])
{
#ifdef _DEBUG
    ::testing::InitGoogleMock();
    ::testing::GTEST_FLAG(filter) = "TestCommandBuffer.*";
    return RUN_ALL_TESTS();
#else
    std::string ssdFefaultType{ "SSD" };
    SSDFactory& factory = SSDFactory::getInstance();
    std::shared_ptr<ISSD> ssdType = factory.createSSD(ssdFefaultType);
    if (ssdType == nullptr) return -1;

    SSDController ssdController{ ssdType };
    std::string commandLine{""};
    for (int i = 1; i < argc; ++i)
        commandLine = commandLine + " " + std::string{ argv[i] };

    ssdController.Run(BEST_UTILS::Trim(commandLine));

    return 0;
#endif
}

