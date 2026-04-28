#include "RSNAErrorReportFromStation.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <cstring>

RSNAErrorReportFromStation::RSNAErrorReportFromStation(const uint8_t *client_mac_address,
                                                       const uint8_t *bssid,
                                                       uint8_t radio_id,
                                                       uint8_t wlan_id,
                                                       uint32_t tkip_icv_errors,
                                                       uint32_t tkip_local_mic_failures,
                                                       uint32_t tkip_remote_mic_failures,
                                                       uint32_t ccmp_replays,
                                                       uint32_t ccmp_decrypt_errors,
                                                       uint32_t tkip_replays)
    : ElementHeader(ElementHeader::RSNAErrorReportFromStation,
                    sizeof(RSNAErrorReportFromStation) - sizeof(ElementHeader)),
      radio_id{ radio_id }, wlan_id{ wlan_id }, reserved{ 0, 0 },
      tkip_icv_errors{ tkip_icv_errors }, tkip_local_mic_failures{ tkip_local_mic_failures },
      tkip_remote_mic_failures{ tkip_remote_mic_failures }, ccmp_replays{ ccmp_replays },
      ccmp_decrypt_errors{ ccmp_decrypt_errors }, tkip_replays{ tkip_replays } {
    memcpy(this->client_mac_address, client_mac_address, mac_address_size);
    memcpy(this->bssid, bssid, mac_address_size);
}

const uint8_t *RSNAErrorReportFromStation::GetClientMACAddress() const {
    return client_mac_address;
}

const uint8_t *RSNAErrorReportFromStation::GetBSSID() const {
    return bssid;
}

uint8_t RSNAErrorReportFromStation::GetRadioID() const {
    return radio_id;
}

uint8_t RSNAErrorReportFromStation::GetWlanID() const {
    return wlan_id;
}

uint32_t RSNAErrorReportFromStation::GetTKIPICVErrors() const {
    return tkip_icv_errors.Get();
}

uint32_t RSNAErrorReportFromStation::GetTKIPLocalMICFailures() const {
    return tkip_local_mic_failures.Get();
}

uint32_t RSNAErrorReportFromStation::GetTKIPRemoteMICFailures() const {
    return tkip_remote_mic_failures.Get();
}

uint32_t RSNAErrorReportFromStation::GetCCMPReplays() const {
    return ccmp_replays.Get();
}

uint32_t RSNAErrorReportFromStation::GetCCMPDecryptErrors() const {
    return ccmp_decrypt_errors.Get();
}

uint32_t RSNAErrorReportFromStation::GetTKIPReplays() const {
    return tkip_replays.Get();
}

bool RSNAErrorReportFromStation::Validate() const {
    static_assert(sizeof(RSNAErrorReportFromStation) == 44); // 4 header + 40 value
    if (ElementHeader::GetElementType() != ElementHeader::RSNAErrorReportFromStation) {
        return false;
    }
    if (ElementHeader::GetLength()
        != (sizeof(RSNAErrorReportFromStation) - sizeof(ElementHeader))) {
        return false;
    }
    if (radio_id > 31) {
        return false;
    }
    if (wlan_id < min_wlan_id || wlan_id > max_wlan_id) {
        return false;
    }
    return true;
}

void RSNAErrorReportFromStation::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(RSNAErrorReportFromStation) <= raw_data->end);
#pragma GCC diagnostic push
#if __GNUC__ >= 8
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
    memcpy(raw_data->current, this, sizeof(RSNAErrorReportFromStation));
#pragma GCC diagnostic pop
    raw_data->current += sizeof(RSNAErrorReportFromStation);
}

RSNAErrorReportFromStation *RSNAErrorReportFromStation::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(RSNAErrorReportFromStation) > raw_data->end) {
        return nullptr;
    }

    auto res = (RSNAErrorReportFromStation *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(RSNAErrorReportFromStation);
    return res;
}

WritableRSNAErrorReportFromStationArray::WritableRSNAErrorReportFromStationArray() {
    items.reserve(ReadableRSNAErrorReportFromStationArray::max_count);
}

void WritableRSNAErrorReportFromStationArray::Add(RSNAErrorReportFromStation element) {
    ASSERT(items.size() + 1 <= ReadableRSNAErrorReportFromStationArray::max_count);

    items.emplace_back(std::move(element));
}

bool WritableRSNAErrorReportFromStationArray::Empty() const {
    return items.empty();
}

void WritableRSNAErrorReportFromStationArray::Clear() {
    items.clear();
}

void WritableRSNAErrorReportFromStationArray::Serialize(RawData *raw_data) const {
    for (const auto &elem : items) {
        elem.Serialize(raw_data);
    }
}

void WritableRSNAErrorReportFromStationArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME RSNAErrorReportFromStation #%zu RadioID:%u, WlanID:%u, "
              "MAC:%02X:%02X:%02X:%02X:%02X:%02X, BSSID:%02X:%02X:%02X:%02X:%02X:%02X",
              i,
              items[i].GetRadioID(),
              items[i].GetWlanID(),
              items[i].GetClientMACAddress()[0],
              items[i].GetClientMACAddress()[1],
              items[i].GetClientMACAddress()[2],
              items[i].GetClientMACAddress()[3],
              items[i].GetClientMACAddress()[4],
              items[i].GetClientMACAddress()[5],
              items[i].GetBSSID()[0],
              items[i].GetBSSID()[1],
              items[i].GetBSSID()[2],
              items[i].GetBSSID()[3],
              items[i].GetBSSID()[4],
              items[i].GetBSSID()[5]);
    }
}

ReadableRSNAErrorReportFromStationArray::ReadableRSNAErrorReportFromStationArray() : count{ 0 } {
}

bool ReadableRSNAErrorReportFromStationArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableRSNAErrorReportFromStationArray::Deserialize elements count exceeds");
        return false;
    }

    auto report = RSNAErrorReportFromStation::Deserialize(raw_data);
    if (report == nullptr) {
        return false;
    }
    items[count] = report;
    count++;
    return true;
}

nonstd::span<const RSNAErrorReportFromStation *const>
ReadableRSNAErrorReportFromStationArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableRSNAErrorReportFromStationArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME RSNAErrorReportFromStation #%zu RadioID:%u, WlanID:%u, "
              "MAC:%02X:%02X:%02X:%02X:%02X:%02X, BSSID:%02X:%02X:%02X:%02X:%02X:%02X",
              i,
              items[i]->GetRadioID(),
              items[i]->GetWlanID(),
              items[i]->GetClientMACAddress()[0],
              items[i]->GetClientMACAddress()[1],
              items[i]->GetClientMACAddress()[2],
              items[i]->GetClientMACAddress()[3],
              items[i]->GetClientMACAddress()[4],
              items[i]->GetClientMACAddress()[5],
              items[i]->GetBSSID()[0],
              items[i]->GetBSSID()[1],
              items[i]->GetBSSID()[2],
              items[i]->GetBSSID()[3],
              items[i]->GetBSSID()[4],
              items[i]->GetBSSID()[5]);
    }
}
