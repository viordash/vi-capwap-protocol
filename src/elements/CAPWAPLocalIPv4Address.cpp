#include "CAPWAPLocalIPv4Address.h"
#include "network_utils.h"
#include "lassert.h"
#include "logging.h"

CAPWAPLocalIPv4Address::CAPWAPLocalIPv4Address(uint32_t ipaddress)
    : ElementHeader(ElementHeader::CAPWAPLocalIPv4Address,
                    sizeof(CAPWAPLocalIPv4Address) - sizeof(ElementHeader)),
      ipaddress{ ipaddress } {
}
bool CAPWAPLocalIPv4Address::Validate() const {
    static_assert(sizeof(CAPWAPLocalIPv4Address) == 8);
    return ElementHeader::GetElementType() == ElementHeader::CAPWAPLocalIPv4Address
        && ElementHeader::GetLength() == (sizeof(CAPWAPLocalIPv4Address) - sizeof(ElementHeader));
}
void CAPWAPLocalIPv4Address::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(CAPWAPLocalIPv4Address) <= raw_data->end);
    CAPWAPLocalIPv4Address *dst = (CAPWAPLocalIPv4Address *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(CAPWAPLocalIPv4Address);
}
CAPWAPLocalIPv4Address *CAPWAPLocalIPv4Address::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(CAPWAPLocalIPv4Address) > raw_data->end) {
        return nullptr;
    }

    auto res = (CAPWAPLocalIPv4Address *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(CAPWAPLocalIPv4Address);
    return res;
}
uint16_t CAPWAPLocalIPv4Address::GetTotalLength() const {
    return GetLength() + sizeof(ElementHeader);
}

uint32_t CAPWAPLocalIPv4Address::GetIPAddress() const {
    return ipaddress;
}

void CAPWAPLocalIPv4Address::Log() const {
    log_i("ME CAPWAPLocalIPv4Address IP Address:%s", IpToString(GetIPAddress()).c_str());
}

WritableCAPWAPLocalIPV4AdrArray::WritableCAPWAPLocalIPV4AdrArray(
    const nonstd::span<const CAPWAPLocalIPv4Address> &items)
    : items(items) {
    ASSERT(items.size() <= ReadableCAPWAPLocalIPV4AdrArray::max_count);
}

void WritableCAPWAPLocalIPV4AdrArray::Serialize(RawData *raw_data) const {
    for (const auto &elem : items) {
        elem.Serialize(raw_data);
    }
}
uint16_t WritableCAPWAPLocalIPV4AdrArray::GetTotalLength() const {
    uint16_t size = 0;
    for (const auto &elem : items) {
        size += elem.GetTotalLength();
    }
    return size;
}

void WritableCAPWAPLocalIPV4AdrArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME CAPWAPLocalIPv4Address #%lu IP Address::%s",
              i,
              IpToString(items[i].GetIPAddress()).c_str());
    }
}

ReadableCAPWAPLocalIPV4AdrArray::ReadableCAPWAPLocalIPV4AdrArray() : count{ 0 } {
}

bool ReadableCAPWAPLocalIPV4AdrArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableCAPWAPLocalIPV4AdrArray::Deserialize elements count exceeds");
        return false;
    }

    auto ip_address = CAPWAPLocalIPv4Address::Deserialize(raw_data);
    if (ip_address == nullptr) {
        return false;
    }
    items[count] = ip_address;
    count++;
    return true;
}

nonstd::span<const CAPWAPLocalIPv4Address *const> ReadableCAPWAPLocalIPV4AdrArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableCAPWAPLocalIPV4AdrArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME CAPWAPLocalIPv4Address #%lu IP Address::%s",
              i,
              IpToString(items[i]->GetIPAddress()).c_str());
    }
}