#include "MultiDomainCapability.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <cstring>

MultiDomainCapability::MultiDomainCapability(uint8_t radio_id,
                                             uint16_t first_channel,
                                             uint16_t number_of_channels,
                                             uint16_t max_tx_power_level)
    : ElementHeader(ElementHeader::MultiDomainCapability,
                    sizeof(MultiDomainCapability) - sizeof(ElementHeader)),
      radio_id{ radio_id }, reserved{ 0 }, first_channel{ first_channel },
      number_of_channels{ number_of_channels }, max_tx_power_level{ max_tx_power_level } {
}

uint8_t MultiDomainCapability::GetRadioID() const {
    return radio_id;
}

uint16_t MultiDomainCapability::GetFirstChannel() const {
    return first_channel.Get();
}

uint16_t MultiDomainCapability::GetNumberOfChannels() const {
    return number_of_channels.Get();
}

uint16_t MultiDomainCapability::GetMaxTxPowerLevel() const {
    return max_tx_power_level.Get();
}

bool MultiDomainCapability::Validate() const {
    static_assert(sizeof(MultiDomainCapability) == 12); // 4 header + 8 value
    if (ElementHeader::GetElementType() != ElementHeader::MultiDomainCapability) {
        return false;
    }
    if (ElementHeader::GetLength() != (sizeof(MultiDomainCapability) - sizeof(ElementHeader))) {
        return false;
    }
    if (radio_id > 31) {
        return false;
    }
    return true;
}

void MultiDomainCapability::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(MultiDomainCapability) <= raw_data->end);
#pragma GCC diagnostic push
#if __GNUC__ >= 8
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
    memcpy(raw_data->current, this, sizeof(MultiDomainCapability));
#pragma GCC diagnostic pop
    raw_data->current += sizeof(MultiDomainCapability);
}

MultiDomainCapability *MultiDomainCapability::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(MultiDomainCapability) > raw_data->end) {
        return nullptr;
    }

    auto res = (MultiDomainCapability *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(MultiDomainCapability);
    return res;
}

WritableMultiDomainCapabilityArray::WritableMultiDomainCapabilityArray() {
    items.reserve(ReadableMultiDomainCapabilityArray::max_count);
}

void WritableMultiDomainCapabilityArray::Add(MultiDomainCapability element) {
    ASSERT(items.size() + 1 <= ReadableMultiDomainCapabilityArray::max_count);

    items.emplace_back(std::move(element));
}

bool WritableMultiDomainCapabilityArray::Empty() const {
    return items.empty();
}

void WritableMultiDomainCapabilityArray::Clear() {
    items.clear();
}

void WritableMultiDomainCapabilityArray::Serialize(RawData *raw_data) const {
    for (const auto &elem : items) {
        elem.Serialize(raw_data);
    }
}

void WritableMultiDomainCapabilityArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME MultiDomainCapability #%zu RadioID:%u, FirstChannel:%u, "
              "NumberOfChannels:%u, MaxTxPowerLevel:%u",
              i,
              items[i].GetRadioID(),
              items[i].GetFirstChannel(),
              items[i].GetNumberOfChannels(),
              items[i].GetMaxTxPowerLevel());
    }
}

ReadableMultiDomainCapabilityArray::ReadableMultiDomainCapabilityArray() : count{ 0 } {
}

bool ReadableMultiDomainCapabilityArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableMultiDomainCapabilityArray::Deserialize elements count exceeds");
        return false;
    }

    auto capability = MultiDomainCapability::Deserialize(raw_data);
    if (capability == nullptr) {
        return false;
    }
    items[count] = capability;
    count++;
    return true;
}

nonstd::span<const MultiDomainCapability *const>
ReadableMultiDomainCapabilityArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableMultiDomainCapabilityArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME MultiDomainCapability #%zu RadioID:%u, FirstChannel:%u, "
              "NumberOfChannels:%u, MaxTxPowerLevel:%u",
              i,
              items[i]->GetRadioID(),
              items[i]->GetFirstChannel(),
              items[i]->GetNumberOfChannels(),
              items[i]->GetMaxTxPowerLevel());
    }
}
