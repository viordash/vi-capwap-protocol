#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "elements/ElementHeader.h"
#include "span.hpp"

struct __attribute__((packed)) ImageDataHeader : ElementHeader {
    static const size_t max_data_size = 1024;
    enum Type : uint8_t { ImageDataIsIncluded = 1, LastImageDataIsIncluded = 2, ErrorOccured = 5 };

    Type type;

    ImageDataHeader(const ImageDataHeader &) = delete;
    ImageDataHeader(ImageDataHeader::Type type, uint16_t length);
    uint16_t GetDataLenght() const;
};

struct __attribute__((packed)) ReadableImageData : ImageDataHeader {
    uint8_t data[];

    ReadableImageData(const ReadableImageData &) = delete;

    bool Validate() const;
    static ReadableImageData *Deserialize(RawData *raw_data);
    uint16_t GetTotalLength() const;
    void Log() const;
};

struct WritableImageData {
    const uint8_t *data;
    ImageDataHeader header;

    WritableImageData(const WritableImageData &) = delete;
    WritableImageData(ImageDataHeader::Type type, const nonstd::span<const uint8_t> &data);

    void Serialize(RawData *raw_data) const;
    uint16_t GetTotalLength() const;
};
