
#include "ACName.h"
#include "Logging.h"
#include "lassert.h"
#include <cstring>
#include <string.h>

ACName::ACName(uint16_t length)
    : ElementHeader(ElementHeader::ACName,
                    (sizeof(ACName) - sizeof(ElementHeader)) + length) {
}

uint16_t ACName::GetDataLenght() const {
    return GetLength();
}

bool ACName::Validate() const {
    static_assert(sizeof(ACName) == 4);
    if (ElementHeader::GetElementType() != ElementHeader::ACName) {
        return false;
    }
    if (GetDataLenght() > ACName::max_data_size) {
        return false;
    }
    return true;
}

WritableACName::WritableACName(const std::string_view name)
    : element{ (uint16_t)name.size() }, name{ name } {
    static_assert(sizeof(element) == 4);
    ASSERT(name.size() <= ACName::max_data_size);
}

void WritableACName::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(ElementHeader) <= raw_data->end);
    std::memcpy(raw_data->current, &element, sizeof(element));
    raw_data->current += sizeof(element);

    std::memcpy(raw_data->current, name.data(), name.size());
    raw_data->current += name.size();
}

void WritableACName::Log() const {
    log_i("ME ACName :%.*s", (int)name.size(), name.data());
}

bool ReadableACName::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ElementHeader) > raw_data->end) {
        return false;
    }

    auto res = (ReadableACName::Element *)raw_data->current;
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

const ReadableACName::Element *const ReadableACName::Get() const {
    return element;
}

void ReadableACName::Log() const {
    ASSERT(element != nullptr);
    log_i("ME ACName :%.*s", (int)element->GetDataLenght(), element->name);
}

ElementHeader::ElementType ReadableACName::GetElementType() const {
    return ElementHeader::ACName;
}

bool ReadableACName::IsPresent() const {
    return is_present;
}
