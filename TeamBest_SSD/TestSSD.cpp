#include "gmock/gmock.h"
#include "ssd.h"

#include <memory>

using namespace testing;

TEST(TestSSD, ContructorTest) {
	EXPECT_NO_THROW(std::shared_ptr<SSD> ssd = std::make_shared<SSD>());
}