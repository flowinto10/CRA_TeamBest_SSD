#include <iostream>
#include "gmock/gmock.h"

using namespace testing;

TEST(SSD_TS, TC0)
{
    EXPECT_EQ(0, 0);
}

int main()
{
    std::cout << "Hello CRA !\n";

    ::testing::InitGoogleMock();
    return RUN_ALL_TESTS();
}

