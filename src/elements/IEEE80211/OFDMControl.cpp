#include "OFDMControl.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <cstring>

OFDMControl::OFDMControl(uint8_t radio_id,
                         uint8_t current_channel,
                         uint8_t band_support,
                         uint32_t ti_threshold)
    : ElementHeader(ElementHeader::OFDMControl, sizeof(OFDMControl) - sizeof(ElementHeader)),
      radio_id{ radio_id }, reserved{ 0 }, current_channel{ current_channel },
      band_support{ band_support }, ti_threshold{ ti_threshold } {
}

uint8_t OFDMControl::GetRadioID() const {
    return radio_id;
}

uint8_t OFDMControl::GetCurrentChannel() const {
    return current_channel;
}

uint8_t OFDMControl::GetBandSupport() const {
    return band_support;
}

uint32_t OFDMControl::GetTIThreshold() const {
    return ti_threshold.Get();
}

bool OFDMControl::Validate() const {
    static_assert(sizeof(OFDMControl) == 12); // 4 header + 8 value
    if (ElementHeader::GetElementType() != ElementHeader::OFDMControl) {
        return false;
    }
    if (ElementHeader::GetLength() != (sizeof(OFDMControl) - sizeof(ElementHeader))) {
        return false;
    }
    if (radio_id > 31) {
        return false;
    }
    return true;
}

void OFDMControl::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(OFDMControl) <= raw_data->end);
#pragma GCC diagnostic push
#if __GNUC__ >= 8
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
    memcpy(raw_data->current, this, sizeof(OFDMControl));
#pragma GCC diagnostic pop
    raw_data->current += sizeof(OFDMControl);
}

OFDMControl *OFDMControl::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(OFDMControl) > raw_data->end) {
        return nullptr;
    }

    auto res = (OFDMControl *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(OFDMControl);
    return res;
}

WritableOFDMControlArray::WritableOFDMControlArray() {
    items.reserve(ReadableOFDMControlArray::max_count);
}

void WritableOFDMControlArray::Add(OFDMControl element) {
    ASSERT(items.size() + 1 <= ReadableOFDMControlArray::max_count);

    items.emplace_back(std::move(element));
}

bool WritableOFDMControlArray::Empty() const {
    return items.empty();
}

void WritableOFDMControlArray::Clear() {
    items.clear();
}

void WritableOFDMControlArray::Serialize(RawData *raw_data) const {
    for (const auto &elem : items) {
        elem.Serialize(raw_data);
    }
}

void WritableOFDMControlArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME OFDMControl #%zu RadioID:%u, Channel:%u, BandSupport:0x%02X, TIThreshold:%u",
              i,
              items[i].GetRadioID(),
              items[i].GetCurrentChannel(),
              items[i].GetBandSupport(),
              items[i].GetTIThreshold());
    }
}

ReadableOFDMControlArray::ReadableOFDMControlArray() : count{ 0 } {
}

bool ReadableOFDMControlArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableOFDMControlArray::Deserialize elements count exceeds");
        return false;
    }

    auto ofdm_control = OFDMControl::Deserialize(raw_data);
    if (ofdm_control == nullptr) {
        return false;
    }
    items[count] = ofdm_control;
    count++;
    return true;
}

nonstd::span<const OFDMControl *const> ReadableOFDMControlArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableOFDMControlArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME OFDMControl #%zu RadioID:%u, Channel:%u, BandSupport:0x%02X, TIThreshold:%u",
              i,
              items[i]->GetRadioID(),
              items[i]->GetCurrentChannel(),
              items[i]->GetBandSupport(),
              items[i]->GetTIThreshold());
    }
}
