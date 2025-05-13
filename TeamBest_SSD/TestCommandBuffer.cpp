#include "gmock/gmock.h"
#include "command_buffer.h"
#include <filesystem>
#include <fstream>

using namespace testing;

int countFilesInBuffer() {
	int count = 0;
	for (const auto& entry : std::filesystem::directory_iterator("buffer")) {
		if (std::filesystem::is_regular_file(entry)) {
			++count;
		}
	}
	return count;
}

TEST(TestCommandBuffer, ContructorTest) {
	EXPECT_NO_THROW(std::shared_ptr<CommandBuffer> buffer 
		= std::make_shared<CommandBuffer>());
}

TEST(TestCommandBuffer, BufferExistsAfterInit){
	const std::string dirName = "buffer";
	if (std::filesystem::exists(dirName))
		std::filesystem::remove_all(dirName);
	CommandBuffer buffer;
	EXPECT_TRUE(std::filesystem::exists(dirName));
}

TEST(TestCommandBuffer, FilesExistAfterInit) {
	const std::string dirName = "buffer";
	CommandBuffer buffer;
	int cnt = countFilesInBuffer();
	EXPECT_EQ(cnt, 5);
}