#include "RateSet.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <cstring>

RateSet::RateSet(uint8_t radio_id, uint16_t length)
    : ElementHeader(ElementHeader::RateSet, sizeof(RateSet) - sizeof(ElementHeader) + length),
      radio_id{ radio_id } {
}

uint8_t RateSet::GetRadioID() const {
    return radio_id;
}

bool RateSet::Validate() const {
    static_assert(sizeof(RateSet) == 5); // 4 header + 1 radio_id
    if (ElementHeader::GetElementType() != ElementHeader::RateSet) {
        return false;
    }
    uint16_t element_length = ElementHeader::GetLength();
    if (element_length < (sizeof(RateSet) - sizeof(ElementHeader) + min_rate_set_length)) {
        return false;
    }
    size_t rate_set_length = element_length - (sizeof(RateSet) - sizeof(ElementHeader));
    if (rate_set_length < min_rate_set_length || rate_set_length > max_rate_set_length) {
        return false;
    }
    if (radio_id > 31) {
        return false;
    }
    return true;
}

WritableRateSetArray::WritableRateSetArray() {
    items.reserve(ReadableRateSetArray::max_count);
}

void WritableRateSetArray::Add(uint8_t radio_id, nonstd::span<const uint8_t> rate_set_data) {
    ASSERT(items.size() + 1 <= ReadableRateSetArray::max_count);
    ASSERT(rate_set_data.size() >= RateSet::min_rate_set_length);
    ASSERT(rate_set_data.size() <= RateSet::max_rate_set_length);

    items.push_back({ radio_id, rate_set_data });
}

bool WritableRateSetArray::Empty() const {
    return items.empty();
}

void WritableRateSetArray::Clear() {
    items.clear();
}

void WritableRateSetArray::Serialize(RawData *raw_data) const {
    for (const auto &item : items) {
        ASSERT(raw_data->current + sizeof(item.header) <= raw_data->end);
        std::memcpy(raw_data->current, &item.header, sizeof(item.header));
        raw_data->current += sizeof(item.header);
        uint16_t data_size =
            item.header.GetLength() - (sizeof(item.header) - sizeof(ElementHeader));
        std::memcpy(raw_data->current, item.data.data(), data_size);
        raw_data->current += data_size;
    }
}

void WritableRateSetArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME RateSet #%zu RadioID:%u, RateSet size:%zu",
              i,
              items[i].header.GetRadioID(),
              items[i].data.size());
    }
}

ReadableRateSetArray::ReadableRateSetArray() : count{ 0 } {
}

bool ReadableRateSetArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableRateSetArray::Deserialize elements count exceeds");
        return false;
    }

    auto res = (ReadableRateSetArray::Item *)raw_data->current;
    if (!res->Validate()) {
        return false;
    }

    uint8_t *last = raw_data->current + sizeof(ElementHeader) + res->GetLength();
    if (last > raw_data->end) {
        return false;
    }

    raw_data->current = last;
    items[count] = res;
    count++;
    return true;
}

nonstd::span<const ReadableRateSetArray::Item *const> ReadableRateSetArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableRateSetArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME RateSet #%zu RadioID:%u, RateSet size:%zu",
              i,
              items[i]->GetRadioID(),
              (items[i]->GetLength() - (sizeof(RateSet) - sizeof(ElementHeader))));
    }
}

ElementHeader::ElementType ReadableRateSetArray::GetElementType() const {
    return ElementHeader::RateSet;
}

bool ReadableRateSetArray::IsPresent() const {
    return count > 0;
}
