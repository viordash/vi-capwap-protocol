#include "DuplicateIPv4Address.h"
#include "ClearHeader.h"
#include "lassert.h"
#include "logging.h"
#include <arpa/inet.h>
#include <string.h>

DuplicateIPv4Address::DuplicateIPv4Address(uint32_t ipaddress,
                                           DuplicateStatus status,
                                           uint8_t mac_length)
    : ElementHeader(ElementHeader::DuplicateIPv4Address,
                    (sizeof(DuplicateIPv4Address) + mac_length) - sizeof(ElementHeader)),
      IPAddress{ ipaddress }, Status{ status }, MACAddress{ mac_length } {
}
bool DuplicateIPv4Address::Validate() const {
    static_assert(sizeof(DuplicateIPv4Address) == 10);

    if (ElementHeader::GetElementType() != ElementHeader::DuplicateIPv4Address) {
        return false;
    }

    switch (MACAddress.Length) {
        case RadioMACAddress::mac_EUI48_size:
        case RadioMACAddress::mac_EUI64_size:
            break;
        default:
            return false;
    }
    auto mac_len = GetLength() - (sizeof(DuplicateIPv4Address) - sizeof(ElementHeader));

    if (mac_len != MACAddress.Length) {
        log_e("DuplicateIPv4Address: wrong element length");
        return false;
    }
    if (!MACAddress.Validate()) {
        return false;
    }

    switch (Status) {
        case DuplicateStatus::Cleared:
        case DuplicateStatus::Detected:
            break;

        default:
            return false;
    }
    return true;
}

WritableDuplicateIPv4AdrArray::WritableDuplicateIPv4AdrArray() {
    items.reserve(ReadableDuplicateIPv4AdrArray::max_count);
}

void WritableDuplicateIPv4AdrArray::Add(uint32_t ipaddress,
                                        DuplicateStatus status,
                                        MacAddress mac_address) {
    ASSERT(items.size() + 1 <= ReadableDuplicateIPv4AdrArray::max_count);

    auto it_exists = std::find_if(
        items.begin(),
        items.end(),
        [&ipaddress, &status, &mac_address](const WritableDuplicateIPv4AdrArray::Item &item) {
            return item.header.IPAddress == ipaddress && item.header.Status == status
                && item.Mac == mac_address;
        });
    if (it_exists != items.end()) {
        *it_exists =
            WritableDuplicateIPv4AdrArray::Item{ ipaddress, status, std::move(mac_address) };
        log_i("DuplicateIPv4Adr: replace IPAddress: %u, Status: %u, MacAddress:",
              ipaddress,
              (unsigned)status);
        MacAddress::Log(0, (*it_exists).Mac.Length, (*it_exists).Mac.Address);
    } else {
        items.emplace_back(ipaddress, status, std::move(mac_address));
    }
}

bool WritableDuplicateIPv4AdrArray::Empty() const {
    return items.empty();
}

void WritableDuplicateIPv4AdrArray::Clear() {
    items.clear();
}

void WritableDuplicateIPv4AdrArray::Serialize(RawData *raw_data) const {
    ASSERT(items.size() <= ReadableDuplicateIPv4AdrArray::max_count);

    for (const auto &elem : items) {
        ASSERT(raw_data->current + sizeof(DuplicateIPv4Address) + elem.header.GetLength()
               <= raw_data->end);
        DuplicateIPv4Address *dst = (DuplicateIPv4Address *)raw_data->current;
        *dst = elem.header;
        raw_data->current += sizeof(DuplicateIPv4Address);

        memcpy(raw_data->current, elem.Mac.Address, elem.header.MACAddress.Length);
        raw_data->current +=
            elem.header.GetLength() - (sizeof(DuplicateIPv4Address) - sizeof(ElementHeader));
    }
}
uint16_t WritableDuplicateIPv4AdrArray::GetTotalLength() const {
    uint16_t len = 0;
    for (const auto &elem : items) {
        len += elem.header.GetLength() + sizeof(ElementHeader);
    }
    return len;
}

void WritableDuplicateIPv4AdrArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        struct in_addr paddr;
        paddr.s_addr = items[i].header.IPAddress;

        log_i("ME DuplicateIPv4Address #%lu IP Address:%s, Status:%u",
              i,
              inet_ntoa(paddr),
              (unsigned)items[i].header.Status);
        MacAddress::Log(i, items[i].Mac.Length, items[i].Mac.Address);
    }
}

ReadableDuplicateIPv4AdrArray::ReadableDuplicateIPv4AdrArray() : count{ 0 } {
}

bool ReadableDuplicateIPv4AdrArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableDuplicateIPv4AdrArray::Deserialize elements count exceeds");
        return false;
    }

    if (raw_data->current + sizeof(DuplicateIPv4Address) > raw_data->end) {
        return false;
    }

    auto item = (DuplicateIPv4Address *)raw_data->current;
    if (!item->Validate()) {
        return false;
    }
    raw_data->current += item->GetLength() + sizeof(ElementHeader);

    items[count] = item;
    count++;
    return true;
}

nonstd::span<const DuplicateIPv4Address *const> ReadableDuplicateIPv4AdrArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableDuplicateIPv4AdrArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        struct in_addr paddr;
        paddr.s_addr = items[i]->IPAddress;

        log_i("ME DuplicateIPv4Address #%lu IP Address:%s, Status:%u",
              i,
              inet_ntoa(paddr),
              items[i]->Status);
        MacAddress::Log(i, items[i]->MACAddress.Length, items[i]->MACAddress.MACAddresses);
    }
}