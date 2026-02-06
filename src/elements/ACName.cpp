
#include "ACName.h"
#include "lassert.h"
#include "logging.h"
#include <string.h>

WritableACName::WritableACName(std::string_view name)
    : ElementHeader(ElementHeader::ACName, name.size()), name{ name.data() } {
}

void WritableACName::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(ElementHeader) + GetLength() <= raw_data->end);
    ElementHeader *dst = (ElementHeader *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(ElementHeader);
    memcpy(raw_data->current, name, GetLength());
    raw_data->current += GetLength();
}
uint16_t WritableACName::GetTotalLength() const {
    return GetLength() + sizeof(ElementHeader);
}

void WritableACName::Log() const {
    log_i("ME ACName :%.*s", GetLength(), name);
}

ReadableACName::ReadableACName() : ElementHeader(ElementHeader::ACName, 0) {
}
bool ReadableACName::Validate() const {
    static_assert(sizeof(ReadableACName) == 4);
    return ElementHeader::GetElementType() == ElementHeader::ACName
        && GetLength()
               <= ReadableACName::max_data_size + (sizeof(ReadableACName) - sizeof(ElementHeader));
}

ReadableACName *ReadableACName::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ElementHeader) > raw_data->end) {
        return nullptr;
    }

    auto res = (ReadableACName *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    if (raw_data->current + sizeof(ElementHeader) + res->GetLength() > raw_data->end) {
        return nullptr;
    }

    raw_data->current += sizeof(ElementHeader) + res->GetLength();
    return res;
}

void ReadableACName::Log() const {
    log_i("ME ACName :%.*s", GetLength(), name);
}
