
#include "DecryptionErrorReport.h"
#include "Logging.h"
#include "lassert.h"
#include <arpa/inet.h>
#include <cstring>
#include <string.h>

DecryptionErrorHeader::DecryptionErrorHeader(uint16_t element_length,
                                             uint8_t radio_id,
                                             uint8_t num_of_entries)
    : ElementHeader(ElementHeader::DecryptionErrorReport, element_length), RadioID{ radio_id },
      NumOfEntries{ num_of_entries } {
}

bool DecryptionErrorHeader::Validate() const {
    static_assert(sizeof(DecryptionErrorHeader) == 6);
    if (GetElementType() != ElementHeader::DecryptionErrorReport) {
        return false;
    }
    if (GetLength() < (sizeof(DecryptionErrorHeader) - sizeof(ElementHeader))) {
        return false;
    }
    if (RadioID > 31) {
        return false;
    }
    if (NumOfEntries == 0) {
        return false;
    }
    return true;
}

uint16_t WritableDecryptionErrorReportArray::Item::CalcEntriesSize(
    const nonstd::span<const MacAddress> &entries) {
    ASSERT(entries.size() <= DecryptionErrorHeader::max_count);
    uint16_t size = sizeof(DecryptionErrorHeader) - sizeof(ElementHeader);
    for (auto &entry : entries) {
        size += sizeof(ReadableMacAddress::Length);

        ASSERT(entry.Length <= ReadableMacAddress::max_length);
        size += entry.Length;
    }
    return size;
}

WritableDecryptionErrorReportArray::WritableDecryptionErrorReportArray() {
    static_assert(sizeof(Item::header) == 6);
    items.reserve(ReadableDecryptionErrorReportArray::max_count);
}

void WritableDecryptionErrorReportArray::Add(uint8_t radio_id,
                                             std::vector<MacAddress> mac_addresses) {
    ASSERT(items.size() + 1 <= ReadableDecryptionErrorReportArray::max_count);

    auto it_exists = std::find_if(
        items.begin(),
        items.end(),
        [&radio_id, &mac_addresses](const WritableDecryptionErrorReportArray::Item &item) {
            return item.header.RadioID == radio_id && mac_addresses == item.MacAddresses;
        });
    if (it_exists != items.end()) {
        *it_exists = WritableDecryptionErrorReportArray::Item{ radio_id, std::move(mac_addresses) };
        log_i("DecryptionErrorReport: replace RadioID: %u, size:%zu",
              radio_id,
              (*it_exists).MacAddresses.size());
    } else {
        items.emplace_back(radio_id, std::move(mac_addresses));
    }
}

bool WritableDecryptionErrorReportArray::Empty() const {
    return items.empty();
}

void WritableDecryptionErrorReportArray::Clear() {
    items.clear();
}

void WritableDecryptionErrorReportArray::Serialize(RawData *raw_data) const {
    ASSERT(items.size() <= ReadableDecryptionErrorReportArray::max_count);

    for (const auto &item : items) {
        ASSERT(raw_data->current + sizeof(item.header) <= raw_data->end);
        std::memcpy(raw_data->current, &item.header, sizeof(item.header));
        raw_data->current += sizeof(item.header);

        for (auto &entry : item.MacAddresses) {
            ReadableMacAddress *write_entry = (ReadableMacAddress *)raw_data->current;
            write_entry->Length = entry.Length;

            std::memcpy(write_entry->MACAddresses, entry.Address, entry.Length);
            raw_data->current += sizeof(ReadableMacAddress::Length) + entry.Length;
        }
    }
}

void WritableDecryptionErrorReportArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME DecryptionErrorReport #%zu RadioID: %u, size:%zu, entries:",
              i,
              items[i].header.RadioID,
              items[i].MacAddresses.size());
        for (const auto &entry : items[i].MacAddresses) {
            MacAddress::Log(i, entry.Length, entry.Address);
        }
    }
}

ReadableDecryptionErrorReportArray::ReadableDecryptionErrorReportArray() : count{ 0 } {
}

bool ReadableDecryptionErrorReportArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableDecryptionErrorReportArray::Deserialize elements count exceeds");
        return false;
    }

    if (raw_data->current + sizeof(DecryptionErrorHeader) > raw_data->end) {
        return false;
    }

    Item &item = items[count];
    item.header = (DecryptionErrorHeader *)raw_data->current;
    if (!item.header->Validate()) {
        return false;
    }
    if (raw_data->current + sizeof(ElementHeader) + item.header->GetLength() > raw_data->end) {
        return false;
    }

    raw_data->current += sizeof(DecryptionErrorHeader);

    const uint8_t *end = raw_data->current + item.header->GetLength()
                       - (sizeof(DecryptionErrorHeader) - sizeof(ElementHeader));

    while (raw_data->current < end) {
        if (item.num_of_entries >= DecryptionErrorHeader::max_count) {
            log_e("ReadableDecryptionErrorReport::Deserialize entries count exceeds");
            return false;
        }

        auto entry = (ReadableMacAddress *)raw_data->current;
        if (!entry->Validate()) {
            return false;
        }

        item.entries[item.num_of_entries] = entry;
        item.num_of_entries++;
        raw_data->current += sizeof(ReadableMacAddress::Length) + entry->Length;
    }

    if (raw_data->current > end) {
        log_e("ReadableDecryptionErrorReport::Deserialize length negative");
        return false;
    }
    count++;
    return true;
}

const nonstd::span<const ReadableMacAddress *const>
ReadableDecryptionErrorReportArray::Item::Get() const {
    nonstd::span span(entries.begin(), num_of_entries);
    return span;
}

uint8_t ReadableDecryptionErrorReportArray::Item::GetRadioID() const {
    ASSERT(header != nullptr);
    return header->RadioID;
}

const nonstd::span<const ReadableDecryptionErrorReportArray::Item>
ReadableDecryptionErrorReportArray::Get() {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableDecryptionErrorReportArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME DecryptionErrorReport #%zu RadioID: %u, size:%u, entries:",
              i,
              items[i].header->RadioID,
              (unsigned)items[i].num_of_entries);
        for (size_t k = 0; k < items[i].num_of_entries; k++) {
            MacAddress::Log(i, items[i].entries[k]->Length, items[i].entries[k]->MACAddresses);
        }
    }
}

ElementHeader::ElementType ReadableDecryptionErrorReportArray::GetElementType() const {
    return ElementHeader::DecryptionErrorReport;
}

bool ReadableDecryptionErrorReportArray::IsPresent() const {
    return count > 0;
}
