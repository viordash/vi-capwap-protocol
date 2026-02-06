
#include "AddMacAclEntry.h"
#include "lassert.h"
#include "logging.h"
#include <arpa/inet.h>
#include <string.h>

WritableAddMacAclEntry::WritableAddMacAclEntry() {
    items.reserve(MacAclEntriesHeader::max_count);
}

void WritableAddMacAclEntry::Add(MacAddress mac_address) {
    ASSERT(items.size() + 1 <= MacAclEntriesHeader::max_count);

    auto it_exists =
        std::find_if(items.begin(), items.end(), [&mac_address](const MacAddress &item) {
            return mac_address == item;
        });
    if (it_exists != items.end()) {
        *it_exists = std::move(mac_address);
        log_i("AddMacAclEntry: replace MacAddress:");
        MacAddress::Log(0, (*it_exists).Length, (*it_exists).Address);
    } else {
        items.emplace_back(std::move(mac_address));
    }
}

bool WritableAddMacAclEntry::Empty() const {
    return items.empty();
}

void WritableAddMacAclEntry::Clear() {
    items.clear();
}

void WritableAddMacAclEntry::Serialize(RawData *raw_data) const {
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

    MacAclEntriesHeader header(ElementHeader::AddMACACLEntry,
                               element_length,
                               (uint8_t)items.size());
    header.Serialize(&header_raw_data);
}

void WritableAddMacAclEntry::Log() const {
    log_i("ME AddMacAclEntry size:%u, entries:", (unsigned)items.size());
    for (size_t i = 0; i < items.size(); i++) {
        auto &item = items[i];
        MacAddress::Log(i, item.Length, item.Address);
    }
}

ReadableAddMacAclEntry::ReadableAddMacAclEntry() : header{}, count{ 0 } {
}

bool ReadableAddMacAclEntry::Deserialize(RawData *raw_data) {
    header = MacAclEntriesHeader::Deserialize(raw_data);
    if (header == nullptr) {
        return false;
    }
    if (header->GetElementType() != ElementHeader::AddMACACLEntry) {
        return false;
    }

    const uint8_t *end = raw_data->current + header->GetLength()
                       - (sizeof(MacAclEntriesHeader) - sizeof(ElementHeader));

    count = 0;
    while (raw_data->current < end) {
        if (count >= MacAclEntriesHeader::max_count) {
            log_e("ReadableAddMacAclEntry::Deserialize entries count exceeds");
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
        log_e("ReadableAddMacAclEntry::Deserialize length negative");
        return false;
    }
    return true;
}

nonstd::span<const ReadableMacAddress *const> ReadableAddMacAclEntry::Get() const {
    nonstd::span span(entries.begin(), count);
    return span;
}

void ReadableAddMacAclEntry::Log() const {
    log_i("ME AddMacAclEntry size:%lu, entries:", count);
    for (size_t i = 0; i < count; i++) {
        auto &entry = entries[i];
        MacAddress::Log(i, entry->Length, entry->MACAddresses);
    }
}
