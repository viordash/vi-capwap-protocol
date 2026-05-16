
#include "VendorSpecificPayload.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <cstring>
#include <string.h>

VendorSpecificPayload::VendorSpecificPayload(uint32_t vendor_identifier,
                                             uint16_t element_id,
                                             uint16_t data_size)
    : ElementHeader(ElementHeader::VendorSpecificPayload,
                    (sizeof(VendorSpecificPayload) - sizeof(ElementHeader)) + data_size),
      vendor_id{ vendor_identifier }, element_id{ element_id } {
}

uint32_t VendorSpecificPayload::GetVendorIdentifier() const {
    return vendor_id.Get();
}

uint32_t VendorSpecificPayload::GetElementId() const {
    return element_id.Get();
}

bool VendorSpecificPayload::Validate() const {
    static_assert(sizeof(VendorSpecificPayload) == 10);
    return GetElementType() == ElementHeader::VendorSpecificPayload
        && GetLength() >= (sizeof(VendorSpecificPayload) - sizeof(ElementHeader))
        && GetLength() <= ReadableVendorSpecificPayloadArray::max_data_size
                              + (sizeof(VendorSpecificPayload) - sizeof(ElementHeader));
}

uint16_t VendorSpecificPayload::GetTotalLength() const {
    return GetLength() + sizeof(ElementHeader);
}

WritableVendorSpecificPayloadArray::WritableVendorSpecificPayloadArray(
    const nonstd::span<const Item> &items)
    : items{ items.begin(), items.end() } {
    static_assert(sizeof(Item::header) == 10);
    ASSERT(items.size() <= ReadableVendorSpecificPayloadArray::max_count);
}

WritableVendorSpecificPayloadArray::WritableVendorSpecificPayloadArray() {
    items.reserve(ReadableVendorSpecificPayloadArray::max_count);
}

void WritableVendorSpecificPayloadArray::Add(uint32_t vendor_identifier,
                                             uint16_t element_id,
                                             std::vector<char> val) {
    ASSERT(items.size() + 1 <= ReadableVendorSpecificPayloadArray::max_count);

    items.emplace_back(vendor_identifier, element_id, std::move(val));
}

void WritableVendorSpecificPayloadArray::Add(uint32_t vendor_identifier,
                                             uint16_t element_id,
                                             const std::string_view str) {
    ASSERT(items.size() + 1 <= ReadableVendorSpecificPayloadArray::max_count);

    std::vector<char> vec(str.begin(), str.end());

    items.emplace_back(vendor_identifier, element_id, std::move(vec));
}

bool WritableVendorSpecificPayloadArray::Empty() const {
    return items.empty();
}

void WritableVendorSpecificPayloadArray::Clear() {
    items.clear();
}

void WritableVendorSpecificPayloadArray::Serialize(RawData *raw_data) const {
    for (const auto &item : items) {
        ASSERT(raw_data->current + sizeof(item.header) <= raw_data->end);
        std::memcpy(raw_data->current, &item.header, sizeof(item.header));
        raw_data->current += sizeof(item.header);
        uint16_t data_size =
            item.header.GetLength() - (sizeof(item.header) - sizeof(ElementHeader));
        std::memcpy(raw_data->current, item.value.data(), data_size);
        raw_data->current += data_size;
    }
}
uint16_t WritableVendorSpecificPayloadArray::GetTotalLength() const {
    uint16_t size = 0;
    for (const auto &item : items) {
        size += item.header.GetTotalLength();
    }
    return size;
}

void WritableVendorSpecificPayloadArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME VendorSpecificPayload  #{} VendorId:{}, ElementId:{}, Value:{}",
              i,
              items[i].header.GetVendorIdentifier(),
              items[i].header.GetElementId(),
              std::string_view((const char *)items[i].value.data(),
                               items[i].header.GetLength()
                                   - (sizeof(VendorSpecificPayload) - sizeof(ElementHeader))));
    }
}

ElementHeader::ElementType WritableVendorSpecificPayloadArray::GetElementType() const {
    return ElementHeader::VendorSpecificPayload;
}

ReadableVendorSpecificPayloadArray::ReadableVendorSpecificPayloadArray() : count{ 0 } {
}

bool ReadableVendorSpecificPayloadArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableVendorSpecificPayloadArray::Deserialize elements count exceeds");
        return false;
    }

    if (raw_data->current + sizeof(VendorSpecificPayload) > raw_data->end) {
        return false;
    }

    auto item = (ReadableVendorSpecificPayloadArray::Item *)raw_data->current;
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

nonstd::span<const ReadableVendorSpecificPayloadArray::Item *const>
ReadableVendorSpecificPayloadArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableVendorSpecificPayloadArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i(
            "ME VendorSpecificPayload  #{} VendorId:{}, ElementId:{}, Value:{}",
            i,
            items[i]->GetVendorIdentifier(),
            items[i]->GetElementId(),
            std::string_view((char *)items[i]->value,
                             items[i]->GetLength()
                                 - (sizeof(VendorSpecificPayload) - sizeof(ElementHeader))));
    }
}

ElementHeader::ElementType ReadableVendorSpecificPayloadArray::GetElementType() const {
    return ElementHeader::VendorSpecificPayload;
}

bool ReadableVendorSpecificPayloadArray::IsPresent() const {
    return count > 0;
}