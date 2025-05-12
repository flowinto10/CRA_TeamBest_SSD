#include "ssd_factory.h"
#include "util.h"

std::shared_ptr<ISSD> SSDFactory::createSSD(const std::string& type) {
    if (BEST_UTILS::ToUpper(type) == "SSD") return std::make_shared<SSD>();
    return nullptr;
}