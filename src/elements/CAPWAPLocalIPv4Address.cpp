#include "CAPWAPLocalIPv4Address.h"
#include "Logging.h"
#include "NetworkUtils.h"
#include "lassert.h"

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

uint32_t CAPWAPLocalIPv4Address::GetIPAddress() const {
    return ipaddress;
}

void CAPWAPLocalIPv4Address::Log() const {
    log_i("ME CAPWAPLocalIPv4Address IP Address:%s", IpToString(GetIPAddress()).c_str());
}

WritableCAPWAPLocalIPV4AdrArray::WritableCAPWAPLocalIPV4AdrArray(
    const nonstd::span<const CAPWAPLocalIPv4Address> &items)
    : items(items) {
    static_assert(sizeof(items[0]) == 8);
    ASSERT(items.size() <= ReadableCAPWAPLocalIPV4AdrArray::max_count);
}

void WritableCAPWAPLocalIPV4AdrArray::Serialize(RawData *raw_data) const {
    for (const auto &item : items) {
        ASSERT(raw_data->current + sizeof(item) <= raw_data->end);
        std::memcpy(raw_data->current, &item, sizeof(item));
        raw_data->current += sizeof(item);
    }
}

void WritableCAPWAPLocalIPV4AdrArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME CAPWAPLocalIPv4Address #%zu IP Address::%s",
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

    if (raw_data->current + sizeof(CAPWAPLocalIPv4Address) > raw_data->end) {
        return false;
    }

    auto item = (CAPWAPLocalIPv4Address *)raw_data->current;
    if (!item->Validate()) {
        return false;
    }
    raw_data->current += sizeof(CAPWAPLocalIPv4Address);

    items[count] = item;
    count++;
    return true;
}

nonstd::span<const CAPWAPLocalIPv4Address *const> ReadableCAPWAPLocalIPV4AdrArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableCAPWAPLocalIPV4AdrArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME CAPWAPLocalIPv4Address #%zu IP Address::%s",
              i,
              IpToString(items[i]->GetIPAddress()).c_str());
    }
}

ElementHeader::ElementType ReadableCAPWAPLocalIPV4AdrArray::GetElementType() const {
    return ElementHeader::CAPWAPLocalIPv4Address;
}

bool ReadableCAPWAPLocalIPV4AdrArray::IsPresent() const {
    return count > 0;
}