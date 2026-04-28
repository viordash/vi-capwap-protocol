#pragma once

#include "Helpers.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

struct __attribute__((packed)) InformationElement : ElementHeader {
  public:
    static const size_t max_ie_length = 255;

    // Flags bit positions
    static const uint8_t FLAG_BEACON = 0x80;         // B: Include in Beacons
    static const uint8_t FLAG_PROBE_RESPONSE = 0x40; // P: Include in Probe Responses

  protected:
    // Radio ID: An 8-bit value representing the radio, whose value is between 0 and 31.
    uint8_t radio_id;

    // WLAN ID: An 8-bit value specifying the WLAN Identifier. The value MUST be between one (1) and 16.
    uint8_t wlan_id;

    // Flags: B (bit 7), P (bit 6), Reserved (bits 5-0)
    uint8_t flags;

  public:
    // Info Element: The IEEE 802.11 Information Element (type, length, value)
    uint8_t ie_data[];

    InformationElement(const InformationElement &) = default;
    InformationElement(uint8_t radio_id, uint8_t wlan_id, uint8_t flags, uint16_t ie_length);

    uint8_t GetRadioID() const;
    uint8_t GetWlanID() const;
    uint8_t GetFlags() const;
    bool GetBeaconFlag() const;
    bool GetProbeResponseFlag() const;
    uint16_t GetIELength() const;

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static InformationElement *Deserialize(RawData *raw_data);
};

struct WritableInformationElementArray {
  public:
    struct Item {
        std::vector<uint8_t> ie_data;
        InformationElement header;

        Item(const Item &) = default;
        Item(uint8_t radio_id, uint8_t wlan_id, uint8_t flags, std::vector<uint8_t> &&ie);

        uint8_t GetRadioID() const;
        uint8_t GetWlanID() const;
    };

  private:
    std::vector<Item> items;

  public:
    WritableInformationElementArray(const WritableInformationElementArray &) = delete;
    WritableInformationElementArray();

    void Add(Item element);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const;
    void Log() const;
};

struct ReadableInformationElementArray {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const InformationElement *, max_count> items;
    size_t count;

  public:
    ReadableInformationElementArray(const ReadableInformationElementArray &) = delete;
    ReadableInformationElementArray();

    bool Deserialize(RawData *raw_data);
    nonstd::span<const InformationElement *const> Get() const;
    void Log() const;
};
