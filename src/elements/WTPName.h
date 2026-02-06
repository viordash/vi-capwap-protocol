#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "elements/ElementHeader.h"
#include <string_view>
#include <vector>

struct WritableWTPName {
    std::vector<char> name;

    WritableWTPName(const WritableWTPName &) = default;
    WritableWTPName(WritableWTPName &&) = default;
    WritableWTPName(std::string_view str);

    void Serialize(RawData *raw_data) const;
    void Log() const;
};

struct __attribute__((packed)) ReadableWTPName : ElementHeader {
    static const size_t max_data_size = 512;
    char name[];

    ReadableWTPName(const ReadableWTPName &) = delete;
    ReadableWTPName();

    bool Validate() const;
    static ReadableWTPName *Deserialize(RawData *raw_data);
    void Log() const;
};
