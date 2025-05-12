#pragma once
#include "ssd.h"
#include <memory>

class SSDFactory{
    public:
        static SSDFactory& getInstance() {
            static SSDFactory instance;
            return instance;
        }

        std::shared_ptr<ISSD> createSSD(const std::string& type);

private:
    SSDFactory() = default;
    ~SSDFactory() = default;

    SSDFactory(const SSDFactory&) = delete;
    SSDFactory& operator=(const SSDFactory&) = delete;
};

