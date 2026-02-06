#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "elements/ElementHeader.h"
#include "span.hpp"

struct WritableACIPv4List : ElementHeader {
    const nonstd::span<const uint32_t> addresses;

    WritableACIPv4List(const WritableACIPv4List &) = delete;
    WritableACIPv4List(const nonstd::span<const uint32_t> &addresses);

    void Serialize(RawData *raw_data) const;
    uint16_t GetTotalLength() const;
    void Log() const;
};

struct __attribute__((packed)) ReadableACIPv4List : ElementHeader {
    static const size_t max_count = 16;
    uint32_t addresses[];

    ReadableACIPv4List(const ReadableACIPv4List &) = delete;
    ReadableACIPv4List();

    bool Validate() const;
    static ReadableACIPv4List *Deserialize(RawData *raw_data);
    size_t GetCount() const;
    void Log() const;
};
