
#include "LocationData.h"
#include "lassert.h"
#include "logging.h"
#include <string.h>

WritableLocationData::WritableLocationData(std::string_view str) : data{ str.begin(), str.end() } {
    ASSERT(data.size() <= ReadableLocationData::max_data_size);
}

void WritableLocationData::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(ElementHeader) + data.size() <= raw_data->end);

    RawData header_raw_data = *raw_data;
    raw_data->current += sizeof(ElementHeader);

    memcpy(raw_data->current, data.data(), data.size());
    raw_data->current += data.size();

    ElementHeader header(ElementHeader::LocationData, (uint16_t)data.size());
    memcpy(header_raw_data.current, &header, sizeof(header));
}

void WritableLocationData::Log() const {
    log_i("ME LocationData :%.*s", (int)data.size(), data.data());
}

ReadableLocationData::ReadableLocationData() : ElementHeader(ElementHeader::LocationData, 0) {
}
bool ReadableLocationData::Validate() const {
    static_assert(sizeof(ReadableLocationData) == 4);
    return ElementHeader::GetElementType() == ElementHeader::LocationData
        && GetLength() <= ReadableLocationData::max_data_size
                              + (sizeof(ReadableLocationData) - sizeof(ElementHeader));
}

ReadableLocationData *ReadableLocationData::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ElementHeader) > raw_data->end) {
        return nullptr;
    }

    auto res = (ReadableLocationData *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    if (raw_data->current + sizeof(ElementHeader) + res->GetLength() > raw_data->end) {
        return nullptr;
    }

    raw_data->current += sizeof(ElementHeader) + res->GetLength();
    return res;
}

void ReadableLocationData::Log() const {
    log_i("ME LocationData :%.*s", GetLength(), data);
}
