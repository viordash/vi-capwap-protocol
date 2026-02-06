#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "elements/ElementHeader.h"
#include <string_view>
#include <vector>

struct WritableLocationData {
    std::vector<char> data;

    WritableLocationData(const WritableLocationData &) = default;
    WritableLocationData(WritableLocationData &&) = default;
    WritableLocationData(std::string_view str);

    void Serialize(RawData *raw_data) const;
    void Log() const;
};

struct __attribute__((packed)) ReadableLocationData : ElementHeader {
    static const size_t max_data_size = 1024;
    char data[];

    ReadableLocationData(const ReadableLocationData &) = delete;
    ReadableLocationData();

    bool Validate() const;
    static ReadableLocationData *Deserialize(RawData *raw_data);
    void Log() const;
};
