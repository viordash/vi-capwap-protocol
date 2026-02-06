#pragma once

#include "ClearHeader.h"
#include "Helpers.h"
#include <cstdint>

struct __attribute__((packed)) DTLSHeader {
    Preamble preamble;
    uint32_t reserved : 24;

    DTLSHeader(const DTLSHeader &) = delete;
    DTLSHeader();

    bool Validate();
    void Serialize(RawData *raw_data) const;
    static DTLSHeader *Deserialize(RawData *raw_data);
};