
#include "WTPBoardData.h"
#include "lassert.h"
#include "logging.h"
#include <string.h>

WTPBoardDataHeader::WTPBoardDataHeader(uint32_t vendor_identifier, uint16_t sub_elements_size)
    : ElementHeader(ElementHeader::WTPBoardData,
                    (sizeof(WTPBoardDataHeader) - sizeof(ElementHeader)) + sub_elements_size),
      vendor_id{ vendor_identifier } {
}

uint32_t WTPBoardDataHeader::GetVendorIdentifier() const {
    return vendor_id.Get();
}

bool WTPBoardDataHeader::Validate() const {
    static_assert(sizeof(WTPBoardDataHeader) == 8);
    return GetElementType() == ElementHeader::WTPBoardData
        && GetLength() >= (sizeof(WTPBoardDataHeader) - sizeof(ElementHeader))
        && GetVendorIdentifier() != 0;
}

void WTPBoardDataHeader::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(WTPBoardDataHeader) <= raw_data->end);
    WTPBoardDataHeader *dst = (WTPBoardDataHeader *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(WTPBoardDataHeader);
}

WTPBoardDataHeader *WTPBoardDataHeader::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(WTPBoardDataHeader) > raw_data->end) {
        return nullptr;
    }

    auto res = (WTPBoardDataHeader *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }

    uint8_t *last = raw_data->current + sizeof(ElementHeader) + res->GetLength();
    if (last > raw_data->end) {
        return nullptr;
    }

    raw_data->current += sizeof(WTPBoardDataHeader);
    return res;
}

BoardDataSubElementHeader::BoardDataSubElementHeader(Type type, uint16_t length)
    : type{ type }, length{ length } {
}

BoardDataSubElementHeader::Type BoardDataSubElementHeader::GetType() const {
    return type;
}

uint16_t BoardDataSubElementHeader::GetLength() const {
    return length.Get();
}

bool BoardDataSubElementHeader::Validate() const {
    static_assert(sizeof(BoardDataSubElementHeader) == 4);
    return type >= WTPModelNumber && type <= BaseMACAddress && GetLength() < 1024;
}

void BoardDataSubElementHeader::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(BoardDataSubElementHeader) + GetLength() <= raw_data->end);
    BoardDataSubElementHeader *dst = (BoardDataSubElementHeader *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(BoardDataSubElementHeader);
}

BoardDataSubElementHeader *BoardDataSubElementHeader::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(BoardDataSubElementHeader) > raw_data->end) {
        return nullptr;
    }

    auto res = (BoardDataSubElementHeader *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }

    raw_data->current += sizeof(BoardDataSubElementHeader);
    return res;
}

uint16_t WritableWTPBoardData::GetSubElementsSize(const nonstd::span<const SubElement> &elements) {
    uint16_t size = 0;
    for (const auto &elem : elements) {
        size += sizeof(BoardDataSubElementHeader) + elem.header.GetLength();
    }
    return size;
}

WritableWTPBoardData::WritableWTPBoardData(const uint32_t vendor_identifier,
                                           const nonstd::span<const SubElement> &items)
    : header{ vendor_identifier, GetSubElementsSize(items) }, items(items) {
}

void WritableWTPBoardData::Serialize(RawData *raw_data) const {
    ASSERT(items.size() <= ReadableWTPBoardData::max_count);

    header.Serialize(raw_data);
    for (const auto &elem : items) {
        elem.header.Serialize(raw_data);
        memcpy(raw_data->current, elem.value, elem.header.GetLength());
        raw_data->current += elem.header.GetLength();
    }
}
uint16_t WritableWTPBoardData::GetTotalLength() const {
    return header.GetLength() + sizeof(ElementHeader);
}

void WritableWTPBoardData::Log() const {
    log_i("ME WTPBoardData VendorId:%u, Sub-Elements count:%lu",
          header.GetVendorIdentifier(),
          items.size());
    for (size_t i = 0; i < items.size(); i++) {
        log_i("    S-E #%lu: Type:0x%04X, Value:%.*s",
              i,
              items[i].header.GetType(),
              items[i].header.GetLength(),
              (char *)items[i].value);
    }
}

ReadableWTPBoardData::ReadableWTPBoardData() : header{}, count{ 0 } {
}

bool ReadableWTPBoardData::Deserialize(RawData *raw_data) {
    header = WTPBoardDataHeader::Deserialize(raw_data);
    if (header == nullptr) {
        return false;
    }

    const uint8_t *end = raw_data->current + header->GetLength()
                       - (sizeof(WTPBoardDataHeader) - sizeof(ElementHeader));
    while (raw_data->current < end) {
        auto sub_element_header = BoardDataSubElementHeader::Deserialize(raw_data);
        if (sub_element_header == nullptr) {
            return false;
        }
        if (count >= max_count) {
            log_e("ReadableWTPBoardData::Deserialize sub elements count exceeds");
            return false;
        }
        items[count] = sub_element_header;
        count++;
        raw_data->current += sub_element_header->GetLength();
    }
    if (raw_data->current > end) {
        log_e("ReadableWTPBoardData::Deserialize length negative");
        return false;
    }
    return true;
}

nonstd::span<const BoardDataSubElementHeader *const> ReadableWTPBoardData::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableWTPBoardData::Log() const {
    log_i("ME WTPBoardData VendorId:%u, Sub-Elements count:%lu",
          header->GetVendorIdentifier(),
          count);
    for (size_t i = 0; i < count; i++) {
        log_i("    S-E #%lu: Type:0x%04X, Value:%.*s",
              i,
              items[i]->GetType(),
              items[i]->GetLength(),
              (char *)items[i]->value);
    }
}