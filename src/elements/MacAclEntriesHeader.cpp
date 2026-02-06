
#include "MacAclEntriesHeader.h"
#include "lassert.h"
#include "logging.h"
#include <arpa/inet.h>
#include <string.h>

MacAclEntriesHeader::MacAclEntriesHeader(ElementType element_type,
                                         uint16_t element_length,
                                         uint8_t num_of_entries)
    : ElementHeader(element_type, element_length), NumOfEntries{ num_of_entries } {
}

MacAclEntriesHeader::MacAclEntriesHeader(ElementType element_type,
                                         const nonstd::span<const WritableMacAddress> &entries)
    : ElementHeader(element_type, CalcEntriesSize(entries)),
      NumOfEntries{ (uint8_t)entries.size() } {
}

bool MacAclEntriesHeader::Validate() const {
    static_assert(sizeof(MacAclEntriesHeader) == 5);
    if (GetLength() < (sizeof(MacAclEntriesHeader) - sizeof(ElementHeader))) {
        return false;
    }
    if (NumOfEntries == 0) {
        return false;
    }
    return true;
}

uint16_t
MacAclEntriesHeader::CalcEntriesSize(const nonstd::span<const WritableMacAddress> &entries) {
    ASSERT(entries.size() <= MacAclEntriesHeader::max_count);
    uint16_t size = sizeof(MacAclEntriesHeader::NumOfEntries);
    for (auto &entry : entries) {
        size += sizeof(ReadableMacAddress::Length);

        ASSERT(entry.MACAddress.size() <= ReadableMacAddress::max_length);
        size += entry.MACAddress.size();
    }
    return size;
}

void MacAclEntriesHeader::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(MacAclEntriesHeader) + GetLength() <= raw_data->end);
    MacAclEntriesHeader *dst = (MacAclEntriesHeader *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(MacAclEntriesHeader);
}

MacAclEntriesHeader *MacAclEntriesHeader::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(MacAclEntriesHeader) > raw_data->end) {
        return nullptr;
    }

    auto res = (MacAclEntriesHeader *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }

    uint8_t *last = raw_data->current + sizeof(ElementHeader) + res->GetLength();
    if (last > raw_data->end) {
        return nullptr;
    }

    raw_data->current += sizeof(MacAclEntriesHeader);
    return res;
}