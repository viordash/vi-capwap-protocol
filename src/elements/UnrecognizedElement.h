#pragma once

#include "elements/ElementHeader.h"
#include <cstdint>

struct __attribute__((packed)) UnrecognizedElement : ElementHeader {
    uint8_t data[];

    UnrecognizedElement(const UnrecognizedElement &) = delete;

    static UnrecognizedElement *Deserialize(RawData *raw_data);
};