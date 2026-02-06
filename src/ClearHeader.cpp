#include "ClearHeader.h"
#include "lassert.h"
#include "logging.h"
#include <cstring>

bool Preamble::Validate() const {
    static_assert(sizeof(Preamble) == 1);
    return version == RFC_5415;
}
Preamble *Preamble::Deserialize(const RawData *raw_data) {
    if (raw_data->current + sizeof(Preamble) > raw_data->end) {
        return nullptr;
    }
    auto res = (Preamble *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    return res;
}

uint8_t ClearHeader::HLEN() const {
    return hlen;
}
uint8_t ClearHeader::RID() const {
    return (rid_0 << 2) + rid_1;
}
WBIDType ClearHeader::WBID() const {
    return (WBIDType)wbid;
}
ClearHeader::PayloadFrameFormat ClearHeader::T() const {
    return t == 0 ? IEEE_8023_frame : native_frame;
}
bool ClearHeader::F() const {
    return f != 0;
}
bool ClearHeader::L() const {
    return l != 0;
}
bool ClearHeader::W() const {
    return w != 0;
}
bool ClearHeader::M() const {
    return m != 0;
}
bool ClearHeader::K() const {
    return k != 0;
}
uint16_t ClearHeader::Fragment_ID() const {
    return fragment_ID.Get();
}
uint16_t ClearHeader::Fragment_Offset() const {
    return (fragment_offset_0 << 5) + fragment_offset_1;
}

ClearHeader::ClearHeader(uint8_t HLEN,
                         uint8_t RID,
                         PayloadFrameFormat T,
                         bool F,
                         bool L,
                         bool W,
                         bool M,
                         bool K,
                         uint16_t Fragment_ID,
                         uint16_t Fragment_Offset)
    : preamble{ Preamble::ClearText, Preamble::RFC_5415 }, rid_0{ (uint8_t)(RID >> 2) },
      hlen{ HLEN }, t{ T }, wbid{ WBIDType::IEEE_80211 }, rid_1{ (uint8_t)(RID & 0x03) },
      flags{ 0 }, k{ K }, m{ M }, w{ W }, l{ L }, f{ F }, fragment_ID{ Fragment_ID },
      fragment_offset_0{ (uint8_t)(Fragment_Offset >> 5) }, reserved{ 0 },
      fragment_offset_1{ (uint8_t)(Fragment_Offset & 0x1F) } {};

bool ClearHeader::Validate() const {
    static_assert(sizeof(ClearHeader) == 8);
    if (W() && WBID() != WBIDType::IEEE_80211) {
        log_e("ClearHeader does not support WBIDType other than IEEE 802.11");
        return false;
    }
    size_t min_hlen = sizeof(ClearHeader);
    if (W()) {
        min_hlen += IEEE80211FrameInfo::MinSize();
    }
    if (M()) {
        min_hlen += RadioMACAddress::MinSize();
    }

    if (HLEN() < min_hlen / 4) {
        log_e("ClearHeader has a small HLEN");
        return false;
    }

    return RID() <= 31 //
        && T() >= PayloadFrameFormat::IEEE_8023_frame && T() <= PayloadFrameFormat::native_frame;
}
void ClearHeader::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(ClearHeader) <= raw_data->end);
    ClearHeader *dst = (ClearHeader *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(ClearHeader);
}
ClearHeader *ClearHeader::Deserialize(RawData *raw_data) {
    auto preamble = Preamble::Deserialize(raw_data);
    if (preamble == nullptr || preamble->type != Preamble::ClearText) {
        return nullptr;
    }
    if (raw_data->current + sizeof(ClearHeader) > raw_data->end) {
        return nullptr;
    }

    auto res = (ClearHeader *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(ClearHeader);
    return res;
}

void ClearHeader::Log() const {
    log_i(
        "Header HLEN:%u, RID:%u, WBID:%u, F:%u, L:%u, W:%u, M:%u, K:%u, Frag_ID:%u, Frag_Offset:%u",
        HLEN(),
        RID(),
        (unsigned)WBID(),
        F(),
        L(),
        W(),
        M(),
        K(),
        Fragment_ID(),
        Fragment_Offset());
}

uint8_t RadioMACAddress::Length() const {
    return length;
}

const uint8_t *RadioMACAddress::MACAddress() const {
    return &mac_address[0];
}
RadioMACAddress::RadioMACAddress(uint8_t len, const uint8_t *mac_adr) {
    switch (len) {
        case mac_EUI48_size:
        case mac_EUI64_size:
            length = len;
            memcpy(mac_address, mac_adr, length);
            break;

        default:
            break;
    }
};
bool RadioMACAddress::Validate() const {
    return length == mac_EUI48_size || length == mac_EUI64_size;
}
void RadioMACAddress::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + length + sizeof(length) <= raw_data->end);

    *((uint8_t *)&raw_data->current[0]) = length;
    memcpy(&raw_data->current[1], mac_address, length);

    raw_data->current += RoundTo4(length + sizeof(length));
}
RadioMACAddress *RadioMACAddress::Deserialize(RawData *raw_data) {
    if (raw_data->current + (sizeof(length) + mac_EUI48_size) > raw_data->end
        && raw_data->current + (sizeof(length) + mac_EUI64_size) > raw_data->end) {
        return nullptr;
    }

    auto res = (RadioMACAddress *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += RoundTo4(res->length + sizeof(length));
    return res;
}
size_t RadioMACAddress::MinSize() {
    return RoundTo4(sizeof(length) + mac_EUI48_size);
}
size_t RadioMACAddress::RoundedSize() const {
    return RoundTo4(sizeof(length) + length);
}

uint8_t IEEE80211FrameInfo::RSSI() const {
    return rssi;
}
uint8_t IEEE80211FrameInfo::SNR() const {
    return snr;
}
uint16_t IEEE80211FrameInfo::DataRate() const {
    return datarate.Get();
}

IEEE80211FrameInfo::IEEE80211FrameInfo(uint8_t RSSI, uint8_t SNR, uint16_t DataRate)
    : length{ sizeof(IEEE80211FrameInfo) - sizeof(length) }, rssi{ RSSI }, snr{ SNR },
      datarate{ DataRate } {};

bool IEEE80211FrameInfo::Validate() const {
    static_assert(sizeof(IEEE80211FrameInfo) - sizeof(length) == 4);
    return length == sizeof(IEEE80211FrameInfo) - sizeof(length);
}
void IEEE80211FrameInfo::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(IEEE80211FrameInfo) <= raw_data->end);

    IEEE80211FrameInfo *dst = (IEEE80211FrameInfo *)raw_data->current;
    *dst = *this;

    raw_data->current += RoundTo4(length + sizeof(length));
}
IEEE80211FrameInfo *IEEE80211FrameInfo::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(IEEE80211FrameInfo) > raw_data->end) {
        return nullptr;
    }
    auto res = (IEEE80211FrameInfo *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += RoundTo4(res->length + sizeof(length));
    return res;
}
size_t IEEE80211FrameInfo::MinSize() {
    return RoundTo4(sizeof(IEEE80211FrameInfo));
}
size_t IEEE80211FrameInfo::RoundedSize() const {
    return RoundTo4(sizeof(IEEE80211FrameInfo));
}

WritableCapwapHeader::WritableCapwapHeader(const uint8_t radio_id,
                                           const ClearHeader::PayloadFrameFormat frame_format,
                                           const bool fragmented,
                                           const bool last_fragment,
                                           const bool is_datachannel_keep_alive,
                                           const uint16_t fragment_id,
                                           const uint16_t fragment_offset,
                                           const RadioMACAddress *radio_mac_adr,
                                           const IEEE80211FrameInfo *ieee80211FrameInfo)
    : ClearHeader((sizeof(ClearHeader)
                   + (ieee80211FrameInfo != nullptr ? ieee80211FrameInfo->RoundedSize() : 0)
                   + (radio_mac_adr != nullptr ? radio_mac_adr->RoundedSize() : 0))
                      / 4,
                  radio_id,
                  frame_format,
                  fragmented,
                  last_fragment,
                  ieee80211FrameInfo != nullptr,
                  radio_mac_adr != nullptr,
                  is_datachannel_keep_alive,
                  fragment_id,
                  fragment_offset),
      radio_mac_address{ radio_mac_adr }, wireless_specific_information{ ieee80211FrameInfo } {
}
void WritableCapwapHeader::Serialize(RawData *raw_data) const {
    ClearHeader::Serialize(raw_data);

    if (ClearHeader::M()) {
        radio_mac_address->Serialize(raw_data);
    }

    if (ClearHeader::W()) {
        wireless_specific_information->Serialize(raw_data);
    }
}

ReadableCapwapHeader::ReadableCapwapHeader() {
    header = {};
    radio_mac_address = {};
    wireless_specific_information = {};
}
bool ReadableCapwapHeader::Deserialize(RawData *raw_data) {
    header = ClearHeader::Deserialize(raw_data);
    if (header == nullptr) {
        return false;
    }

    if (header->M()) {
        radio_mac_address = RadioMACAddress::Deserialize(raw_data);
        if (radio_mac_address == nullptr) {
            return false;
        }
    }

    if (header->W()) {
        wireless_specific_information = IEEE80211FrameInfo::Deserialize(raw_data);
        if (wireless_specific_information == nullptr) {
            return false;
        }
    }
    return true;
}