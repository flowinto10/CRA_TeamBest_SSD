#include "gmock/gmock.h"
#include "ssd.h"

#include <memory>

using namespace testing;

TEST(TestSSD, ContructorTest) {
	EXPECT_NO_THROW(std::shared_ptr<SSD> ssd = std::make_shared<SSD>());
}

TEST(TestSSD, TestFileExistsAfterInit) {
    SSD app;
    const std::string filename = "ssd_nand.txt";

    // 사전 정리: 파일이 있으면 삭제
    if (std::filesystem::exists(filename)) {
        std::filesystem::remove(filename);
    }

    app.Initialize(filename);

    // 검증: 파일이 생성되었는지 확인
    EXPECT_TRUE(std::filesystem::exists(filename));
}