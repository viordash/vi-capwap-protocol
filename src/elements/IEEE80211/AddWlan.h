#pragma once

#include "Helpers.h"
#include "IElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <string_view>
#include <vector>

// Fixed header part before variable Key field
struct __attribute__((packed)) AddWlanHeader : ElementHeader {
  public:
    static const size_t max_key_length = 256;
    static const size_t max_ssid_length = 32;

    enum KeyStatus : uint8_t {
        PerStationKeys = 0,
        StaticWep = 1,
        BeginRekeying = 2,
        CompletedRekeying = 3
    };

    enum QoS : uint8_t { BestEffort = 0, Video = 1, Voice = 2, Background = 3 };

    enum AuthType : uint8_t {
        OpenSystem = 0,
        WepSharedKey = 1,
        Wpa2Psk = 2,
        WpaPsk = 3,
        Wpa3Sae = 4,
        WpaEap = 5,
        Wpa2Eap = 6,
        Wpa3Enterprise = 7,
    };

    enum MACMode : uint8_t { LocalMAC = 0, SplitMAC = 1 };

    enum TunnelMode : uint8_t { LocalBridging = 0, Tunnel8023 = 1, Tunnel80211 = 2 };

  protected:
    // Radio ID: An 8-bit value representing the radio, whose value is between 0 and 31.
    uint8_t radio_id;

    // WLAN ID: An 8-bit value specifying the WLAN Identifier. The value MUST be between one (1) and 16.
    uint8_t wlan_id;

    // Capability: A 16-bit value containing the Capability information field.
    NetworkU16 capability;

    // Key Index: The Key Index associated with the key.
    uint8_t key_index;

    // Key Status: A 1-byte value that specifies the state and usage of the key.
    KeyStatus key_status;

    // Key Length: A 16-bit value representing the length of the Key field.
    NetworkU16 key_length;

  public:
    AddWlanHeader(const AddWlanHeader &) = default;
    AddWlanHeader(uint8_t radio_id,
                  uint8_t wlan_id,
                  uint16_t capability,
                  uint8_t key_index,
                  KeyStatus key_status,
                  uint16_t key_length,
                  uint16_t total_length);

    uint8_t GetRadioID() const;
    uint8_t GetWlanID() const;
    uint16_t GetCapability() const;
    uint8_t GetKeyIndex() const;
    KeyStatus GetKeyStatus() const;
    uint16_t GetKeyLength() const;
    bool Validate() const;
};

// Fixed tail part after variable Key field
struct __attribute__((packed)) AddWlanTail {
  public:
    // Group TSC: A 48-bit value containing the Transmit Sequence Counter for the updated group key.
    uint8_t group_tsc[6];

    // QoS: An 8-bit value specifying the default QoS policy.
    AddWlanHeader::QoS qos;

    // Auth Type: An 8-bit value specifying the supported authentication type.
    AddWlanHeader::AuthType auth_type;

    // MAC Mode: This field specifies whether the WTP should support the WLAN in Local or Split MAC mode.
    AddWlanHeader::MACMode mac_mode;

    // Tunnel Mode: This field specifies the frame tunneling type.
    AddWlanHeader::TunnelMode tunnel_mode;

    // Suppress SSID: A boolean indicating whether the SSID is to be advertised by the WTP.
    uint8_t suppress_ssid;

    AddWlanTail(const AddWlanTail &) = default;
    AddWlanTail(const uint8_t *group_tsc,
                AddWlanHeader::QoS qos,
                AddWlanHeader::AuthType auth_type,
                AddWlanHeader::MACMode mac_mode,
                AddWlanHeader::TunnelMode tunnel_mode,
                uint8_t suppress_ssid);

    bool Validate() const;
};

struct WritableAddWlanArray : IWritableWlanConfigurationRequestOptionalElement {
  public:
    struct Item {
        AddWlanHeader header;
        std::vector<uint8_t> key;
        AddWlanTail tail;
        std::vector<char> ssid;

        Item(const Item &) = default;
        Item(uint8_t radio_id,
             uint8_t wlan_id,
             uint16_t capability,
             uint8_t key_index,
             AddWlanHeader::KeyStatus key_status,
             const nonstd::span<const uint8_t> &key,
             const uint8_t *group_tsc,
             AddWlanHeader::QoS qos,
             AddWlanHeader::AuthType auth_type,
             AddWlanHeader::MACMode mac_mode,
             AddWlanHeader::TunnelMode tunnel_mode,
             uint8_t suppress_ssid,
             const std::string_view &ssid);

        uint8_t GetRadioID() const;
        uint8_t GetWlanID() const;
        void Serialize(RawData *raw_data) const;
        void Log() const;
    };

  private:
    std::vector<Item> items;

  public:
    WritableAddWlanArray(const WritableAddWlanArray &) = delete;
    WritableAddWlanArray();

    void Add(Item wlan);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const override;
    void Log() const override;
};

struct ReadableAddWlanArray : IReadableWlanConfigurationRequestOptionalElement {
  public:
    static const size_t max_count = 16;

    struct Item {
        const AddWlanHeader *header;
        const uint8_t *key;
        const AddWlanTail *tail;
        const char *ssid;
        size_t ssid_length;
    };

  protected:
    std::array<Item, max_count> items;
    size_t count;

  public:
    ReadableAddWlanArray(const ReadableAddWlanArray &) = delete;
    ReadableAddWlanArray();

    bool Deserialize(RawData *raw_data) override;
    ElementHeader::ElementType GetElementType() const override;
    bool IsPresent() const override;
    nonstd::span<const Item> Get() const;
    void Log() const override;
};
