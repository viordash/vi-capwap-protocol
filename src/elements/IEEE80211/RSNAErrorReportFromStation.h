#pragma once

#include "Helpers.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>

// RFC 5416 Section 6.12 - IEEE 802.11 RSNA Error Report From Station
//
//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                      Client MAC Address                       |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |          Client MAC           |            BSSID              |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                            BSSID                              |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |   Radio ID    |   WLAN ID     |           Reserved            |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                      TKIP ICV Errors                          |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                 TKIP Local MIC Failures                       |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                TKIP Remote MIC Failures                       |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                       CCMP Replays                            |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                   CCMP Decrypt Errors                         |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                       TKIP Replays                            |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Type:   1035 for IEEE 802.11 RSNA Error Report From Station
// Length: 40

struct __attribute__((packed)) RSNAErrorReportFromStation : ElementHeader {
  public:
    static const size_t mac_address_size = 6;
    static const uint8_t min_wlan_id = 1;
    static const uint8_t max_wlan_id = 16;

  protected:
    // Client MAC Address: The station's MAC address
    uint8_t client_mac_address[mac_address_size];

    // BSSID: The BSSID
    uint8_t bssid[mac_address_size];

    // Radio ID: An 8-bit value representing the radio, whose value is between 0 and 31.
    uint8_t radio_id;

    // WLAN ID: An 8-bit value specifying the WLAN Identifier (1-16)
    uint8_t wlan_id;

    // Reserved: Set to zero
    uint8_t reserved[2];

    // TKIP ICV Errors: Number of TKIP ICV errors
    NetworkU32 tkip_icv_errors;

    // TKIP Local MIC Failures: Number of TKIP local MIC failures
    NetworkU32 tkip_local_mic_failures;

    // TKIP Remote MIC Failures: Number of TKIP remote MIC failures
    NetworkU32 tkip_remote_mic_failures;

    // CCMP Replays: Number of CCMP replays
    NetworkU32 ccmp_replays;

    // CCMP Decrypt Errors: Number of CCMP decrypt errors
    NetworkU32 ccmp_decrypt_errors;

    // TKIP Replays: Number of TKIP replays
    NetworkU32 tkip_replays;

  public:
    RSNAErrorReportFromStation(const RSNAErrorReportFromStation &) = default;
    RSNAErrorReportFromStation(const uint8_t *client_mac_address,
                               const uint8_t *bssid,
                               uint8_t radio_id,
                               uint8_t wlan_id,
                               uint32_t tkip_icv_errors,
                               uint32_t tkip_local_mic_failures,
                               uint32_t tkip_remote_mic_failures,
                               uint32_t ccmp_replays,
                               uint32_t ccmp_decrypt_errors,
                               uint32_t tkip_replays);

    const uint8_t *GetClientMACAddress() const;
    const uint8_t *GetBSSID() const;
    uint8_t GetRadioID() const;
    uint8_t GetWlanID() const;
    uint32_t GetTKIPICVErrors() const;
    uint32_t GetTKIPLocalMICFailures() const;
    uint32_t GetTKIPRemoteMICFailures() const;
    uint32_t GetCCMPReplays() const;
    uint32_t GetCCMPDecryptErrors() const;
    uint32_t GetTKIPReplays() const;

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static RSNAErrorReportFromStation *Deserialize(RawData *raw_data);
};

struct WritableRSNAErrorReportFromStationArray {
  protected:
    std::vector<RSNAErrorReportFromStation> items;

  public:
    WritableRSNAErrorReportFromStationArray(const WritableRSNAErrorReportFromStationArray &) =
        delete;
    WritableRSNAErrorReportFromStationArray();

    void Add(RSNAErrorReportFromStation element);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const;
    void Log() const;
};

struct ReadableRSNAErrorReportFromStationArray {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const RSNAErrorReportFromStation *, max_count> items;
    size_t count;

  public:
    ReadableRSNAErrorReportFromStationArray(const ReadableRSNAErrorReportFromStationArray &) =
        delete;
    ReadableRSNAErrorReportFromStationArray();

    bool Deserialize(RawData *raw_data);
    nonstd::span<const RSNAErrorReportFromStation *const> Get() const;
    void Log() const;
};
