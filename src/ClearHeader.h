#pragma once

#include "Helpers.h"
#include <algorithm>
#include <cstdint>

enum WBIDType { Reserved0 = 0, IEEE_80211 = 1, Reserved2 = 2, EPCGlobal = 3 };

struct __attribute__((packed)) Preamble {
    enum Version : uint8_t { RFC_5415 = 0 };
    enum PayloadType : uint8_t { ClearText = 0, DTLSCrypted = 1 };

    PayloadType type : 4;
    Version version : 4;

    bool Validate() const;
    static Preamble *Deserialize(const RawData *raw_data);
};

struct __attribute__((packed)) ClearHeader {
    enum PayloadFrameFormat { IEEE_8023_frame = 0, native_frame = 1 };

  private:
    Preamble preamble;

    uint8_t rid_0 : 3;
    uint8_t hlen : 5;

    uint8_t t : 1;
    uint8_t wbid : 5;
    uint8_t rid_1 : 2;

    uint8_t flags : 3; // reserved bits for future flags
    uint8_t k : 1;
    uint8_t m : 1;
    uint8_t w : 1;
    uint8_t l : 1;
    uint8_t f : 1;

    NetworkU16 fragment_ID;

    uint8_t fragment_offset_0;

    uint8_t reserved : 3; //reserved for future use
    uint8_t fragment_offset_1 : 5;

  public:
    // length of the CAPWAP transport header in 4-byte words
    uint8_t HLEN() const;
    // Radio ID number for this packet, whose value is between 1 - 31
    uint8_t RID() const;
    // wireless binding identifier: 0 - Reserved, 1 - IEEE 802.11, 2 - Reserved, 3 - EPCGlobal
    WBIDType WBID() const;
    // payload frame format: 0 - IEEE 802.3 frame, 1 = native frame).
    PayloadFrameFormat T() const;
    // indicates whether this packet is a fragment
    bool F() const;
    // last fragment, valid only if 'F' bit is set
    bool L() const;
    // optional Wireless Specific Information field is present in header
    bool W() const;
    // optional Radio MAC Address field is present in header
    bool M() const;
    // indicates the packet is a Data Channel Keep-Alive packet
    bool K() const;
    // value is incremented with each new set of fragments, valid only if 'F' bit is set
    uint16_t Fragment_ID() const;
    // indicates where in payload this fragment belongs during re-assembly, valid only if 'F' bit is set
    uint16_t Fragment_Offset() const;

    ClearHeader(const ClearHeader &) = delete;
    ClearHeader(uint8_t HLEN,
                uint8_t RID,
                PayloadFrameFormat T,
                bool F,
                bool L,
                bool W,
                bool M,
                bool K,
                uint16_t Fragment_ID,
                uint16_t Fragment_Offset);

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static ClearHeader *Deserialize(RawData *raw_data);
    void Log() const;
};

struct __attribute__((packed)) RadioMACAddress {
    const static int mac_EUI48_size = 6;
    const static int mac_EUI64_size = 8;

  private:
    uint8_t length;
    uint8_t mac_address[std::max(RadioMACAddress::mac_EUI48_size, RadioMACAddress::mac_EUI64_size)];

  public:
    uint8_t Length() const;
    const uint8_t *MACAddress() const;

    RadioMACAddress(const RadioMACAddress &) = delete;
    RadioMACAddress(uint8_t len, const uint8_t *mac_adr);

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static RadioMACAddress *Deserialize(RawData *raw_data);
    static size_t MinSize();
    size_t RoundedSize() const;
};

struct __attribute__((packed)) IEEE80211FrameInfo {
  private:
    uint8_t length;
    uint8_t rssi;
    uint8_t snr;
    NetworkU16 datarate;

  public:
    uint8_t RSSI() const;
    uint8_t SNR() const;
    uint16_t DataRate() const;

    IEEE80211FrameInfo(uint8_t RSSI, uint8_t SNR, uint16_t DataRate);

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static IEEE80211FrameInfo *Deserialize(RawData *raw_data);
    static size_t MinSize();
    size_t RoundedSize() const;
};

struct WritableCapwapHeader : ClearHeader {
    const RadioMACAddress *radio_mac_address;
    const IEEE80211FrameInfo *wireless_specific_information;

    WritableCapwapHeader(const WritableCapwapHeader &) = delete;
    WritableCapwapHeader(const uint8_t radio_id,
                         const ClearHeader::PayloadFrameFormat frame_format,
                         const bool fragmented,
                         const bool last_fragment,
                         const bool is_datachannel_keep_alive,
                         const uint16_t fragment_id,
                         const uint16_t fragment_offset,
                         const RadioMACAddress *radio_mac_adr,
                         const IEEE80211FrameInfo *ieee80211FrameInfo);

    void Serialize(RawData *raw_data) const;
};

struct ReadableCapwapHeader {
    ClearHeader *header;
    RadioMACAddress *radio_mac_address;
    IEEE80211FrameInfo *wireless_specific_information;

    ReadableCapwapHeader(const ReadableCapwapHeader &) = delete;
    ReadableCapwapHeader();
    bool Deserialize(RawData *raw_data);
};
