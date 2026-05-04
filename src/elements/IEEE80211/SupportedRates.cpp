#include "SupportedRates.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <cstring>

SupportedRates::SupportedRates(uint8_t radio_id, uint16_t length)
    : ElementHeader(ElementHeader::SupportedRates,
                    sizeof(SupportedRates) - sizeof(ElementHeader) + length),
      radio_id{ radio_id } {
}

uint8_t SupportedRates::GetRadioID() const {
    return radio_id;
}

uint8_t SupportedRates::GetRatesCount() const {
    return ElementHeader::GetLength() - 1;
}

bool SupportedRates::Validate() const {
    static_assert(sizeof(SupportedRates) == 5);
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

WritableSupportedRatesArray::WritableSupportedRatesArray() {
    static_assert(sizeof(Item::header) == 5);
    items.reserve(ReadableSupportedRatesArray::max_count);
}

void WritableSupportedRatesArray::Add(Item element) {
    ASSERT(items.size() + 1 <= ReadableSupportedRatesArray::max_count);

    auto it_exists = std::find_if(items.begin(), items.end(), [&element](const Item &item) {
        return item.header.GetRadioID() == element.header.GetRadioID();
    });

    if (it_exists != items.end()) {
        *it_exists = std::move(element);
        log_i("SupportedRates: replace RadioID: %u", (*it_exists).header.GetRadioID());
    } else {
        items.emplace_back(std::move(element));
    }
}

bool WritableSupportedRatesArray::Empty() const {
    return items.empty();
}

void WritableSupportedRatesArray::Clear() {
    items.clear();
}

void WritableSupportedRatesArray::Serialize(RawData *raw_data) const {
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

void WritableSupportedRatesArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME SupportedRates #%zu RadioID:%u, Rates count:%zu",
              i,
              items[i].header.GetRadioID(),
              items[i].data.size());
    }
}

ReadableSupportedRatesArray::ReadableSupportedRatesArray() : count{ 0 } {
}

bool ReadableSupportedRatesArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableSupportedRatesArray::Deserialize elements count exceeds");
        return false;
    }

    if (raw_data->current + sizeof(SupportedRates) > raw_data->end) {
        return false;
    }

    auto item = (ReadableSupportedRatesArray::Item *)raw_data->current;
    if (!item->Validate()) {
        return false;
    }

    uint8_t *last = raw_data->current + sizeof(ElementHeader) + item->GetLength();
    if (last > raw_data->end) {
        return false;
    }

    raw_data->current = last;
    items[count] = item;
    count++;
    return true;
}

nonstd::span<const ReadableSupportedRatesArray::Item *const>
ReadableSupportedRatesArray::Get() const {
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

ElementHeader::ElementType ReadableSupportedRatesArray::GetElementType() const {
    return ElementHeader::SupportedRates;
}

bool ReadableSupportedRatesArray::IsPresent() const {
    return count > 0;
}
