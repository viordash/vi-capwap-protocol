#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "elements/ElementHeader.h"
#include "elements/MacAddress.h"
#include "span.hpp"
#include <vector>

struct __attribute__((packed)) DecryptionErrorHeader : ElementHeader {
    static const size_t max_count = 255;
    // Radio ID:   The Radio Identifier refers to an interface index on the WTP, whose value is between one (1) and 31
    uint8_t RadioID;
    // Num of Entries:   The number of instances of the Length/MAC Address fields in the array.  This field MUST NOT exceed the value of 255.
    uint8_t NumOfEntries;

    DecryptionErrorHeader(const DecryptionErrorHeader &) = default;
    DecryptionErrorHeader(uint16_t element_length, uint8_t radio_id, uint8_t num_of_entries);
    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static DecryptionErrorHeader *Deserialize(RawData *raw_data);
};

struct WritableDecryptionErrorReportArray {
  public:
    struct Item {
        std::vector<MacAddress> MacAddresses;
        DecryptionErrorHeader header;

        Item(const Item &) = default;
        Item(uint8_t radio_id, std::vector<MacAddress> &&entries)
            : MacAddresses{ std::move(entries) },
              header{ CalcEntriesSize(MacAddresses), radio_id, (uint8_t)MacAddresses.size() } {};

        static uint16_t CalcEntriesSize(const nonstd::span<const MacAddress> &entries);
    };

  private:
    std::vector<Item> items;

  public:
    WritableDecryptionErrorReportArray(const WritableDecryptionErrorReportArray &) = delete;
    WritableDecryptionErrorReportArray();

    void Add(uint8_t radio_id, std::vector<MacAddress> mac_addresses);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const;
    uint16_t GetTotalLength() const;
    void Log() const;
};

struct ReadableDecryptionErrorReportArray {
  public:
    static const size_t max_count = 32;
    struct Item {
      private:
        DecryptionErrorHeader *header;
        std::array<const ReadableMacAddress *, DecryptionErrorHeader::max_count> entries;
        size_t num_of_entries;
        friend ReadableDecryptionErrorReportArray;

      public:
        Item(const Item &) = delete;
        Item() : num_of_entries{} {};
        const nonstd::span<const ReadableMacAddress *const> Get() const;
        uint8_t GetRadioID() const;
    };

  protected:
    std::array<Item, max_count> items;
    size_t count;

  public:
    ReadableDecryptionErrorReportArray(const ReadableDecryptionErrorReportArray &) = delete;
    ReadableDecryptionErrorReportArray();

    bool Deserialize(RawData *raw_data);
    const nonstd::span<const Item> Get();
    void Log() const;
};
