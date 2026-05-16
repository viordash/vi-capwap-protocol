#include "MaximumMessageLength.h"
#include "Logging.h"
#include "lassert.h"
#include <cstring>

MaximumMessageLength::MaximumMessageLength(uint16_t length)
    : ElementHeader(ElementHeader::MaximumMessageLength,
                    sizeof(MaximumMessageLength) - sizeof(ElementHeader)),
      length{ length } {
}
bool MaximumMessageLength::Validate() const {
    static_assert(sizeof(MaximumMessageLength) == 6);
    return ElementHeader::GetElementType() == ElementHeader::MaximumMessageLength
        && ElementHeader::GetLength() == (sizeof(MaximumMessageLength) - sizeof(ElementHeader));
}

void MaximumMessageLength::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(MaximumMessageLength) <= raw_data->end);
#pragma GCC diagnostic push
#if __GNUC__ >= 8
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
    std::memcpy(raw_data->current, this, sizeof(MaximumMessageLength));
#pragma GCC diagnostic pop
    raw_data->current += sizeof(MaximumMessageLength);
}

MaximumMessageLength *MaximumMessageLength::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(MaximumMessageLength) > raw_data->end) {
        return nullptr;
    }

    auto res = (MaximumMessageLength *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(MaximumMessageLength);
    return res;
}

uint16_t MaximumMessageLength::GetValue() const {
    return length.Get();
}

void MaximumMessageLength::Log() const {
    log_i("ME MaximumMessageLength :{}", GetValue());
}

WritableMaximumMessageLength::WritableMaximumMessageLength(uint16_t length) : element{ length } {
}

void WritableMaximumMessageLength::Serialize(RawData *raw_data) const {
    element.Serialize(raw_data);
}

void WritableMaximumMessageLength::Log() const {
    element.Log();
}

bool ReadableMaximumMessageLength::Deserialize(RawData *raw_data) {
    element = MaximumMessageLength::Deserialize(raw_data);
    is_present = element != nullptr;
    return is_present;
}

const MaximumMessageLength *ReadableMaximumMessageLength::Get() const {
    return element;
}

void ReadableMaximumMessageLength::Log() const {
    ASSERT(element != nullptr);
    element->Log();
}

ElementHeader::ElementType ReadableMaximumMessageLength::GetElementType() const {
    return ElementHeader::MaximumMessageLength;
}

bool ReadableMaximumMessageLength::IsPresent() const {
    return is_present;
}