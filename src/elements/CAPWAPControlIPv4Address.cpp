#include "CAPWAPControlIPv4Address.h"
#include "network_utils.h"
#include "lassert.h"
#include "logging.h"

CAPWAPControlIPv4Address::CAPWAPControlIPv4Address(uint32_t ipaddress, uint16_t wtp_count)
    : ElementHeader(ElementHeader::CAPWAPControlIPv4Address,
                    sizeof(CAPWAPControlIPv4Address) - sizeof(ElementHeader)),
      ipaddress{ ipaddress }, wtp_count{ wtp_count } {
}
bool CAPWAPControlIPv4Address::Validate() const {
    static_assert(sizeof(CAPWAPControlIPv4Address) == 10);
    if (ElementHeader::GetElementType() != ElementHeader::CAPWAPControlIPv4Address) {
        return false;
    }
    if (ElementHeader::GetLength() != (sizeof(CAPWAPControlIPv4Address) - sizeof(ElementHeader))) {
        return false;
    }
    return true;
}

uint32_t CAPWAPControlIPv4Address::GetIPAddress() const {
    return ipaddress;
}
uint16_t CAPWAPControlIPv4Address::GetWTPCount() const {
    return wtp_count.Get();
}

void CAPWAPControlIPv4Address::Log() const {
    log_i("ME CAPWAPControlIPv4Address IP Address:%s, WTP Count:%u",
          IpToString(GetIPAddress()).c_str(),
          GetWTPCount());
}

WritableCAPWAPControlIPV4AdrArray::WritableCAPWAPControlIPV4AdrArray(
    const nonstd::span<const CAPWAPControlIPv4Address> &items)
    : items(items) {
    ASSERT(items.size() <= ReadableCAPWAPControlIPV4AdrArray::max_count);
}

void WritableCAPWAPControlIPV4AdrArray::Serialize(RawData *raw_data) const {
    for (const auto &elem : items) {
        ASSERT(raw_data->current + sizeof(CAPWAPControlIPv4Address) <= raw_data->end);
        CAPWAPControlIPv4Address *dst = (CAPWAPControlIPv4Address *)raw_data->current;
        *dst = elem;
        raw_data->current += sizeof(CAPWAPControlIPv4Address);
    }
}
uint16_t WritableCAPWAPControlIPV4AdrArray::GetTotalLength() const {
    uint16_t size = 0;
    for (const auto &elem : items) {
        size += elem.GetLength() + sizeof(ElementHeader);
    }
    return size;
}

void WritableCAPWAPControlIPV4AdrArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME CAPWAPControlIPv4Address #%lu IP Address::%s, WTP Count:%u",
              i,
              IpToString(items[i].GetIPAddress()).c_str(),
              items[i].GetWTPCount());
    }
}

ReadableCAPWAPControlIPV4AdrArray::ReadableCAPWAPControlIPV4AdrArray() : count{ 0 } {
}

bool ReadableCAPWAPControlIPV4AdrArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableCAPWAPControlIPV4AdrArray::Deserialize elements count exceeds");
        return false;
    }

    if (raw_data->current + sizeof(CAPWAPControlIPv4Address) > raw_data->end) {
        return false;
    }

    auto item = (CAPWAPControlIPv4Address *)raw_data->current;
    if (!item->Validate()) {
        return false;
    }
    raw_data->current += sizeof(CAPWAPControlIPv4Address);

    items[count] = item;
    count++;
    return true;
}

nonstd::span<const CAPWAPControlIPv4Address *const> ReadableCAPWAPControlIPV4AdrArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableCAPWAPControlIPV4AdrArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME CAPWAPControlIPv4Address #%lu IP Address::%s, WTP Count:%u",
              i,
              IpToString(items[i]->GetIPAddress()).c_str(),
              items[i]->GetWTPCount());
    }
}