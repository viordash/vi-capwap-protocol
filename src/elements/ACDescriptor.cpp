
#include "ACDescriptor.h"
#include "lassert.h"
#include "logging.h"
#include <string.h>

ACDescriptorHeader::ACDescriptorHeader(uint16_t stations,
                                       uint16_t limit,
                                       uint16_t active_wtp,
                                       uint16_t max_wtp,
                                       bool x509_cert,
                                       bool preshared_secret,
                                       RMACField rmac,
                                       bool clear_data_chn,
                                       bool dtls_data_chn,
                                       uint16_t sub_elements_size)
    : ElementHeader(ElementHeader::ACDescriptor,
                    (sizeof(ACDescriptorHeader) - sizeof(ElementHeader)) + sub_elements_size),
      stations{ stations }, limit{ limit }, active_wtp{ active_wtp }, max_wtp{ max_wtp }, r0{ 0 },
      x509_cert{ x509_cert }, preshared_secret{ preshared_secret }, reserved{ 0 }, rmac{ rmac },
      reserved1{ 0 }, r1{ 0 }, clear_data_chn{ clear_data_chn }, dtls_data_chn{ dtls_data_chn },
      reserved2{ 0 } {
}

bool ACDescriptorHeader::Validate() const {
    static_assert(sizeof(ACDescriptorHeader) == 16);
    if (GetElementType() != ElementHeader::ACDescriptor) {
        return false;
    }
    if (GetLength() < (sizeof(ACDescriptorHeader) - sizeof(ElementHeader))) {
        return false;
    }
    switch (rmac) {
        case RMACField::NotSupported:
        case RMACField::Supported:
            return true;
        default:
            return false;
    }
}

void ACDescriptorHeader::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(ACDescriptorHeader) + GetLength() <= raw_data->end);
    ACDescriptorHeader *dst = (ACDescriptorHeader *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(ACDescriptorHeader);
}

ACDescriptorHeader *ACDescriptorHeader::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ACDescriptorHeader) > raw_data->end) {
        return nullptr;
    }

    auto res = (ACDescriptorHeader *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }

    uint8_t *last = raw_data->current + sizeof(ElementHeader) + res->GetLength();
    if (last > raw_data->end) {
        return nullptr;
    }

    raw_data->current += sizeof(ACDescriptorHeader);
    return res;
}

uint16_t ACDescriptorHeader::GetStations() const {
    return stations.Get();
}

uint16_t ACDescriptorHeader::GetLimit() const {
    return limit.Get();
}

uint16_t ACDescriptorHeader::GetActiveWTPs() const {
    return active_wtp.Get();
}

uint16_t ACDescriptorHeader::GetMaxWTPs() const {
    return max_wtp.Get();
}

bool ACDescriptorHeader::X509Cert() const {
    return x509_cert != 0;
}

bool ACDescriptorHeader::PresharedSecret() const {
    return preshared_secret != 0;
}

ACDescriptorHeader::RMACField ACDescriptorHeader::GetRMAC() const {
    return rmac;
}

bool ACDescriptorHeader::ClearDataChannel() const {
    return clear_data_chn != 0;
}

bool ACDescriptorHeader::DTLSDataChannel() const {
    return dtls_data_chn != 0;
}

ACInformationSubElementHeader::ACInformationSubElementHeader(uint32_t vendor_identifier,
                                                             Type type,
                                                             uint16_t length)
    : vendor_id{ vendor_identifier }, type{ type }, length{ length } {
}

uint32_t ACInformationSubElementHeader::GetVendorIdentifier() const {
    return vendor_id.Get();
}

ACInformationSubElementHeader::Type ACInformationSubElementHeader::GetType() const {
    return type;
}
uint16_t ACInformationSubElementHeader::GetLength() const {
    return length.Get();
}
bool ACInformationSubElementHeader::Validate() const {
    static_assert(sizeof(ACInformationSubElementHeader) == 8);
    return true;
}

uint16_t WritableACDescriptor::GetSubElementsSize(const nonstd::span<const SubElement> &elements) {
    uint16_t size = 0;
    for (const auto &elem : elements) {
        size += sizeof(ACInformationSubElementHeader) + elem.header.GetLength();
    }
    return size;
}

WritableACDescriptor::WritableACDescriptor(uint16_t stations,
                                           uint16_t limit,
                                           uint16_t active_wtp,
                                           uint16_t max_wtp,
                                           bool x509_cert,
                                           bool preshared_secret,
                                           ACDescriptorHeader::RMACField rmac,
                                           bool clear_data_chn,
                                           bool dtls_data_chn,
                                           const nonstd::span<const SubElement> &elements)
    : header{ stations,         limit, active_wtp,     max_wtp,       x509_cert,
              preshared_secret, rmac,  clear_data_chn, dtls_data_chn, GetSubElementsSize(elements) },
      sub_elements(elements) {
}

void WritableACDescriptor::Serialize(RawData *raw_data) const {
    ASSERT(sub_elements.size() <= ReadableACDescriptor::max_count);

    header.Serialize(raw_data);

    for (const auto &elem : sub_elements) {
        ACInformationSubElementHeader *sub_element =
            (ACInformationSubElementHeader *)raw_data->current;
        *sub_element = elem.header;

        memcpy(sub_element->data, elem.data, elem.header.GetLength());
        raw_data->current += sizeof(ACInformationSubElementHeader) + elem.header.GetLength();
    }
}
uint16_t WritableACDescriptor::GetTotalLength() const {
    return header.GetLength() + sizeof(ElementHeader);
}

void WritableACDescriptor::Log() const {
    log_i("ME ACDescriptor Stations:%u, Limit:%u, Active WTPs:%u, Max WTPs:%u, X.509:%u, "
          "Pre-shared secret:%u, RMAC:%u, "
          "Clear Text Data Channel Supported:%u, DTLS-Enabled Data Channel Supported:%u"
          "Sub-Elements count:%lu",
          header.GetStations(),
          header.GetLimit(),
          header.GetActiveWTPs(),
          header.GetMaxWTPs(),
          header.X509Cert(),
          header.PresharedSecret(),
          header.GetRMAC(),
          header.ClearDataChannel(),
          header.DTLSDataChannel(),
          sub_elements.size());

    for (size_t i = 0; i < sub_elements.size(); i++) {
        log_i("    S-E #%lu: VendorId:%u, Type:0x%04X, Data:%.*s",
              i,
              sub_elements[i].header.GetVendorIdentifier(),
              sub_elements[i].header.GetType(),
              sub_elements[i].header.GetLength(),
              sub_elements[i].data);
    }
}

ReadableACDescriptor::ReadableACDescriptor() : header{}, count{ 0 } {
}

bool ReadableACDescriptor::Deserialize(RawData *raw_data) {
    header = ACDescriptorHeader::Deserialize(raw_data);
    if (header == nullptr) {
        return false;
    }

    const uint8_t *end = raw_data->current + header->GetLength()
                       - (sizeof(ACDescriptorHeader) - sizeof(ElementHeader));

    while (raw_data->current < end) {
        if (count >= max_count) {
            log_e("ReadableACDescriptor::Deserialize decr sub elements count "
                  "exceeds");
            return false;
        }

        auto sub_element_header = (ACInformationSubElementHeader *)raw_data->current;
        if (!sub_element_header->Validate()) {
            return false;
        }

        items[count] = sub_element_header;
        count++;
        raw_data->current +=
            sizeof(ACInformationSubElementHeader) + sub_element_header->GetLength();
    }

    if (raw_data->current > end) {
        log_e("ReadableACDescriptor::Deserialize length negative");
        return false;
    }
    return true;
}

nonstd::span<const ACInformationSubElementHeader *const> ReadableACDescriptor::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableACDescriptor::Log() const {
    log_i("ME ACDescriptor Stations:%u, Limit:%u, Active WTPs:%u, Max WTPs:%u, X.509:%u, "
          "Pre-shared secret:%u, RMAC:%u, "
          "Clear Text Data Channel Supported:%u, DTLS-Enabled Data Channel Supported:%u, "
          "Sub-Elements count:%lu",
          header->GetStations(),
          header->GetLimit(),
          header->GetActiveWTPs(),
          header->GetMaxWTPs(),
          header->X509Cert(),
          header->PresharedSecret(),
          header->GetRMAC(),
          header->ClearDataChannel(),
          header->DTLSDataChannel(),
          count);

    for (size_t i = 0; i < count; i++) {
        log_i("    S-E #%lu: VendorId:%u, Type:0x%04X, Data:%.*s",
              i,
              items[i]->GetVendorIdentifier(),
              items[i]->GetType(),
              items[i]->GetLength(),
              items[i]->data);
    }
}
