#include "MaximumMessageLength.h"
#include "lassert.h"
#include "logging.h"

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
    MaximumMessageLength *dst = (MaximumMessageLength *)raw_data->current;
    *dst = *this;
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

uint16_t MaximumMessageLength::GetTotalLength() const {
    return GetLength() + sizeof(ElementHeader);
}

void MaximumMessageLength::Log() const {
    log_i("ME MaximumMessageLength :%u", GetValue());
}