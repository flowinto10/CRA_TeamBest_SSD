#include "ssd_factory.h"

std::shared_ptr<ISSD> SSDFactory::createSSD(const std::string& type) {
    if (type == "SSD") return std::make_shared<SSD>();
    return nullptr;
}