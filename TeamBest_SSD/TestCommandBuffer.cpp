#include "gmock/gmock.h"
#include "command_buffer.h"


using namespace testing;


TEST(TestCommandBuffer, ContructorTest) {
	EXPECT_NO_THROW(std::shared_ptr<CommandBuffer> buffer 
		= std::make_shared<CommandBuffer>());
}