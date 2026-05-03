#pragma once

#include "Helpers.h"
#include "IElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

// RFC 5416 Section 6.21 - IEEE 802.11 Update WLAN
//
//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |    Radio ID   |    WLAN ID    |          Capability           |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |   Key Index   |  Key Status   |          Key Length           |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |    Key...
// +-+-+-+-+-+-+-+-+
//
// Type:   1044 for IEEE 802.11 Update WLAN
// Length:  >= 8

struct __attribute__((packed)) UpdateWlan : ElementHeader {
  public:
    // Maximum key length (typical WPA2 key is 32 bytes)
    static const size_t max_key_length = 64;

    // Key Status values
    enum KeyStatus : uint8_t {
        PerStation = 0,
        SharedWEP = 1,
        BeginRekeying = 2,
        CompletedRekeying = 3
    };

  protected:
    // Radio ID: An 8-bit value representing the radio, whose value is between 0 and 31.
    uint8_t radio_id;
    // WLAN ID: An 8-bit value specifying the WLAN Identifier. The value MUST be between one (1) and 16.
    uint8_t wlan_id;

    // Capability: An unsigned 16-bit value identifying the capabilities of the WLAN.
    NetworkU16 capability;

    // Key Index: An unsigned 8-bit value identifying the key index.
    uint8_t key_index;

    // Key Status: An unsigned 8-bit value representing the status of the key.
    KeyStatus key_status;

    // Key Length: An unsigned 16-bit value representing the length of the Key field in bytes.
    NetworkU16 key_length;

  public:
    // Key: A variable-length field containing the encryption key.
    uint8_t key[];

    UpdateWlan(const UpdateWlan &) = default;
    UpdateWlan(uint8_t radio_id,
               uint8_t wlan_id,
               uint16_t capability,
               uint8_t key_index,
               KeyStatus key_status,
               uint16_t key_length);

    uint8_t GetRadioID() const;
    uint8_t GetWlanID() const;
    uint16_t GetCapability() const;
    uint8_t GetKeyIndex() const;
    KeyStatus GetKeyStatus() const;
    uint16_t GetKeyLength() const;

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static UpdateWlan *Deserialize(RawData *raw_data);
};

struct WritableUpdateWlanArray : IWritableElement {
  public:
    struct Item {
        nonstd::span<const uint8_t> key_data;
        uint8_t radio_id;
        uint8_t wlan_id;
        uint16_t capability;
        uint8_t key_index;
        UpdateWlan::KeyStatus key_status;

        Item(const Item &) = default;
        Item(uint8_t radio_id,
             uint8_t wlan_id,
             uint16_t capability,
             uint8_t key_index,
             UpdateWlan::KeyStatus key_status,
             nonstd::span<const uint8_t> key);

        uint8_t GetRadioID() const;
        uint8_t GetWlanID() const;
    };

  private:
    std::vector<Item> items;

  public:
    WritableUpdateWlanArray(const WritableUpdateWlanArray &) = delete;
    WritableUpdateWlanArray();

    void Add(Item element);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableUpdateWlanArray : IReadableElement {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const UpdateWlan *, max_count> items;
    size_t count;

  public:
    ReadableUpdateWlanArray(const ReadableUpdateWlanArray &) = delete;
    ReadableUpdateWlanArray();

    bool Deserialize(RawData *raw_data) override final;
    nonstd::span<const UpdateWlan *const> Get() const;
    void Log() const override final;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};
