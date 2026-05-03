#include "InformationElement.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <cstring>

InformationElement::InformationElement(uint8_t radio_id,
                                       uint8_t wlan_id,
                                       uint8_t flags,
                                       uint16_t ie_length)
    : ElementHeader(ElementHeader::InformationElement,
                    (sizeof(InformationElement) - sizeof(ElementHeader)) + ie_length),
      radio_id{ radio_id }, wlan_id{ wlan_id }, flags{ flags } {
}

uint8_t InformationElement::GetRadioID() const {
    return radio_id;
}

uint8_t InformationElement::GetWlanID() const {
    return wlan_id;
}

uint8_t InformationElement::GetFlags() const {
    return flags;
}

bool InformationElement::GetBeaconFlag() const {
    return (flags & FLAG_BEACON) != 0;
}

bool InformationElement::GetProbeResponseFlag() const {
    return (flags & FLAG_PROBE_RESPONSE) != 0;
}

uint16_t InformationElement::GetIELength() const {
    return GetLength() - (sizeof(InformationElement) - sizeof(ElementHeader));
}

bool InformationElement::Validate() const {
    static_assert(sizeof(InformationElement) == 7);
    if (ElementHeader::GetElementType() != ElementHeader::InformationElement) {
        return false;
    }
    if (GetLength() < (sizeof(InformationElement) - sizeof(ElementHeader))) {
        return false;
    }
    if (GetIELength() > max_ie_length) {
        log_e("InformationElement: IE length exceeds limit: %u", (unsigned)max_ie_length);
        return false;
    }
    if (radio_id > 31) {
        return false;
    }
    if (wlan_id < 1 || wlan_id > 16) {
        return false;
    }
    // Check reserved bits in flags (bits 5-0 must be zero)
    if ((flags & 0x3F) != 0) {
        return false;
    }
    return true;
}

WritableInformationElementArray::Item::Item(uint8_t radio_id,
                                            uint8_t wlan_id,
                                            uint8_t flags,
                                            nonstd::span<const uint8_t> ie)
    : ie_data{ ie }, header{ radio_id, wlan_id, flags, (uint16_t)ie_data.size() } {
}

uint8_t WritableInformationElementArray::Item::GetRadioID() const {
    return header.GetRadioID();
}

uint8_t WritableInformationElementArray::Item::GetWlanID() const {
    return header.GetWlanID();
}

WritableInformationElementArray::WritableInformationElementArray() {
    items.reserve(ReadableInformationElementArray::max_count);
}

void WritableInformationElementArray::Add(WritableInformationElementArray::Item element) {
    ASSERT(items.size() + 1 <= ReadableInformationElementArray::max_count);

    items.emplace_back(std::move(element));
}

bool WritableInformationElementArray::Empty() const {
    return items.empty();
}

void WritableInformationElementArray::Clear() {
    items.clear();
}

void WritableInformationElementArray::Serialize(RawData *raw_data) const {
    for (const auto &elem : items) {
        ASSERT(raw_data->current + sizeof(InformationElement) <= raw_data->end);
        std::memcpy(raw_data->current, &elem.header, sizeof(InformationElement));
        raw_data->current += sizeof(InformationElement);

        uint16_t ie_size =
            elem.header.GetLength() - (sizeof(InformationElement) - sizeof(ElementHeader));
        std::memcpy(raw_data->current, elem.ie_data.data(), ie_size);
        raw_data->current += ie_size;
    }
}

void WritableInformationElementArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME InformationElement #%zu RadioID:%u, WlanID:%u, Flags:0x%02X (B:%u, P:%u), "
              "IELen:%zu",
              i,
              items[i].header.GetRadioID(),
              items[i].header.GetWlanID(),
              items[i].header.GetFlags(),
              items[i].header.GetBeaconFlag() ? 1 : 0,
              items[i].header.GetProbeResponseFlag() ? 1 : 0,
              items[i].ie_data.size());
    }
}

ReadableInformationElementArray::ReadableInformationElementArray() : count{ 0 } {
}

bool ReadableInformationElementArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableInformationElementArray::Deserialize elements count exceeds");
        return false;
    }

    if (raw_data->current + sizeof(InformationElement) > raw_data->end) {
        return false;
    }

    auto res = (InformationElement *)raw_data->current;
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

nonstd::span<const InformationElement *const> ReadableInformationElementArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableInformationElementArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME InformationElement #%zu RadioID:%u, WlanID:%u, Flags:0x%02X (B:%u, P:%u), "
              "IELen:%u",
              i,
              items[i]->GetRadioID(),
              items[i]->GetWlanID(),
              items[i]->GetFlags(),
              items[i]->GetBeaconFlag() ? 1 : 0,
              items[i]->GetProbeResponseFlag() ? 1 : 0,
              items[i]->GetIELength());
    }
}

ElementHeader::ElementType ReadableInformationElementArray::GetElementType() const {
    return ElementHeader::InformationElement;
}

bool ReadableInformationElementArray::IsPresent() const {
    return count > 0;
}
