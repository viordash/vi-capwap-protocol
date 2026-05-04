#include "AddWlan.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <cstring>

AddWlanHeader::AddWlanHeader(uint8_t radio_id,
                             uint8_t wlan_id,
                             uint16_t capability,
                             uint8_t key_index,
                             KeyStatus key_status,
                             uint16_t key_length,
                             uint16_t total_length)
    : ElementHeader(ElementHeader::AddWlan, total_length), radio_id{ radio_id }, wlan_id{ wlan_id },
      capability{ capability }, key_index{ key_index }, key_status{ key_status },
      key_length{ key_length } {
}

uint8_t AddWlanHeader::GetRadioID() const {
    return radio_id;
}

uint8_t AddWlanHeader::GetWlanID() const {
    return wlan_id;
}

uint16_t AddWlanHeader::GetCapability() const {
    return capability.Get();
}

uint8_t AddWlanHeader::GetKeyIndex() const {
    return key_index;
}

AddWlanHeader::KeyStatus AddWlanHeader::GetKeyStatus() const {
    return key_status;
}

uint16_t AddWlanHeader::GetKeyLength() const {
    return key_length.Get();
}

bool AddWlanHeader::Validate() const {
    static_assert(sizeof(AddWlanHeader) == 12);
    if (GetElementType() != ElementHeader::AddWlan) {
        return false;
    }
    if (radio_id > 31) {
        return false;
    }
    if (wlan_id < 1 || wlan_id > 16) {
        return false;
    }
    if (GetKeyLength() > max_key_length) {
        log_e("AddWlan: key length exceeds limit: %u", (unsigned)max_key_length);
        return false;
    }
    switch (key_status) {
        case AddWlanHeader::KeyStatus::PerStationKeys:
        case AddWlanHeader::KeyStatus::StaticWep:
        case AddWlanHeader::KeyStatus::BeginRekeying:
        case AddWlanHeader::KeyStatus::CompletedRekeying:
            break;
        default:
            return false;
    }
    return true;
}

AddWlanTail::AddWlanTail(const uint8_t *group_tsc_val,
                         AddWlanHeader::QoS qos,
                         AddWlanHeader::AuthType auth_type,
                         AddWlanHeader::MACMode mac_mode,
                         AddWlanHeader::TunnelMode tunnel_mode,
                         uint8_t suppress_ssid)
    : qos{ qos }, auth_type{ auth_type }, mac_mode{ mac_mode }, tunnel_mode{ tunnel_mode },
      suppress_ssid{ suppress_ssid } {
    memcpy(group_tsc, group_tsc_val, sizeof(group_tsc));
}

bool AddWlanTail::Validate() const {
    static_assert(sizeof(AddWlanTail) == 11);
    switch (qos) {
        case AddWlanHeader::QoS::BestEffort:
        case AddWlanHeader::QoS::Video:
        case AddWlanHeader::QoS::Voice:
        case AddWlanHeader::QoS::Background:
            break;
        default:
            return false;
    }
    switch (auth_type) {
        case AddWlanHeader::AuthType::OpenSystem:
        case AddWlanHeader::AuthType::WepSharedKey:
            break;
        default:
            return false;
    }
    switch (mac_mode) {
        case AddWlanHeader::MACMode::LocalMAC:
        case AddWlanHeader::MACMode::SplitMAC:
            break;
        default:
            return false;
    }
    switch (tunnel_mode) {
        case AddWlanHeader::TunnelMode::LocalBridging:
        case AddWlanHeader::TunnelMode::Tunnel8023:
        case AddWlanHeader::TunnelMode::Tunnel80211:
            break;
        default:
            return false;
    }
    return true;
}

WritableAddWlanArray::Item::Item(uint8_t radio_id,
                                 uint8_t wlan_id,
                                 uint16_t capability,
                                 uint8_t key_index,
                                 AddWlanHeader::KeyStatus key_status,
                                 const nonstd::span<const uint8_t> &key_data,
                                 const uint8_t *group_tsc,
                                 AddWlanHeader::QoS qos,
                                 AddWlanHeader::AuthType auth_type,
                                 AddWlanHeader::MACMode mac_mode,
                                 AddWlanHeader::TunnelMode tunnel_mode,
                                 uint8_t suppress_ssid,
                                 const std::string_view &ssid_data)
    : header{ radio_id,
              wlan_id,
              capability,
              key_index,
              key_status,
              (uint16_t)key_data.size(),
              (uint16_t)((sizeof(AddWlanHeader) - sizeof(ElementHeader)) + key_data.size()
                         + sizeof(AddWlanTail) + ssid_data.size()) },
      key{ key_data.begin(), key_data.end() },
      tail{ group_tsc, qos, auth_type, mac_mode, tunnel_mode, suppress_ssid },
      ssid{ ssid_data.begin(), ssid_data.end() } {
}

uint8_t WritableAddWlanArray::Item::GetRadioID() const {
    return header.GetRadioID();
}

uint8_t WritableAddWlanArray::Item::GetWlanID() const {
    return header.GetWlanID();
}

void WritableAddWlanArray::Item::Serialize(RawData *raw_data) const {
    size_t total_size = sizeof(AddWlanHeader) + key.size() + sizeof(AddWlanTail) + ssid.size();
    ASSERT(raw_data->current + total_size <= raw_data->end);

    memcpy(raw_data->current, &header, sizeof(header));
    raw_data->current += sizeof(header);

    if (!key.empty()) {
        memcpy(raw_data->current, key.data(), key.size());
        raw_data->current += key.size();
    }

    memcpy(raw_data->current, &tail, sizeof(tail));
    raw_data->current += sizeof(tail);

    if (!ssid.empty()) {
        memcpy(raw_data->current, ssid.data(), ssid.size());
        raw_data->current += ssid.size();
    }
}

void WritableAddWlanArray::Item::Log() const {
    log_i("ME AddWlan RadioID:%u, WlanID:%u, Capability:0x%04X, KeyIndex:%u, KeyStatus:%u, "
          "KeyLen:%zu, QoS:%u, AuthType:%u, MACMode:%u, TunnelMode:%u, SuppressSSID:%u, SSID:%.*s",
          header.GetRadioID(),
          header.GetWlanID(),
          header.GetCapability(),
          header.GetKeyIndex(),
          header.GetKeyStatus(),
          key.size(),
          tail.qos,
          tail.auth_type,
          tail.mac_mode,
          tail.tunnel_mode,
          tail.suppress_ssid,
          (int)ssid.size(),
          ssid.data());
}

WritableAddWlanArray::WritableAddWlanArray() {
    items.reserve(ReadableAddWlanArray::max_count);
}

void WritableAddWlanArray::Add(WritableAddWlanArray::Item wlan) {
    ASSERT(items.size() + 1 <= ReadableAddWlanArray::max_count);

    auto it_exists =
        std::find_if(items.begin(), items.end(), [&wlan](const WritableAddWlanArray::Item &item) {
            return item.header.GetRadioID() == wlan.header.GetRadioID()
                && item.header.GetWlanID() == wlan.header.GetWlanID();
        });

    if (it_exists != items.end()) {
        *it_exists = std::move(wlan);
        log_i("AddWlan: replace RadioID: %u, WlanID: %u",
              (*it_exists).header.GetRadioID(),
              (*it_exists).header.GetWlanID());
    } else {
        items.emplace_back(std::move(wlan));
    }
}

bool WritableAddWlanArray::Empty() const {
    return items.empty();
}

void WritableAddWlanArray::Clear() {
    items.clear();
}

void WritableAddWlanArray::Serialize(RawData *raw_data) const {
    for (const auto &elem : items) {
        elem.Serialize(raw_data);
    }
}

void WritableAddWlanArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME AddWlan #%zu", i);
        items[i].Log();
    }
}

ReadableAddWlanArray::ReadableAddWlanArray() : count{ 0 } {
}

bool ReadableAddWlanArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableAddWlanArray::Deserialize elements count exceeds");
        return false;
    }

    if (raw_data->current + sizeof(AddWlanHeader) > raw_data->end) {
        return false;
    }

    auto hdr = (const AddWlanHeader *)raw_data->current;
    if (!hdr->Validate()) {
        return false;
    }

    size_t element_length = hdr->GetLength();
    if (raw_data->current + sizeof(ElementHeader) + element_length > raw_data->end) {
        return false;
    }

    uint16_t key_len = hdr->GetKeyLength();

    size_t min_size =
        (sizeof(AddWlanHeader) - sizeof(ElementHeader)) + key_len + sizeof(AddWlanTail);
    if (element_length < min_size) {
        log_e("AddWlan: element length too small");
        return false;
    }

    size_t ssid_len = element_length - min_size;
    if (ssid_len > AddWlanHeader::max_ssid_length) {
        log_e("AddWlan: SSID length exceeds limit: %u", (unsigned)AddWlanHeader::max_ssid_length);
        return false;
    }

    items[count].header = hdr;
    raw_data->current += sizeof(AddWlanHeader);

    items[count].key = raw_data->current;
    raw_data->current += key_len;

    auto tail_ptr = (const AddWlanTail *)raw_data->current;
    if (!tail_ptr->Validate()) {
        return false;
    }
    items[count].tail = tail_ptr;
    raw_data->current += sizeof(AddWlanTail);

    items[count].ssid = (const char *)raw_data->current;
    items[count].ssid_length = ssid_len;
    raw_data->current += ssid_len;

    count++;
    return true;
}

nonstd::span<const ReadableAddWlanArray::Item> ReadableAddWlanArray::Get() const {
    return { items.begin(), count };
}

ElementHeader::ElementType ReadableAddWlanArray::GetElementType() const {
    return ElementHeader::AddWlan;
}

bool ReadableAddWlanArray::IsPresent() const {
    return count > 0;
}

void ReadableAddWlanArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i(
            "ME AddWlan #%zu RadioID:%u, WlanID:%u, Capability:0x%04X, KeyIndex:%u, KeyStatus:%u, "
            "KeyLen:%u, QoS:%u, AuthType:%u, MACMode:%u, TunnelMode:%u, SuppressSSID:%u, "
            "SSID:%.*s",
            i,
            items[i].header->GetRadioID(),
            items[i].header->GetWlanID(),
            items[i].header->GetCapability(),
            items[i].header->GetKeyIndex(),
            items[i].header->GetKeyStatus(),
            items[i].header->GetKeyLength(),
            items[i].tail->qos,
            items[i].tail->auth_type,
            items[i].tail->mac_mode,
            items[i].tail->tunnel_mode,
            items[i].tail->suppress_ssid,
            (int)items[i].ssid_length,
            items[i].ssid);
    }
}
