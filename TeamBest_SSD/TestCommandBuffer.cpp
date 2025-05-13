#include "gmock/gmock.h"
#include "command_buffer.h"
#include <filesystem>
#include <fstream>

using namespace testing;


TEST(TestCommandBuffer, ContructorTest) {
	EXPECT_NO_THROW(std::shared_ptr<CommandBuffer> buffer 
		= std::make_shared<CommandBuffer>());
}

TEST(TestCommandBuffer, BufferExistsAfterInit){
	const std::string dirName = "buffer";
	// 사전 정리: 파일이 있으면 삭제
	if (std::filesystem::exists(dirName)) {
		std::filesystem::remove(dirName);
	}
	CommandBuffer buffer;
	// buffer 폴더가 생성되었는지 확인
	EXPECT_TRUE(std::filesystem::exists(dirName));
}