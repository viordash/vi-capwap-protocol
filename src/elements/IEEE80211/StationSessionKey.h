#pragma once

#include "Helpers.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

// RFC 5416 Section 6.15 - IEEE 802.11 Station Session Key
//
//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                           MAC Address                         |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |          MAC Address          |A|C|           Flags           |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                         Pairwise TSC                          |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |         Pairwise TSC          |         Pairwise RSC          |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                         Pairwise RSC                          |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |    Key...
// +-+-+-+-+-+-+-+-
//
// Type:   1038 for IEEE 802.11 Station Session Key
// Length: >= 20 (variable, depends on key length)

struct __attribute__((packed)) StationSessionKey : ElementHeader {
  public:
    static const size_t mac_address_size = 6;
    static const size_t tsc_size = 6;
    static const size_t rsc_size = 6;
    static const size_t max_key_length = 256;

    // Flags bit positions
    static const uint16_t FLAG_AKM_ONLY = 0x8000;      // A: AKM-Only (bit 15)
    static const uint16_t FLAG_AC_CRYPTO = 0x4000;     // C: AC provides encryption (bit 14)

  protected:
    // MAC Address: The station's MAC Address
    uint8_t mac_address[mac_address_size];

    // Flags: A (bit 15), C (bit 14), Reserved (bits 13-0)
    NetworkU16 flags;

    // Pairwise TSC: Transmit Sequence Counter for unicast packets to station
    uint8_t pairwise_tsc[tsc_size];

    // Pairwise RSC: Receive Sequence Counter for unicast packets from station
    uint8_t pairwise_rsc[rsc_size];

  public:
    // Key: Variable-length pairwise encryption key
    uint8_t key_data[];

    StationSessionKey(const StationSessionKey &) = default;
    StationSessionKey(const uint8_t *mac_address,
                      uint16_t flags,
                      const uint8_t *pairwise_tsc,
                      const uint8_t *pairwise_rsc,
                      uint16_t key_length);

    const uint8_t *GetMACAddress() const;
    uint16_t GetFlags() const;
    bool GetAKMOnlyFlag() const;
    bool GetACCryptoFlag() const;
    const uint8_t *GetPairwiseTSC() const;
    const uint8_t *GetPairwiseRSC() const;
    uint16_t GetKeyLength() const;

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static StationSessionKey *Deserialize(RawData *raw_data);
};

struct WritableStationSessionKeyArray {
  public:
    struct Item {
        std::vector<uint8_t> key_data;
        StationSessionKey header;

        Item(const Item &) = default;
        Item(const uint8_t *mac_address,
             uint16_t flags,
             const uint8_t *pairwise_tsc,
             const uint8_t *pairwise_rsc,
             std::vector<uint8_t> &&key);

        const uint8_t *GetMACAddress() const;
    };

  private:
    std::vector<Item> items;

  public:
    WritableStationSessionKeyArray(const WritableStationSessionKeyArray &) = delete;
    WritableStationSessionKeyArray();

    void Add(Item element);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const;
    void Log() const;
};

struct ReadableStationSessionKeyArray {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const StationSessionKey *, max_count> items;
    size_t count;

  public:
    ReadableStationSessionKeyArray(const ReadableStationSessionKeyArray &) = delete;
    ReadableStationSessionKeyArray();

    bool Deserialize(RawData *raw_data);
    nonstd::span<const StationSessionKey *const> Get() const;
    void Log() const;
};
