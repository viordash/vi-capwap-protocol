
#include "ACIPv4List.h"
#include "lassert.h"
#include "logging.h"
#include <arpa/inet.h>
#include <string.h>

WritableACIPv4List::WritableACIPv4List(const nonstd::span<const uint32_t> &addresses)
    : ElementHeader(ElementHeader::ACIPv4List, addresses.size() * sizeof(uint32_t)),
      addresses{ addresses } {
}

void WritableACIPv4List::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(ElementHeader) + GetLength() <= raw_data->end);
    ElementHeader *dst = (ElementHeader *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(ElementHeader);
    memcpy(raw_data->current, addresses.data(), addresses.size() * sizeof(uint32_t));
    raw_data->current += addresses.size() * sizeof(uint32_t);
}
uint16_t WritableACIPv4List::GetTotalLength() const {
    return GetLength() + sizeof(ElementHeader);
}

void WritableACIPv4List::Log() const {
    log_i("ME ACIPv4List size:%lu, adrs:", addresses.size());
    for (size_t i = 0; i < addresses.size(); i++) {
        struct in_addr paddr;
        paddr.s_addr = addresses[i];
        log_i("     #%lu, %s", i, inet_ntoa(paddr));
    }
}

ReadableACIPv4List::ReadableACIPv4List() : ElementHeader(ElementHeader::ACIPv4List, 0) {
}
bool ReadableACIPv4List::Validate() const {
    return ElementHeader::GetElementType() == ElementHeader::ACIPv4List
        && GetLength() <= ReadableACIPv4List::max_count * sizeof(uint32_t)
        && (GetLength() % sizeof(uint32_t)) == 0;
}

ReadableACIPv4List *ReadableACIPv4List::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ElementHeader) > raw_data->end) {
        return nullptr;
    }

    auto res = (ReadableACIPv4List *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    if (raw_data->current + sizeof(ElementHeader) + res->GetLength() > raw_data->end) {
        return nullptr;
    }

    raw_data->current += sizeof(ElementHeader) + res->GetLength();
    return res;
}

size_t ReadableACIPv4List::GetCount() const {
    return GetLength() / sizeof(uint32_t);
}

void ReadableACIPv4List::Log() const {
    log_i("ME ACIPv4List size:%lu, adrs:", GetCount());
    for (size_t i = 0; i < GetCount(); i++) {
        struct in_addr paddr;
        paddr.s_addr = addresses[i];
        log_i("     #%lu, %s", i, inet_ntoa(paddr));
    }
}
