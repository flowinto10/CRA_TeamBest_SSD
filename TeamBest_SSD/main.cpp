#include <iostream>

#ifdef _DEBUG
#include "gmock/gmock.h"
using namespace testing;
#else
#include "ssd_controller.h"
#include "ssd_factory.h"
#endif


int main(int argc, char* argv[])
{
#ifdef _DEBUG
    ::testing::InitGoogleMock();
    return RUN_ALL_TESTS();
#else
    SSDFactory& factory = SSDFactory::getInstance();
    std::shared_ptr<ISSD> ssdType = factory.createSSD(argv[1]);
    if (ssdType == nullptr) return -1;

    SSDController ssdController{ ssdType };
    std::string commandLine{""};
    for (int i = 1; i < argc; ++i)
        commandLine = commandLine + " " + std::string{ argv[i] };

    std::cout << "Input Command : " << commandLine << std::endl;
    ssdController.Run(commandLine);
    std::cout << "Done\n";

    return 0;
#endif
}

