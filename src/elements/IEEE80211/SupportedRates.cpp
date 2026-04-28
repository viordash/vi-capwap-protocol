#include "SupportedRates.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <cstring>

SupportedRates::SupportedRates(uint8_t radio_id, uint8_t rates_count)
    : ElementHeader(ElementHeader::SupportedRates, 1 + rates_count), radio_id{ radio_id } {
}

uint8_t SupportedRates::GetRadioID() const {
    return radio_id;
}

uint8_t SupportedRates::GetRatesCount() const {
    return ElementHeader::GetLength() - 1;
}

bool SupportedRates::Validate() const {
    if (ElementHeader::GetElementType() != ElementHeader::SupportedRates) {
        return false;
    }
    auto length = ElementHeader::GetLength();
    if (length < sizeof(radio_id) + min_rates) {
        return false;
    }
    if (length > sizeof(radio_id) + max_rates) {
        return false;
    }
    if (radio_id > 31) {
        return false;
    }
    return true;
}

void SupportedRates::Serialize(RawData *raw_data) const {
    size_t total_size = sizeof(ElementHeader) + ElementHeader::GetLength();
    ASSERT(raw_data->current + total_size <= raw_data->end);
#pragma GCC diagnostic push
#if __GNUC__ >= 8
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
    memcpy(raw_data->current, this, total_size);
#pragma GCC diagnostic pop
    raw_data->current += total_size;
}

SupportedRates *SupportedRates::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ElementHeader) > raw_data->end) {
        return nullptr;
    }

    auto res = (SupportedRates *)raw_data->current;
    size_t total_size = sizeof(ElementHeader) + res->GetLength();
    if (raw_data->current + total_size > raw_data->end) {
        return nullptr;
    }
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += total_size;
    return res;
}

WritableSupportedRatesArray::Item::Item(uint8_t radio_id, nonstd::span<const uint8_t> rates)
    : rates_data(rates), header(radio_id, rates_data.size()) {
}

uint8_t WritableSupportedRatesArray::Item::GetRadioID() const {
    return header.GetRadioID();
}

WritableSupportedRatesArray::WritableSupportedRatesArray() {
    items.reserve(ReadableSupportedRatesArray::max_count);
}

void WritableSupportedRatesArray::Add(Item element) {
    ASSERT(items.size() + 1 <= ReadableSupportedRatesArray::max_count);
    items.emplace_back(std::move(element));
}

bool WritableSupportedRatesArray::Empty() const {
    return items.empty();
}

void WritableSupportedRatesArray::Clear() {
    items.clear();
}

void WritableSupportedRatesArray::Serialize(RawData *raw_data) const {
    for (const auto &elem : items) {
        elem.header.Serialize(raw_data);
        memcpy(raw_data->current - elem.rates_data.size(),
               elem.rates_data.data(),
               elem.rates_data.size());
    }
}

void WritableSupportedRatesArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME SupportedRates #%zu RadioID:%u, Rates count:%zu",
              i,
              items[i].GetRadioID(),
              items[i].rates_data.size());
    }
}

ReadableSupportedRatesArray::ReadableSupportedRatesArray() : count{ 0 } {
}

bool ReadableSupportedRatesArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableSupportedRatesArray::Deserialize elements count exceeds");
        return false;
    }

    auto sr = SupportedRates::Deserialize(raw_data);
    if (sr == nullptr) {
        return false;
    }
    items[count] = sr;
    count++;
    return true;
}

nonstd::span<const SupportedRates *const> ReadableSupportedRatesArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableSupportedRatesArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME SupportedRates #%zu RadioID:%u, Rates count:%u",
              i,
              items[i]->GetRadioID(),
              items[i]->GetRatesCount());
    }
}
