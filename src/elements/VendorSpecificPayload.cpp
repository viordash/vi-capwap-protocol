
#include "VendorSpecificPayload.h"
#include "lassert.h"
#include "logging.h"
#include <algorithm>
#include <string.h>

WritableVendorSpecificPayloadArray VendorSpecificPayload::Dummy;

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

void VendorSpecificPayload::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(VendorSpecificPayload) <= raw_data->end);
    VendorSpecificPayload *dst = (VendorSpecificPayload *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(VendorSpecificPayload);
}

VendorSpecificPayload *VendorSpecificPayload::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(VendorSpecificPayload) > raw_data->end) {
        return nullptr;
    }

    auto res = (VendorSpecificPayload *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }

    uint8_t *last = raw_data->current + sizeof(ElementHeader) + res->GetLength();
    if (last > raw_data->end) {
        return nullptr;
    }

    raw_data->current = last;
    return res;
}

WritableVendorSpecificPayloadArray::WritableVendorSpecificPayloadArray(
    const nonstd::span<const Item> &items)
    : items{ items.begin(), items.end() } {
    ASSERT(items.size() <= ReadableVendorSpecificPayloadArray::max_count);
}

WritableVendorSpecificPayloadArray::WritableVendorSpecificPayloadArray() {
    items.reserve(ReadableVendorSpecificPayloadArray::max_count);
}

void WritableVendorSpecificPayloadArray::Add(uint32_t vendor_identifier,
                                             uint16_t element_id,
                                             std::vector<char> val) {
    ASSERT(items.size() + 1 <= ReadableVendorSpecificPayloadArray::max_count);

    auto it_exists = std::find_if(
        items.begin(),
        items.end(),
        [&vendor_identifier, &element_id](const WritableVendorSpecificPayloadArray::Item &item) {
            return item.header.GetVendorIdentifier() == vendor_identifier
                && item.header.GetElementId() == element_id;
        });

    if (it_exists != items.end()) {
        *it_exists = WritableVendorSpecificPayloadArray::Item{ vendor_identifier,
                                                               element_id,
                                                               std::move(val) };
        log_i("VendorSpecificPayload: replace vendor_identifier: %u, element_id: %u",
              vendor_identifier,
              element_id);
    } else {
        items.emplace_back(vendor_identifier, element_id, std::move(val));
    }
}

void WritableVendorSpecificPayloadArray::Add(uint32_t vendor_identifier,
                                             uint16_t element_id,
                                             const std::string_view str) {
    ASSERT(items.size() + 1 <= ReadableVendorSpecificPayloadArray::max_count);

    std::vector<char> vec(str.begin(), str.end());

    auto it_exists = std::find_if(
        items.begin(),
        items.end(),
        [&vendor_identifier, &element_id](const WritableVendorSpecificPayloadArray::Item &item) {
            return item.header.GetVendorIdentifier() == vendor_identifier
                && item.header.GetElementId() == element_id;
        });

    if (it_exists != items.end()) {
        *it_exists = WritableVendorSpecificPayloadArray::Item{ vendor_identifier,
                                                               element_id,
                                                               std::move(vec) };
        log_i("VendorSpecificPayload: replace vendor_identifier: %u, element_id: %u",
              vendor_identifier,
              element_id);
    } else {
        items.emplace_back(vendor_identifier, element_id, std::move(vec));
    }
}

bool WritableVendorSpecificPayloadArray::Empty() const {
    return items.empty();
}

void WritableVendorSpecificPayloadArray::Clear() {
    items.clear();
}

void WritableVendorSpecificPayloadArray::Serialize(RawData *raw_data) const {
    for (const auto &elem : items) {
        elem.header.Serialize(raw_data);
        uint16_t data_size =
            elem.header.GetLength() - (sizeof(VendorSpecificPayload) - sizeof(ElementHeader));
        memcpy(raw_data->current, elem.value.data(), data_size);
        raw_data->current += data_size;
    }
}
uint16_t WritableVendorSpecificPayloadArray::GetTotalLength() const {
    uint16_t size = 0;
    for (const auto &elem : items) {
        size += elem.header.GetTotalLength();
    }
    return size;
}

void WritableVendorSpecificPayloadArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME VendorSpecificPayload  #%lu VendorId:%u, ElementId:%u, Value:%.*s",
              i,
              items[i].header.GetVendorIdentifier(),
              items[i].header.GetElementId(),
              (int)(items[i].header.GetLength()
                    - (sizeof(VendorSpecificPayload) - sizeof(ElementHeader))),
              items[i].value.data());
    }
}

ReadableVendorSpecificPayloadArray::ReadableVendorSpecificPayloadArray() : count{ 0 } {
}

bool ReadableVendorSpecificPayloadArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableVendorSpecificPayloadArray::Deserialize elements count exceeds");
        return false;
    }

    auto vendor_payload = VendorSpecificPayload::Deserialize(raw_data);
    if (vendor_payload == nullptr) {
        return false;
    }
    items[count] = vendor_payload;
    count++;
    return true;
}

nonstd::span<const VendorSpecificPayload *const> ReadableVendorSpecificPayloadArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableVendorSpecificPayloadArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i(
            "ME VendorSpecificPayload  #%lu VendorId:%u, ElementId:%u, Value:%.*s",
            i,
            items[i]->GetVendorIdentifier(),
            items[i]->GetElementId(),
            (int)(items[i]->GetLength() - (sizeof(VendorSpecificPayload) - sizeof(ElementHeader))),
            (char *)items[i]->value);
    }
}
