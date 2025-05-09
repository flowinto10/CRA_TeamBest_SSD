#include "gmock/gmock.h"
#include "ssd.h"

#include <memory>

using namespace testing;

TEST(TestSSD, ContructorTest) {
	std::shared_ptr<SSD> ssd = std::make_shared<SSD>();

	EXPECT_EQ(1, 1);
}