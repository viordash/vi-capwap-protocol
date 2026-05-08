#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/AddWlan.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(AddWlanTestsGroup){ //
                               TEST_SETUP(){}

                               TEST_TEARDOWN(){}
};

TEST(AddWlanTestsGroup, Array_Serialize_Deserialize) {
    uint8_t buffer[4096] = {};

    const uint8_t group_tsc_0[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    const uint8_t group_tsc_1[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
    const uint8_t key_0[] = { 0xAA, 0xBB, 0xCC, 0xDD };

    WritableAddWlanArray w_wlans;

    w_wlans.Add({ 1,
                  1,
                  0x0001,
                  0,
                  AddWlanHeader::KeyStatus::PerStationKeys,
                  {},
                  group_tsc_0,
                  AddWlanHeader::QoS::BestEffort,
                  AddWlanHeader::AuthType::OpenSystem,
                  AddWlanHeader::MACMode::LocalMAC,
                  AddWlanHeader::TunnelMode::LocalBridging,
                  0,
                  "SSID1" });

    w_wlans.Add({ 1,
                  2,
                  0x0031,
                  1,
                  AddWlanHeader::KeyStatus::StaticWep,
                  { key_0, sizeof(key_0) },
                  group_tsc_1,
                  AddWlanHeader::QoS::Voice,
                  AddWlanHeader::AuthType::WepSharedKey,
                  AddWlanHeader::MACMode::SplitMAC,
                  AddWlanHeader::TunnelMode::Tunnel80211,
                  1,
                  "Network2" });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_wlans.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableAddWlanArray r_wlans;
    CHECK_FALSE(r_wlans.IsPresent());
    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_TRUE(r_wlans.IsPresent());
    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_FALSE(r_wlans.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_wlans.Get().size());

    CHECK_EQUAL(1, r_wlans.Get()[0].header->GetRadioID());
    CHECK_EQUAL(1, r_wlans.Get()[0].header->GetWlanID());
    CHECK_EQUAL(0, r_wlans.Get()[0].header->GetKeyLength());
    CHECK_EQUAL(AddWlanHeader::QoS::BestEffort, r_wlans.Get()[0].tail->qos);
    STRNCMP_EQUAL("SSID1", r_wlans.Get()[0].ssid, r_wlans.Get()[0].ssid_length);

    CHECK_EQUAL(1, r_wlans.Get()[1].header->GetRadioID());
    CHECK_EQUAL(2, r_wlans.Get()[1].header->GetWlanID());
    CHECK_EQUAL(4, r_wlans.Get()[1].header->GetKeyLength());
    CHECK_EQUAL(AddWlanHeader::QoS::Voice, r_wlans.Get()[1].tail->qos);
    STRNCMP_EQUAL("Network2", r_wlans.Get()[1].ssid, r_wlans.Get()[1].ssid_length);
}

TEST(AddWlanTestsGroup, Array_unique_by_radio_and_wlan_id) {
    uint8_t buffer[4096] = {};

    const uint8_t group_tsc[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    WritableAddWlanArray w_wlans;

    // Add same RadioID + WlanID multiple times - should replace
    w_wlans.Add({ 1,
                  1,
                  0x0001,
                  0,
                  AddWlanHeader::KeyStatus::PerStationKeys,
                  {},
                  group_tsc,
                  AddWlanHeader::QoS::BestEffort,
                  AddWlanHeader::AuthType::OpenSystem,
                  AddWlanHeader::MACMode::LocalMAC,
                  AddWlanHeader::TunnelMode::LocalBridging,
                  0,
                  "First" });

    w_wlans.Add({ 1,
                  1,
                  0x0002,
                  0,
                  AddWlanHeader::KeyStatus::PerStationKeys,
                  {},
                  group_tsc,
                  AddWlanHeader::QoS::Video,
                  AddWlanHeader::AuthType::OpenSystem,
                  AddWlanHeader::MACMode::LocalMAC,
                  AddWlanHeader::TunnelMode::LocalBridging,
                  0,
                  "Second" });

    // Different WlanID - should be a new entry
    w_wlans.Add({ 1,
                  2,
                  0x0003,
                  0,
                  AddWlanHeader::KeyStatus::PerStationKeys,
                  {},
                  group_tsc,
                  AddWlanHeader::QoS::Voice,
                  AddWlanHeader::AuthType::OpenSystem,
                  AddWlanHeader::MACMode::LocalMAC,
                  AddWlanHeader::TunnelMode::LocalBridging,
                  0,
                  "Third" });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_wlans.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableAddWlanArray r_wlans;
    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_FALSE(r_wlans.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_wlans.Get().size());

    // First entry should have the replaced values
    CHECK_EQUAL(1, r_wlans.Get()[0].header->GetRadioID());
    CHECK_EQUAL(1, r_wlans.Get()[0].header->GetWlanID());
    CHECK_EQUAL(0x0002, r_wlans.Get()[0].header->GetCapability());
    CHECK_EQUAL(AddWlanHeader::QoS::Video, r_wlans.Get()[0].tail->qos);
    STRNCMP_EQUAL("Second", r_wlans.Get()[0].ssid, r_wlans.Get()[0].ssid_length);

    // Second entry
    CHECK_EQUAL(1, r_wlans.Get()[1].header->GetRadioID());
    CHECK_EQUAL(2, r_wlans.Get()[1].header->GetWlanID());
    STRNCMP_EQUAL("Third", r_wlans.Get()[1].ssid, r_wlans.Get()[1].ssid_length);
}

TEST(AddWlanTestsGroup, Empty_key_and_ssid) {
    uint8_t buffer[256] = {};
    const uint8_t group_tsc[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    WritableAddWlanArray w_wlans;
    w_wlans.Add({ 0,
                  1,
                  0x0000,
                  0,
                  AddWlanHeader::KeyStatus::PerStationKeys,
                  {},
                  group_tsc,
                  AddWlanHeader::QoS::BestEffort,
                  AddWlanHeader::AuthType::OpenSystem,
                  AddWlanHeader::MACMode::LocalMAC,
                  AddWlanHeader::TunnelMode::LocalBridging,
                  0,
                  "" });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_wlans.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableAddWlanArray r_wlans;
    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_EQUAL(1, r_wlans.Get().size());
    CHECK_EQUAL(0, r_wlans.Get()[0].header->GetKeyLength());
    CHECK_EQUAL(0, r_wlans.Get()[0].ssid_length);
}

TEST(AddWlanTestsGroup, RadioID_boundaries) {
    uint8_t buffer[512] = {};
    const uint8_t group_tsc[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    WritableAddWlanArray w_wlans;

    // RadioID = 0 (minimum)
    w_wlans.Add({ 0,
                  1,
                  0x0001,
                  0,
                  AddWlanHeader::KeyStatus::PerStationKeys,
                  {},
                  group_tsc,
                  AddWlanHeader::QoS::BestEffort,
                  AddWlanHeader::AuthType::OpenSystem,
                  AddWlanHeader::MACMode::LocalMAC,
                  AddWlanHeader::TunnelMode::LocalBridging,
                  0,
                  "Min" });

    // RadioID = 31 (maximum)
    w_wlans.Add({ 31,
                  1,
                  0x0002,
                  0,
                  AddWlanHeader::KeyStatus::PerStationKeys,
                  {},
                  group_tsc,
                  AddWlanHeader::QoS::BestEffort,
                  AddWlanHeader::AuthType::OpenSystem,
                  AddWlanHeader::MACMode::LocalMAC,
                  AddWlanHeader::TunnelMode::LocalBridging,
                  0,
                  "Max" });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_wlans.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableAddWlanArray r_wlans;
    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_EQUAL(2, r_wlans.Get().size());
    CHECK_EQUAL(0, r_wlans.Get()[0].header->GetRadioID());
    CHECK_EQUAL(31, r_wlans.Get()[1].header->GetRadioID());
}

TEST(AddWlanTestsGroup, WlanID_boundaries) {
    uint8_t buffer[512] = {};
    const uint8_t group_tsc[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    WritableAddWlanArray w_wlans;

    // WlanID = 1 (minimum)
    w_wlans.Add({ 1,
                  1,
                  0x0001,
                  0,
                  AddWlanHeader::KeyStatus::PerStationKeys,
                  {},
                  group_tsc,
                  AddWlanHeader::QoS::BestEffort,
                  AddWlanHeader::AuthType::OpenSystem,
                  AddWlanHeader::MACMode::LocalMAC,
                  AddWlanHeader::TunnelMode::LocalBridging,
                  0,
                  "Min" });

    // WlanID = 16 (maximum)
    w_wlans.Add({ 1,
                  16,
                  0x0002,
                  0,
                  AddWlanHeader::KeyStatus::PerStationKeys,
                  {},
                  group_tsc,
                  AddWlanHeader::QoS::BestEffort,
                  AddWlanHeader::AuthType::OpenSystem,
                  AddWlanHeader::MACMode::LocalMAC,
                  AddWlanHeader::TunnelMode::LocalBridging,
                  0,
                  "Max" });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_wlans.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableAddWlanArray r_wlans;
    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_EQUAL(2, r_wlans.Get().size());
    CHECK_EQUAL(1, r_wlans.Get()[0].header->GetWlanID());
    CHECK_EQUAL(16, r_wlans.Get()[1].header->GetWlanID());
}

TEST(AddWlanTestsGroup, Max_ssid_length) {
    uint8_t buffer[512] = {};
    const uint8_t group_tsc[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    const char max_ssid[33] = "12345678901234567890123456789012"; // 32 chars

    WritableAddWlanArray w_wlans;
    w_wlans.Add({ 1,
                  1,
                  0x0001,
                  0,
                  AddWlanHeader::KeyStatus::PerStationKeys,
                  {},
                  group_tsc,
                  AddWlanHeader::QoS::BestEffort,
                  AddWlanHeader::AuthType::OpenSystem,
                  AddWlanHeader::MACMode::LocalMAC,
                  AddWlanHeader::TunnelMode::LocalBridging,
                  0,
                  max_ssid });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_wlans.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableAddWlanArray r_wlans;
    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_EQUAL(1, r_wlans.Get().size());
    CHECK_EQUAL(32, r_wlans.Get()[0].ssid_length);
    STRNCMP_EQUAL(max_ssid, r_wlans.Get()[0].ssid, r_wlans.Get()[0].ssid_length);
}

TEST(AddWlanTestsGroup, All_enum_values) {
    uint8_t buffer[2048] = {};
    const uint8_t group_tsc[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

    WritableAddWlanArray w_wlans;

    // All KeyStatus values
    w_wlans.Add({ 1,
                  1,
                  0,
                  0,
                  AddWlanHeader::KeyStatus::PerStationKeys,
                  {},
                  group_tsc,
                  AddWlanHeader::QoS::BestEffort,
                  AddWlanHeader::AuthType::OpenSystem,
                  AddWlanHeader::MACMode::LocalMAC,
                  AddWlanHeader::TunnelMode::LocalBridging,
                  0,
                  "KS0" });
    w_wlans.Add({ 1,
                  2,
                  0,
                  0,
                  AddWlanHeader::KeyStatus::StaticWep,
                  {},
                  group_tsc,
                  AddWlanHeader::QoS::BestEffort,
                  AddWlanHeader::AuthType::OpenSystem,
                  AddWlanHeader::MACMode::LocalMAC,
                  AddWlanHeader::TunnelMode::LocalBridging,
                  0,
                  "KS1" });
    w_wlans.Add({ 1,
                  3,
                  0,
                  0,
                  AddWlanHeader::KeyStatus::BeginRekeying,
                  {},
                  group_tsc,
                  AddWlanHeader::QoS::BestEffort,
                  AddWlanHeader::AuthType::OpenSystem,
                  AddWlanHeader::MACMode::LocalMAC,
                  AddWlanHeader::TunnelMode::LocalBridging,
                  0,
                  "KS2" });
    w_wlans.Add({ 1,
                  4,
                  0,
                  0,
                  AddWlanHeader::KeyStatus::CompletedRekeying,
                  {},
                  group_tsc,
                  AddWlanHeader::QoS::BestEffort,
                  AddWlanHeader::AuthType::OpenSystem,
                  AddWlanHeader::MACMode::LocalMAC,
                  AddWlanHeader::TunnelMode::LocalBridging,
                  0,
                  "KS3" });

    // All QoS values
    w_wlans.Add({ 2,
                  1,
                  0,
                  0,
                  AddWlanHeader::KeyStatus::PerStationKeys,
                  {},
                  group_tsc,
                  AddWlanHeader::QoS::BestEffort,
                  AddWlanHeader::AuthType::OpenSystem,
                  AddWlanHeader::MACMode::LocalMAC,
                  AddWlanHeader::TunnelMode::LocalBridging,
                  0,
                  "Q0" });
    w_wlans.Add({ 2,
                  2,
                  0,
                  0,
                  AddWlanHeader::KeyStatus::PerStationKeys,
                  {},
                  group_tsc,
                  AddWlanHeader::QoS::Video,
                  AddWlanHeader::AuthType::OpenSystem,
                  AddWlanHeader::MACMode::LocalMAC,
                  AddWlanHeader::TunnelMode::LocalBridging,
                  0,
                  "Q1" });
    w_wlans.Add({ 2,
                  3,
                  0,
                  0,
                  AddWlanHeader::KeyStatus::PerStationKeys,
                  {},
                  group_tsc,
                  AddWlanHeader::QoS::Voice,
                  AddWlanHeader::AuthType::OpenSystem,
                  AddWlanHeader::MACMode::LocalMAC,
                  AddWlanHeader::TunnelMode::LocalBridging,
                  0,
                  "Q2" });
    w_wlans.Add({ 2,
                  4,
                  0,
                  0,
                  AddWlanHeader::KeyStatus::PerStationKeys,
                  {},
                  group_tsc,
                  AddWlanHeader::QoS::Background,
                  AddWlanHeader::AuthType::OpenSystem,
                  AddWlanHeader::MACMode::LocalMAC,
                  AddWlanHeader::TunnelMode::LocalBridging,
                  0,
                  "Q3" });

    // All TunnelMode values
    w_wlans.Add({ 3,
                  1,
                  0,
                  0,
                  AddWlanHeader::KeyStatus::PerStationKeys,
                  {},
                  group_tsc,
                  AddWlanHeader::QoS::BestEffort,
                  AddWlanHeader::AuthType::OpenSystem,
                  AddWlanHeader::MACMode::LocalMAC,
                  AddWlanHeader::TunnelMode::LocalBridging,
                  0,
                  "T0" });
    w_wlans.Add({ 3,
                  2,
                  0,
                  0,
                  AddWlanHeader::KeyStatus::PerStationKeys,
                  {},
                  group_tsc,
                  AddWlanHeader::QoS::BestEffort,
                  AddWlanHeader::AuthType::OpenSystem,
                  AddWlanHeader::MACMode::LocalMAC,
                  AddWlanHeader::TunnelMode::Tunnel8023,
                  0,
                  "T1" });
    w_wlans.Add({ 3,
                  3,
                  0,
                  0,
                  AddWlanHeader::KeyStatus::PerStationKeys,
                  {},
                  group_tsc,
                  AddWlanHeader::QoS::BestEffort,
                  AddWlanHeader::AuthType::OpenSystem,
                  AddWlanHeader::MACMode::LocalMAC,
                  AddWlanHeader::TunnelMode::Tunnel80211,
                  0,
                  "T2" });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_wlans.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableAddWlanArray r_wlans;
    for (int i = 0; i < 11; i++) {
        CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    }
    CHECK_EQUAL(11, r_wlans.Get().size());

    // Verify KeyStatus
    CHECK_EQUAL(AddWlanHeader::KeyStatus::PerStationKeys, r_wlans.Get()[0].header->GetKeyStatus());
    CHECK_EQUAL(AddWlanHeader::KeyStatus::StaticWep, r_wlans.Get()[1].header->GetKeyStatus());
    CHECK_EQUAL(AddWlanHeader::KeyStatus::BeginRekeying, r_wlans.Get()[2].header->GetKeyStatus());
    CHECK_EQUAL(AddWlanHeader::KeyStatus::CompletedRekeying,
                r_wlans.Get()[3].header->GetKeyStatus());

    // Verify QoS
    CHECK_EQUAL(AddWlanHeader::QoS::BestEffort, r_wlans.Get()[4].tail->qos);
    CHECK_EQUAL(AddWlanHeader::QoS::Video, r_wlans.Get()[5].tail->qos);
    CHECK_EQUAL(AddWlanHeader::QoS::Voice, r_wlans.Get()[6].tail->qos);
    CHECK_EQUAL(AddWlanHeader::QoS::Background, r_wlans.Get()[7].tail->qos);

    // Verify TunnelMode
    CHECK_EQUAL(AddWlanHeader::TunnelMode::LocalBridging, r_wlans.Get()[8].tail->tunnel_mode);
    CHECK_EQUAL(AddWlanHeader::TunnelMode::Tunnel8023, r_wlans.Get()[9].tail->tunnel_mode);
    CHECK_EQUAL(AddWlanHeader::TunnelMode::Tunnel80211, r_wlans.Get()[10].tail->tunnel_mode);
}

TEST(AddWlanTestsGroup, Deserialize_invalid_radio_id) {
    uint8_t buffer[256] = {};
    const uint8_t group_tsc[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    WritableAddWlanArray w_wlans;
    w_wlans.Add({ 1,
                  1,
                  0x0001,
                  0,
                  AddWlanHeader::KeyStatus::PerStationKeys,
                  {},
                  group_tsc,
                  AddWlanHeader::QoS::BestEffort,
                  AddWlanHeader::AuthType::OpenSystem,
                  AddWlanHeader::MACMode::LocalMAC,
                  AddWlanHeader::TunnelMode::LocalBridging,
                  0,
                  "Test" });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_wlans.Serialize(&raw_data);

    // Corrupt RadioID to 32 (invalid, max is 31)
    buffer[4] = 32;

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableAddWlanArray r_wlans;
    CHECK_FALSE(r_wlans.Deserialize(&raw_data));
}

TEST(AddWlanTestsGroup, Deserialize_invalid_wlan_id_zero) {
    uint8_t buffer[256] = {};
    const uint8_t group_tsc[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    WritableAddWlanArray w_wlans;
    w_wlans.Add({ 1,
                  1,
                  0x0001,
                  0,
                  AddWlanHeader::KeyStatus::PerStationKeys,
                  {},
                  group_tsc,
                  AddWlanHeader::QoS::BestEffort,
                  AddWlanHeader::AuthType::OpenSystem,
                  AddWlanHeader::MACMode::LocalMAC,
                  AddWlanHeader::TunnelMode::LocalBridging,
                  0,
                  "Test" });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_wlans.Serialize(&raw_data);

    // Corrupt WlanID to 0 (invalid, min is 1)
    buffer[5] = 0;

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableAddWlanArray r_wlans;
    CHECK_FALSE(r_wlans.Deserialize(&raw_data));
}

TEST(AddWlanTestsGroup, Deserialize_invalid_wlan_id_exceeds) {
    uint8_t buffer[256] = {};
    const uint8_t group_tsc[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    WritableAddWlanArray w_wlans;
    w_wlans.Add({ 1,
                  1,
                  0x0001,
                  0,
                  AddWlanHeader::KeyStatus::PerStationKeys,
                  {},
                  group_tsc,
                  AddWlanHeader::QoS::BestEffort,
                  AddWlanHeader::AuthType::OpenSystem,
                  AddWlanHeader::MACMode::LocalMAC,
                  AddWlanHeader::TunnelMode::LocalBridging,
                  0,
                  "Test" });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_wlans.Serialize(&raw_data);

    // Corrupt WlanID to 17 (invalid, max is 16)
    buffer[5] = 17;

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableAddWlanArray r_wlans;
    CHECK_FALSE(r_wlans.Deserialize(&raw_data));
}

TEST(AddWlanTestsGroup, Deserialize_truncated_header) {
    uint8_t buffer[8] = { 0x00, 0x04, 0x00, 0x10, 0x01, 0x01, 0x00, 0x00 };

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    ReadableAddWlanArray r_wlans;
    CHECK_FALSE(r_wlans.Deserialize(&raw_data));
}

TEST(AddWlanTestsGroup, Deserialize_truncated_data) {
    uint8_t buffer[256] = {};
    const uint8_t group_tsc[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    WritableAddWlanArray w_wlans;
    w_wlans.Add({ 1,
                  1,
                  0x0001,
                  0,
                  AddWlanHeader::KeyStatus::PerStationKeys,
                  {},
                  group_tsc,
                  AddWlanHeader::QoS::BestEffort,
                  AddWlanHeader::AuthType::OpenSystem,
                  AddWlanHeader::MACMode::LocalMAC,
                  AddWlanHeader::TunnelMode::LocalBridging,
                  0,
                  "Test" });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_wlans.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    // Truncate by 2 bytes
    raw_data = { buffer, buffer + data_size - 2 };

    ReadableAddWlanArray r_wlans;
    CHECK_FALSE(r_wlans.Deserialize(&raw_data));
}

TEST(AddWlanTestsGroup, Max_array_count) {
    uint8_t buffer[8192] = {};
    const uint8_t group_tsc[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    WritableAddWlanArray w_wlans;

    // Add 16 items (maximum)
    for (uint8_t i = 1; i <= 16; i++) {
        w_wlans.Add({ (uint8_t)(i - 1),
                      i,
                      0x0001,
                      0,
                      AddWlanHeader::KeyStatus::PerStationKeys,
                      {},
                      group_tsc,
                      AddWlanHeader::QoS::BestEffort,
                      AddWlanHeader::AuthType::OpenSystem,
                      AddWlanHeader::MACMode::LocalMAC,
                      AddWlanHeader::TunnelMode::LocalBridging,
                      0,
                      "X" });
    }

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_wlans.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableAddWlanArray r_wlans;
    for (int i = 0; i < 16; i++) {
        CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    }
    CHECK_EQUAL(16, r_wlans.Get().size());

    // 17th should fail - array is full
    CHECK_FALSE(r_wlans.Deserialize(&raw_data));
}
