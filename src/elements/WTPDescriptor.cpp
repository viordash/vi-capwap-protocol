
#include "WTPDescriptor.h"
#include "lassert.h"
#include "logging.h"
#include <string.h>

WTPDescriptorHeader::WTPDescriptorHeader(uint8_t max_radios,
                                         uint8_t radios_in_use,
                                         uint8_t num_encrypt,
                                         uint16_t sub_items_size)
    : ElementHeader(ElementHeader::WTPDescriptor,
                    (sizeof(WTPDescriptorHeader) - sizeof(ElementHeader)) + sub_items_size),
      MaxRadios{ max_radios }, RadiosInUse{ radios_in_use }, NumEncrypt{ num_encrypt } {
}

bool WTPDescriptorHeader::Validate() const {
    static_assert(sizeof(WTPDescriptorHeader) == 7);
    return GetElementType() == ElementHeader::WTPDescriptor
        && GetLength() >= (sizeof(WTPDescriptorHeader) - sizeof(ElementHeader)) && NumEncrypt > 0;
}

void WTPDescriptorHeader::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(WTPDescriptor) <= raw_data->end);
    WTPDescriptorHeader *dst = (WTPDescriptorHeader *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(WTPDescriptorHeader);
}

WTPDescriptorHeader *WTPDescriptorHeader::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(WTPDescriptorHeader) > raw_data->end) {
        return nullptr;
    }

    auto res = (WTPDescriptorHeader *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }

    uint8_t *last = raw_data->current + sizeof(ElementHeader) + res->GetLength();
    if (last > raw_data->end) {
        return nullptr;
    }

    raw_data->current += sizeof(WTPDescriptorHeader);
    return res;
}

EncryptionSubElement::EncryptionSubElement(uint16_t encryption_capabilities)
    : WBID{ WBIDType::IEEE_80211 }, Resvd{ 0 }, EncryptionCapabilities{ encryption_capabilities } {
}

bool EncryptionSubElement::Validate() const {
    static_assert(sizeof(EncryptionSubElement) == 3);
    return WBID == WBIDType::IEEE_80211;
}

void EncryptionSubElement::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(EncryptionSubElement) <= raw_data->end);
    EncryptionSubElement *dst = (EncryptionSubElement *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(EncryptionSubElement);
}

EncryptionSubElement *EncryptionSubElement::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(EncryptionSubElement) > raw_data->end) {
        return nullptr;
    }

    auto res = (EncryptionSubElement *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }

    raw_data->current += sizeof(EncryptionSubElement);
    return res;
}

DescriptorSubElementHeader::DescriptorSubElementHeader(uint32_t vendor_identifier,
                                                       Type type,
                                                       uint16_t length)
    : vendor_id{ vendor_identifier }, type{ type }, length{ length } {
}

uint32_t DescriptorSubElementHeader::GetVendorIdentifier() const {
    return vendor_id.Get();
}

DescriptorSubElementHeader::Type DescriptorSubElementHeader::GetType() const {
    return type;
}
uint16_t DescriptorSubElementHeader::GetLength() const {
    return length.Get();
}
bool DescriptorSubElementHeader::Validate() const {
    static_assert(sizeof(DescriptorSubElementHeader) == 8);
    switch (type) {
        case Type::HardwareVersion:
        case Type::ActiveSoftwareVersion:
        case Type::BootVersion:
        case Type::OtherSoftwareVersion:
            return true;

        default:
            return false;
    }
}
void DescriptorSubElementHeader::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(DescriptorSubElementHeader) <= raw_data->end);
    DescriptorSubElementHeader *dst = (DescriptorSubElementHeader *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(DescriptorSubElementHeader);
}

DescriptorSubElementHeader *DescriptorSubElementHeader::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(DescriptorSubElementHeader) > raw_data->end) {
        return nullptr;
    }

    auto res = (DescriptorSubElementHeader *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }

    raw_data->current += sizeof(DescriptorSubElementHeader);
    return res;
}

uint16_t WritableWTPDescriptor::GetSubElementsSize(
    const nonstd::span<const EncryptionSubElement> &encr_items,
    const nonstd::span<const SubElement> &desc_items) {
    uint16_t size = 0;
    for (const auto &elem : encr_items) {
        size += sizeof(elem);
    }
    for (const auto &elem : desc_items) {
        size += sizeof(DescriptorSubElementHeader) + elem.header.GetLength();
    }
    return size;
}

WritableWTPDescriptor::WritableWTPDescriptor(
    uint8_t max_radios,
    uint8_t radios_in_use,
    const nonstd::span<const EncryptionSubElement> &encr_items,
    const nonstd::span<const SubElement> &desc_items)
    : header{ max_radios,
              radios_in_use,
              (uint8_t)encr_items.size(),
              GetSubElementsSize(encr_items, desc_items) },
      encr_items(encr_items), desc_items{ desc_items } {
}

void WritableWTPDescriptor::Serialize(RawData *raw_data) const {
    ASSERT(encr_items.size() <= ReadableWTPDescriptor::encr_max_count);
    ASSERT(desc_items.size() <= ReadableWTPDescriptor::desc_max_count);

    header.Serialize(raw_data);

    for (const auto &elem : encr_items) {
        elem.Serialize(raw_data);
    }

    for (const auto &elem : desc_items) {
        elem.header.Serialize(raw_data);
        memcpy(raw_data->current, elem.utf8_value, elem.header.GetLength());
        raw_data->current += elem.header.GetLength();
    }
}
uint16_t WritableWTPDescriptor::GetTotalLength() const {
    return header.GetLength() + sizeof(ElementHeader);
}

void WritableWTPDescriptor::Log() const {
    log_i("ME WTPDescriptor MaxRadios:%u, RadiosInUse:%u, NumEncrypt:%lu, Sub-Elements count:%lu",
          header.MaxRadios,
          header.RadiosInUse,
          encr_items.size(),
          desc_items.size());
    for (size_t i = 0; i < encr_items.size(); i++) {
        log_i("    Encr S-E #%lu: WBID:%u, Encr-Caps:%u",
              i,
              encr_items[i].WBID,
              encr_items[i].EncryptionCapabilities);
    }
    for (size_t i = 0; i < desc_items.size(); i++) {
        log_i("    S-E #%lu: VendorId:%u, Type:0x%04X, Value:%.*s",
              i,
              desc_items[i].header.GetVendorIdentifier(),
              desc_items[i].header.GetType(),
              desc_items[i].header.GetLength(),
              desc_items[i].utf8_value);
    }
}

ReadableWTPDescriptor::ReadableWTPDescriptor() : header{}, desc_count{ 0 } {
}

bool ReadableWTPDescriptor::Deserialize(RawData *raw_data) {
    header = WTPDescriptorHeader::Deserialize(raw_data);
    if (header == nullptr) {
        return false;
    }

    size_t encr_count = 0;
    const uint8_t *end = raw_data->current + header->GetLength()
                       - (sizeof(WTPDescriptorHeader) - sizeof(ElementHeader));
    while (raw_data->current < end && encr_count < header->NumEncrypt) {
        auto sub_element = EncryptionSubElement::Deserialize(raw_data);
        if (sub_element == nullptr) {
            return false;
        }
        if (encr_count >= encr_max_count) {
            log_e("ReadableWTPDescriptor::Deserialize encr sub elements count "
                  "exceeds max");
            return false;
        }
        encr_items[encr_count] = sub_element;
        encr_count++;
    }

    if (encr_count != header->NumEncrypt) {
        log_e("ReadableWTPDescriptor::Deserialize encr sub elements count "
              "not equal NumEncrypt");
        return false;
    }

    desc_count = 0;
    while (raw_data->current < end) {
        auto sub_element_header = DescriptorSubElementHeader::Deserialize(raw_data);
        if (sub_element_header == nullptr) {
            return false;
        }
        if (desc_count >= desc_max_count) {
            log_e("ReadableWTPDescriptor::Deserialize decr sub elements count "
                  "exceeds");
            return false;
        }
        desc_items[desc_count] = sub_element_header;
        desc_count++;
        raw_data->current += sub_element_header->GetLength();
    }

    if (raw_data->current > end) {
        log_e("ReadableWTPDescriptor::Deserialize length negative");
        return false;
    }
    return true;
}

const nonstd::span<const EncryptionSubElement *> ReadableWTPDescriptor::GetEncryptions() {
    nonstd::span span(encr_items.begin(), header->NumEncrypt);
    return span;
}

const nonstd::span<const DescriptorSubElementHeader *> ReadableWTPDescriptor::GetDescriptors() {
    nonstd::span span(desc_items.begin(), desc_count);
    return span;
}

void ReadableWTPDescriptor::Log() const {
    log_i("ME WTPDescriptor MaxRadios:%u, RadiosInUse:%u, NumEncrypt:%u, Sub-Elements count:%lu",
          header->MaxRadios,
          header->RadiosInUse,
          header->NumEncrypt,
          desc_count);
    for (size_t i = 0; i < header->NumEncrypt; i++) {
        log_i("    Encr S-E #%lu: WBID:%u, Encr-Caps:%u",
              i,
              encr_items[i]->WBID,
              encr_items[i]->EncryptionCapabilities);
    }
    for (size_t i = 0; i < desc_count; i++) {
        log_i("    S-E #%lu: VendorId:%u, Type:0x%04X, Value:%.*s",
              i,
              desc_items[i]->GetVendorIdentifier(),
              desc_items[i]->GetType(),
              desc_items[i]->GetLength(),
              desc_items[i]->utf8_value);
    }
}