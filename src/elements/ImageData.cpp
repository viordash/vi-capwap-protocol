
#include "ImageData.h"
#include "Logging.h"
#include "lassert.h"
#include <cstring>
#include <string.h>

ImageData::ImageData(ImageData::Type type, uint16_t length)
    : ElementHeader(ElementHeader::ImageData, (sizeof(ImageData) - sizeof(ElementHeader)) + length),
      type{ type } {
}

uint16_t ImageData::GetDataLenght() const {
    return GetLength() - (sizeof(ImageData) - sizeof(ElementHeader));
}

bool ImageData::Validate() const {
    static_assert(sizeof(ImageData) == 5);
    if (ElementHeader::GetElementType() != ElementHeader::ImageData) {
        return false;
    }
    if (GetDataLenght() > ImageData::max_data_size) {
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

WritableImageData::WritableImageData(ImageData::Type type,
                                     const nonstd::span<const uint8_t> image_data)
    : element{ type, (uint16_t)image_data.size() }, data{ image_data } {
    static_assert(sizeof(element) == 5);
    ASSERT(image_data.size() <= ImageData::max_data_size);
}

void WritableImageData::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(ImageData) <= raw_data->end);
    std::memcpy(raw_data->current, &element, sizeof(element));
    raw_data->current += sizeof(element);

    std::memcpy(raw_data->current, data.data(), data.size());
    raw_data->current += data.size();
}

void WritableImageData::Log() const {
    log_i("ME ImageData data lenght: %u, type: %u", element.GetDataLenght(), element.type);
}

bool ReadableImageData::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ElementHeader) > raw_data->end) {
        return false;
    }

    auto res = (ReadableImageData::Element *)raw_data->current;
    if (!res->Validate()) {
        return false;
    }
    if (raw_data->current + sizeof(ElementHeader) + res->GetLength() > raw_data->end) {
        return false;
    }
    raw_data->current += sizeof(ElementHeader) + res->GetLength();

    element = res;
    is_present = true;
    return true;
}

const ReadableImageData::Element *const ReadableImageData::Get() const {
    return element;
}

void ReadableImageData::Log() const {
    ASSERT(element != nullptr);
    log_i("ME ImageData data lenght: %u, type: %u", element->GetDataLenght(), element->type);
}

ElementHeader::ElementType ReadableImageData::GetElementType() const {
    return ElementHeader::ImageData;
}

bool ReadableImageData::IsPresent() const {
    return is_present;
}
