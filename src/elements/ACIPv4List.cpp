
#include "ACIPv4List.h"
#include "Logging.h"
#include "NetworkUtils.h"
#include "lassert.h"
#include <string.h>

ACIPv4ListWritePacket::ACIPv4ListWritePacket(const nonstd::span<const uint32_t> addresses)
    : ElementHeader(ElementHeader::ACIPv4List, addresses.size() * sizeof(uint32_t)),
      addresses{ addresses } {
}

void ACIPv4ListWritePacket::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(ElementHeader) + GetLength() <= raw_data->end);
    ElementHeader *dst = (ElementHeader *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(ElementHeader);
    memcpy(raw_data->current, addresses.data(), addresses.size() * sizeof(uint32_t));
    raw_data->current += addresses.size() * sizeof(uint32_t);
}
uint16_t ACIPv4ListWritePacket::GetTotalLength() const {
    return GetLength() + sizeof(ElementHeader);
}

void ACIPv4ListWritePacket::Log() const {
    log_i("ME ACIPv4List size:%zu, adrs:", addresses.size());
    for (size_t i = 0; i < addresses.size(); i++) {
        log_i("     #%zu, %s", i, IpToString(addresses[i]).c_str());
    }
}

ACIPv4ListReadPacket::ACIPv4ListReadPacket() : ElementHeader(ElementHeader::ACIPv4List, 0) {
}
bool ACIPv4ListReadPacket::Validate() const {
    return ElementHeader::GetElementType() == ElementHeader::ACIPv4List
        && GetLength() <= ACIPv4ListReadPacket::max_count * sizeof(uint32_t)
        && (GetLength() % sizeof(uint32_t)) == 0;
}

ACIPv4ListReadPacket *ACIPv4ListReadPacket::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ElementHeader) > raw_data->end) {
        return nullptr;
    }

    auto res = (ACIPv4ListReadPacket *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    if (raw_data->current + sizeof(ElementHeader) + res->GetLength() > raw_data->end) {
        return nullptr;
    }

    raw_data->current += sizeof(ElementHeader) + res->GetLength();
    return res;
}

size_t ACIPv4ListReadPacket::GetCount() const {
    return GetLength() / sizeof(uint32_t);
}

void ACIPv4ListReadPacket::Log() const {
    log_i("ME ACIPv4List size:%zu, adrs:", GetCount());
    for (size_t i = 0; i < GetCount(); i++) {
        log_i("     #%zu, %s", i, IpToString(addresses[i]).c_str());
    }
}

WritableACIPv4List::WritableACIPv4List(const nonstd::span<const uint32_t> addresses)
    : element{ addresses } {
}

void WritableACIPv4List::Serialize(RawData *raw_data) const {
    element.Serialize(raw_data);
}

void WritableACIPv4List::Log() const {
    element.Log();
}

bool ReadableACIPv4List::Deserialize(RawData *raw_data) {
    element = ACIPv4ListReadPacket::Deserialize(raw_data);
    is_present = element != nullptr;
    return is_present;
}

const ACIPv4ListReadPacket *const ReadableACIPv4List::Get() const {
    return element;
}

void ReadableACIPv4List::Log() const {
    ASSERT(element != nullptr);
    element->Log();
}

ElementHeader::ElementType ReadableACIPv4List::GetElementType() const {
    return ElementHeader::ACIPv4List;
}

bool ReadableACIPv4List::IsPresent() const {
    return is_present;
}