
#include "LocationData.h"
#include "Logging.h"
#include "lassert.h"
#include <cstring>
#include <string.h>

LocationData::LocationData(uint16_t length)
    : ElementHeader(ElementHeader::LocationData,
                    (sizeof(LocationData) - sizeof(ElementHeader)) + length) {
}

uint16_t LocationData::GetDataLenght() const {
    return GetLength();
}

bool LocationData::Validate() const {
    static_assert(sizeof(LocationData) == 4);
    if (ElementHeader::GetElementType() != ElementHeader::LocationData) {
        return false;
    }
    if (GetDataLenght() > LocationData::max_data_size) {
        return false;
    }
    return true;
}

WritableLocationData::WritableLocationData(const std::string_view location)
    : element{ (uint16_t)location.size() }, data{ location } {
    static_assert(sizeof(element) == 4);
    ASSERT(data.size() <= LocationData::max_data_size);
}

void WritableLocationData::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(ElementHeader) <= raw_data->end);
    std::memcpy(raw_data->current, &element, sizeof(element));
    raw_data->current += sizeof(element);

    std::memcpy(raw_data->current, data.data(), data.size());
    raw_data->current += data.size();
}

void WritableLocationData::Log() const {
    log_i("ME LocationData :%.*s", (int)data.size(), data.data());
}

bool ReadableLocationData::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ElementHeader) > raw_data->end) {
        return false;
    }

    if (raw_data->current + sizeof(ReadableLocationData::Element) > raw_data->end) {
        return false;
    }

    auto item = (ReadableLocationData::Element *)raw_data->current;
    if (!item->Validate()) {
        return false;
    }

    uint8_t *last = raw_data->current + sizeof(ElementHeader) + item->GetLength();
    if (last > raw_data->end) {
        return false;
    }

    raw_data->current = last;
    element = item;
    is_present = true;
    return true;
}

ReadableLocationData::Element *const ReadableLocationData::Get() const {
    return element;
}

void ReadableLocationData::Log() const {
    ASSERT(element != nullptr);
    log_i("ME LocationData :%.*s", (int)element->GetDataLenght(), element->data);
}

ElementHeader::ElementType ReadableLocationData::GetElementType() const {
    return ElementHeader::LocationData;
}

bool ReadableLocationData::IsPresent() const {
    return is_present;
}
