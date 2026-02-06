
#include "WTPName.h"
#include "lassert.h"
#include "logging.h"
#include <string.h>

WritableWTPName::WritableWTPName(std::string_view str) : name{ str.begin(), str.end() } {
    ASSERT(name.size() <= ReadableWTPName::max_data_size);
}

void WritableWTPName::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(ElementHeader) + name.size() <= raw_data->end);

    RawData header_raw_data = *raw_data;
    raw_data->current += sizeof(ElementHeader);

    memcpy(raw_data->current, name.data(), name.size());
    raw_data->current += name.size();

    ElementHeader header(ElementHeader::WTPName, (uint16_t)name.size());
    memcpy(header_raw_data.current, &header, sizeof(header));
}

void WritableWTPName::Log() const {
    log_i("ME WTPName :%.*s", (int)name.size(), name.data());
}

ReadableWTPName::ReadableWTPName() : ElementHeader(ElementHeader::WTPName, 0) {
}
bool ReadableWTPName::Validate() const {
    static_assert(sizeof(ReadableWTPName) == 4);
    return ElementHeader::GetElementType() == ElementHeader::WTPName
        && GetLength() <= ReadableWTPName::max_data_size
                              + (sizeof(ReadableWTPName) - sizeof(ElementHeader));
}

ReadableWTPName *ReadableWTPName::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ElementHeader) > raw_data->end) {
        return nullptr;
    }

    auto res = (ReadableWTPName *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    if (raw_data->current + sizeof(ElementHeader) + res->GetLength() > raw_data->end) {
        return nullptr;
    }

    raw_data->current += sizeof(ElementHeader) + res->GetLength();
    return res;
}

void ReadableWTPName::Log() const {
    log_i("ME WTPName :%.*s", GetLength(), name);
}
