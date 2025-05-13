#include "gmock/gmock.h"
#include "command_buffer.h"
#include <filesystem>
#include <fstream>

using namespace testing;

// buffer 폴더 내의 파일 수 확인
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
	// 사전 정리: buffer 폴더가 있으면 강제 삭제
	if (std::filesystem::exists(dirName)) {
		// buffer 폴더 내의 모든 파일 및 하위 디렉터리 삭제
		std::filesystem::remove_all(dirName);
	}
	CommandBuffer buffer;
	// buffer 폴더가 생성되었는지 확인
	EXPECT_TRUE(std::filesystem::exists(dirName));
}

TEST(TestCommandBuffer, FilesExistAfterInit) {
	const std::string dirName = "buffer";

	/*if (std::filesystem::exists(dirName)) {
		std::filesystem::remove(dirName);
	}*/
	CommandBuffer buffer;
	int cnt = countFilesInBuffer();
	EXPECT_EQ(cnt, 5);
}