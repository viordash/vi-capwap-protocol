#include "RateSet.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <cstring>

RateSet::RateSet(uint8_t radio_id, nonstd::span<const uint8_t> rate_set_data)
    : ElementHeader(ElementHeader::RateSet,
                    sizeof(RateSet) - sizeof(ElementHeader) + rate_set_data.size()),
      radio_id{ radio_id } {
    memcpy(this->rate_set_data, rate_set_data.data(), rate_set_data.size());
}

uint8_t RateSet::GetRadioID() const {
    return radio_id;
}

nonstd::span<const uint8_t> RateSet::GetRateSetData() const {
    size_t data_length = ElementHeader::GetLength() - (sizeof(RateSet) - sizeof(ElementHeader));
    return nonstd::span<const uint8_t>(rate_set_data, data_length);
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

void RateSet::Serialize(RawData *raw_data) const {
    size_t total_size = sizeof(RateSet) - sizeof(ElementHeader)
                      + (ElementHeader::GetLength() - (sizeof(RateSet) - sizeof(ElementHeader)));
    ASSERT(raw_data->current + sizeof(ElementHeader) + total_size <= raw_data->end);

#pragma GCC diagnostic push
#if __GNUC__ >= 8
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
    memcpy(raw_data->current, this, sizeof(ElementHeader) + total_size);
#pragma GCC diagnostic pop
    raw_data->current += sizeof(ElementHeader) + total_size;
}

RateSet *RateSet::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(RateSet) > raw_data->end) {
        return nullptr;
    }

    auto res = (RateSet *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }

    size_t rate_set_length =
        res->ElementHeader::GetLength() - (sizeof(RateSet) - sizeof(ElementHeader));
    size_t total_size = sizeof(RateSet) + rate_set_length;

    if (raw_data->current + total_size > raw_data->end) {
        return nullptr;
    }

    raw_data->current += total_size;
    return res;
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
        uint16_t element_length = 1 + item.rate_set_data.size();
        ASSERT(raw_data->current + sizeof(ElementHeader) + element_length <= raw_data->end);

        // Write header
        ElementHeader header(ElementHeader::RateSet, element_length);
        memcpy(raw_data->current, &header, sizeof(ElementHeader));
        raw_data->current += sizeof(ElementHeader);

        // Write radio_id
        *raw_data->current = item.radio_id;
        raw_data->current += 1;

        // Write rate_set_data
        memcpy(raw_data->current, item.rate_set_data.data(), item.rate_set_data.size());
        raw_data->current += item.rate_set_data.size();
    }
}

void WritableRateSetArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME RateSet #%zu RadioID:%u, RateSet size:%zu",
              i,
              items[i].radio_id,
              items[i].rate_set_data.size());
    }
}

ReadableRateSetArray::ReadableRateSetArray() : count{ 0 } {
}

bool ReadableRateSetArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableRateSetArray::Deserialize elements count exceeds");
        return false;
    }

    auto rate_set = RateSet::Deserialize(raw_data);
    if (rate_set == nullptr) {
        return false;
    }
    items[count] = rate_set;
    count++;
    return true;
}

nonstd::span<const RateSet *const> ReadableRateSetArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableRateSetArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME RateSet #%zu RadioID:%u, RateSet size:%zu",
              i,
              items[i]->GetRadioID(),
              items[i]->GetRateSetData().size());
    }
}
