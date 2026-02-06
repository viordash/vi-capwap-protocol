#pragma once

#include "elements/ElementHeader.h"
#include "span.hpp"
#include <cstdint>

struct __attribute__((packed)) InitiateDownload : ElementHeader {

    InitiateDownload(const InitiateDownload &) = delete;
    InitiateDownload();

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static InitiateDownload *Deserialize(RawData *raw_data);

    uint16_t GetTotalLength() const;
    void Log() const;
};