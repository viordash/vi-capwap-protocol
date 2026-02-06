
#include "DeleteMacAclEntry.h"
#include "lassert.h"
#include "logging.h"
#include <arpa/inet.h>
#include <string.h>

WritableDeleteMacAclEntry::WritableDeleteMacAclEntry() {
    items.reserve(MacAclEntriesHeader::max_count);
}

void WritableDeleteMacAclEntry::Add(MacAddress mac_address) {
    ASSERT(items.size() + 1 <= MacAclEntriesHeader::max_count);

    auto it_exists =
        std::find_if(items.begin(), items.end(), [&mac_address](const MacAddress &item) {
            return mac_address == item;
        });
    if (it_exists != items.end()) {
        *it_exists = std::move(mac_address);
        log_i("DeleteMacAclEntry: replace MacAddress:");
        MacAddress::Log(0, mac_address.Length, mac_address.Address);
    } else {
        items.emplace_back(std::move(mac_address));
    }
}

bool WritableDeleteMacAclEntry::Empty() const {
    return items.empty();
}

void WritableDeleteMacAclEntry::Clear() {
    items.clear();
}

void WritableDeleteMacAclEntry::Serialize(RawData *raw_data) const {
    ASSERT(items.size() <= MacAclEntriesHeader::max_count);

    if (Empty()) {
        return;
    }
    RawData header_raw_data = *raw_data;
    raw_data->current += sizeof(MacAclEntriesHeader);

    for (auto &elem : items) {
        ReadableMacAddress *write_entry = (ReadableMacAddress *)raw_data->current;
        write_entry->Length = elem.Length;

        memcpy(write_entry->MACAddresses, elem.Address, elem.Length);
        raw_data->current += sizeof(ReadableMacAddress::Length) + elem.Length;
    }

    int element_length = raw_data->current - header_raw_data.current - sizeof(ElementHeader);
    ASSERT(element_length > 0 && element_length < INT16_MAX);

    MacAclEntriesHeader header(ElementHeader::DeleteMACACLEntry,
                               element_length,
                               (uint8_t)items.size());
    header.Serialize(&header_raw_data);
}

void WritableDeleteMacAclEntry::Log() const {
    log_i("ME DeleteMacAclEntry size:%u, entries:", (unsigned)items.size());
    for (size_t i = 0; i < items.size(); i++) {
        auto &item = items[i];
        MacAddress::Log(i, item.Length, item.Address);
    }
}

ReadableDeleteMacAclEntry::ReadableDeleteMacAclEntry() : header{}, count{ 0 } {
}

bool ReadableDeleteMacAclEntry::Deserialize(RawData *raw_data) {
    header = MacAclEntriesHeader::Deserialize(raw_data);
    if (header == nullptr) {
        return false;
    }
    if (header->GetElementType() != ElementHeader::DeleteMACACLEntry) {
        return false;
    }

    const uint8_t *end = raw_data->current + header->GetLength()
                       - (sizeof(MacAclEntriesHeader) - sizeof(ElementHeader));

    count = 0;
    while (raw_data->current < end) {
        if (count >= MacAclEntriesHeader::max_count) {
            log_e("ReadableDeleteMacAclEntry::Deserialize entries count exceeds");
            return false;
        }

        auto entry = (ReadableMacAddress *)raw_data->current;
        if (!entry->Validate()) {
            return false;
        }

        entries[count] = entry;
        count++;
        raw_data->current += sizeof(ReadableMacAddress::Length) + entry->Length;
    }

    if (raw_data->current > end) {
        log_e("ReadableDeleteMacAclEntry::Deserialize length negative");
        return false;
    }
    return true;
}

nonstd::span<const ReadableMacAddress *const> ReadableDeleteMacAclEntry::Get() const {
    nonstd::span span(entries.begin(), count);
    return span;
}

void ReadableDeleteMacAclEntry::Log() const {
    log_i("ME DeleteMacAclEntry size:%lu, entries:", count);
    for (size_t i = 0; i < count; i++) {
        auto &entry = entries[i];
        MacAddress::Log(i, entry->Length, entry->MACAddresses);
    }
}
