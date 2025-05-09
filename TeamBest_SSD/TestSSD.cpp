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

    // ���� ����: ������ ������ ����
    if (std::filesystem::exists(filename)) {
        std::filesystem::remove(filename);
    }

    app.Initialize(filename);

    // ����: ������ �����Ǿ����� Ȯ��
    EXPECT_TRUE(std::filesystem::exists(filename));
}