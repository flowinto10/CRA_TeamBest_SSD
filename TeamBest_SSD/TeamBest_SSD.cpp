#include <iostream>
#include "gmock/gmock.h"

using namespace testing;

TEST(SSD_TS, TC0)
{
    EXPECT_EQ(0, 0);
}

int main()
{
#ifdef _DEBUG
    ::testing::InitGoogleMock();
    return RUN_ALL_TESTS();
#else
    std::cout << "Hello CRA !\n";

    return 0;
#endif
}

