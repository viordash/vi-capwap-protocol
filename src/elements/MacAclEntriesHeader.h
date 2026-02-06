#pragma once
#include "elements/ElementHeader.h"
#include "elements/MacAddress.h"
#include "span.hpp"

struct __attribute__((packed)) MacAclEntriesHeader : ElementHeader {
    static const size_t max_count = 255;
    uint8_t NumOfEntries;

    MacAclEntriesHeader(const MacAclEntriesHeader &) = delete;
    MacAclEntriesHeader(ElementType element_type, uint16_t element_length, uint8_t num_of_entries);
    MacAclEntriesHeader(ElementType element_type,
                        const nonstd::span<const WritableMacAddress> &entries);
    static uint16_t CalcEntriesSize(const nonstd::span<const WritableMacAddress> &entries);
    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static MacAclEntriesHeader *Deserialize(RawData *raw_data);
};
