
#include "WTPName.h"
#include "Logging.h"
#include "lassert.h"
#include <cstring>
#include <string.h>

WTPName::WTPName(uint16_t length)
    : ElementHeader(ElementHeader::WTPName, (sizeof(WTPName) - sizeof(ElementHeader)) + length) {
}

uint16_t WTPName::GetDataLenght() const {
    return GetLength();
}

bool WTPName::Validate() const {
    static_assert(sizeof(WTPName) == 4);
    if (ElementHeader::GetElementType() != ElementHeader::WTPName) {
        return false;
    }
    if (GetDataLenght() > WTPName::max_data_size) {
        return false;
    }
    return true;
}

WritableWTPName::WritableWTPName(const std::string_view location)
    : element{ (uint16_t)location.size() }, name{ location } {
    static_assert(sizeof(element) == 4);
    ASSERT(name.size() <= WTPName::max_data_size);
}

void WritableWTPName::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(ElementHeader) <= raw_data->end);
    std::memcpy(raw_data->current, &element, sizeof(element));
    raw_data->current += sizeof(element);

    std::memcpy(raw_data->current, name.data(), name.size());
    raw_data->current += name.size();
}

void WritableWTPName::Log() const {
    log_i("ME WTPName :{}", std::string_view(name.data(), name.size()));
}

bool ReadableWTPName::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ElementHeader) > raw_data->end) {
        return false;
    }

    auto item = (ReadableWTPName::Element *)raw_data->current;
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

const ReadableWTPName::Element *ReadableWTPName::Get() const {
    return element;
}

void ReadableWTPName::Log() const {
    ASSERT(element != nullptr);
    log_i("ME WTPName :{}", std::string_view(element->name, element->GetDataLenght()));
}

ElementHeader::ElementType ReadableWTPName::GetElementType() const {
    return ElementHeader::WTPName;
}

bool ReadableWTPName::IsPresent() const {
    return is_present;
}
