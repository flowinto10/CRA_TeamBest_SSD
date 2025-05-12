#include "gmock/gmock.h"
#include "ssd_controller.h"
#include "ssd.h"
#include "ssd_factory.h"

#include <typeinfo>

using namespace std;
using namespace testing;

TEST(TestSSDFactory, NonExistType) {
	SSDFactory& factory = SSDFactory::getInstance();

	std::shared_ptr<ISSD> iAmNotRealSSD
		= factory.createSSD("iAmNotRealSSD");
	
	EXPECT_EQ(nullptr, iAmNotRealSSD);
}

TEST(TestSSDFactory, FactorySSDType) {
	SSDFactory& factory = SSDFactory::getInstance();

	std::shared_ptr<ISSD> ssdType = factory.createSSD("ssd");
	EXPECT_EQ(typeid(SSD), typeid(*ssdType));
}