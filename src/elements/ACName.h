#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "elements/ElementHeader.h"
#include <string_view>

struct __attribute__((packed)) WritableACName : ElementHeader {
    const char *name;

    WritableACName(const WritableACName &) = delete;
    WritableACName(std::string_view name);

    void Serialize(RawData *raw_data) const;
    uint16_t GetTotalLength() const;
    void Log() const;
};

struct __attribute__((packed)) ReadableACName : ElementHeader {
    static const size_t max_data_size = 512;
    char name[];

    ReadableACName(const ReadableACName &) = delete;
    ReadableACName();

    bool Validate() const;
    static ReadableACName *Deserialize(RawData *raw_data);
    void Log() const;
};
