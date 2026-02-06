
#include "ImageData.h"
#include "lassert.h"
#include "logging.h"
#include <string.h>

ImageDataHeader::ImageDataHeader(ImageDataHeader::Type type, uint16_t length)
    : ElementHeader(ElementHeader::ImageData,
                    (sizeof(ImageDataHeader) - sizeof(ElementHeader)) + length),
      type{ type } {
}

uint16_t ImageDataHeader::GetDataLenght() const {
    return GetLength() - (sizeof(ImageDataHeader) - sizeof(ElementHeader));
}

bool ReadableImageData::Validate() const {
    static_assert(sizeof(ReadableImageData) == 5);
    if (ElementHeader::GetElementType() != ElementHeader::ImageData) {
        return false;
    }
    if (GetDataLenght() > ImageDataHeader::max_data_size) {
        return false;
    }

    switch (type) {
        case Type::ImageDataIsIncluded:
        case Type::LastImageDataIsIncluded:
        case Type::ErrorOccured:
            return true;

        default:
            break;
    }
    return false;
}

ReadableImageData *ReadableImageData::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ElementHeader) > raw_data->end) {
        return nullptr;
    }

    auto res = (ReadableImageData *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    if (raw_data->current + sizeof(ElementHeader) + res->GetLength() > raw_data->end) {
        return nullptr;
    }

    raw_data->current += sizeof(ElementHeader) + res->GetLength();
    return res;
}

uint16_t ReadableImageData::GetTotalLength() const {
    return GetLength() + sizeof(ElementHeader);
}

void ReadableImageData::Log() const {
    log_i("ME ImageData data lenght: %u, type: %u", GetDataLenght(), type);
}

WritableImageData::WritableImageData(ImageDataHeader::Type type,
                                     const nonstd::span<const uint8_t> &data)
    : data{ data.data() }, header{ type, (uint16_t)data.size() } {
}

void WritableImageData::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(ImageDataHeader) <= raw_data->end);
    ImageDataHeader *dst = (ImageDataHeader *)raw_data->current;
    *dst = header;
    raw_data->current += sizeof(ImageDataHeader);

    uint16_t data_size = header.GetDataLenght();
    memcpy(raw_data->current, data, data_size);
    raw_data->current += data_size;
}
uint16_t WritableImageData::GetTotalLength() const {
    return header.GetLength() + sizeof(ElementHeader);
}
