#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/RSNAErrorReportFromStation.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(RSNAErrorReportFromStationTestsGroup){ //
                                                   TEST_SETUP(){}

                                                   TEST_TEARDOWN(){}
};

TEST(RSNAErrorReportFromStationTestsGroup, Deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Message Element Header (4 bytes) ----
        0x04, 0x0B,     // Type: 1035 (IEEE 802.11 RSNA Error Report From Station)
        0x00, 0x28,     // Length: 40 bytes

        // ---- Value (40 bytes) ----
        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,  // Client MAC Address
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66,  // BSSID
        0x05,                                 // Radio ID: 5
        0x08,                                 // WLAN ID: 8
        0x00, 0x00,                           // Reserved
        0x00, 0x00, 0x00, 0x0A,              // TKIP ICV Errors: 10
        0x00, 0x00, 0x00, 0x14,              // TKIP Local MIC Failures: 20
        0x00, 0x00, 0x00, 0x1E,              // TKIP Remote MIC Failures: 30
        0x00, 0x00, 0x00, 0x28,              // CCMP Replays: 40
        0x00, 0x00, 0x00, 0x32,              // CCMP Decrypt Errors: 50
        0x00, 0x00, 0x00, 0x3C               // TKIP Replays: 60
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = RSNAErrorReportFromStation::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::RSNAErrorReportFromStation,
                element->GetElementType());

    CHECK_EQUAL(0xAA, element->GetClientMACAddress()[0]);
    CHECK_EQUAL(0xFF, element->GetClientMACAddress()[5]);
    CHECK_EQUAL(0x11, element->GetBSSID()[0]);
    CHECK_EQUAL(0x66, element->GetBSSID()[5]);
    CHECK_EQUAL(5, element->GetRadioID());
    CHECK_EQUAL(8, element->GetWlanID());
    CHECK_EQUAL(10, element->GetTKIPICVErrors());
    CHECK_EQUAL(20, element->GetTKIPLocalMICFailures());
    CHECK_EQUAL(30, element->GetTKIPRemoteMICFailures());
    CHECK_EQUAL(40, element->GetCCMPReplays());
    CHECK_EQUAL(50, element->GetCCMPDecryptErrors());
    CHECK_EQUAL(60, element->GetTKIPReplays());
}

TEST(RSNAErrorReportFromStationTestsGroup, Serialize) {
    uint8_t buffer[256] = {};
    uint8_t client_mac[6] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
    uint8_t bssid[6] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

    RSNAErrorReportFromStation element{ client_mac, bssid, 3, 10, 100, 200, 300, 400, 500, 600 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 44, raw_data.current);

    // clang-format off
    const uint8_t reference[] = {
        0x04, 0x0B,     // Type: 1035
        0x00, 0x28,     // Length: 40

        0x11, 0x22, 0x33, 0x44, 0x55, 0x66,  // Client MAC
        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,  // BSSID
        0x03,                                 // Radio ID: 3
        0x0A,                                 // WLAN ID: 10
        0x00, 0x00,                           // Reserved
        0x00, 0x00, 0x00, 0x64,              // TKIP ICV Errors: 100
        0x00, 0x00, 0x00, 0xC8,              // TKIP Local MIC Failures: 200
        0x00, 0x00, 0x01, 0x2C,              // TKIP Remote MIC Failures: 300
        0x00, 0x00, 0x01, 0x90,              // CCMP Replays: 400
        0x00, 0x00, 0x01, 0xF4,              // CCMP Decrypt Errors: 500
        0x00, 0x00, 0x02, 0x58               // TKIP Replays: 600
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = RSNAErrorReportFromStation::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(&buffer[0] + 44, raw_data.current);
    CHECK_EQUAL(3, deserialized->GetRadioID());
    CHECK_EQUAL(10, deserialized->GetWlanID());
    CHECK_EQUAL(100, deserialized->GetTKIPICVErrors());
    CHECK_EQUAL(200, deserialized->GetTKIPLocalMICFailures());
    CHECK_EQUAL(300, deserialized->GetTKIPRemoteMICFailures());
    CHECK_EQUAL(400, deserialized->GetCCMPReplays());
    CHECK_EQUAL(500, deserialized->GetCCMPDecryptErrors());
    CHECK_EQUAL(600, deserialized->GetTKIPReplays());
}

TEST(RSNAErrorReportFromStationTestsGroup, Serialize_Deserialize_array) {
    uint8_t buffer[2048] = {};
    uint8_t client_mac1[6] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
    uint8_t bssid1[6] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    uint8_t client_mac2[6] = { 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC };
    uint8_t bssid2[6] = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC };

    WritableRSNAErrorReportFromStationArray w_reports;
    w_reports.Add({ client_mac1, bssid1, 1, 5, 10, 20, 30, 40, 50, 60 });
    w_reports.Add({ client_mac2, bssid2, 2, 10, 100, 200, 300, 400, 500, 600 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_reports.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 88, raw_data.current); // 2 × 44 = 88

    ReadableRSNAErrorReportFromStationArray r_reports;
    raw_data = { buffer, buffer + 88 };

    CHECK_TRUE(r_reports.Deserialize(&raw_data));
    CHECK_TRUE(r_reports.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_reports.Get().size());

    CHECK_EQUAL(1, r_reports.Get()[0]->GetRadioID());
    CHECK_EQUAL(5, r_reports.Get()[0]->GetWlanID());
    CHECK_EQUAL(10, r_reports.Get()[0]->GetTKIPICVErrors());
    CHECK_EQUAL(60, r_reports.Get()[0]->GetTKIPReplays());

    CHECK_EQUAL(2, r_reports.Get()[1]->GetRadioID());
    CHECK_EQUAL(10, r_reports.Get()[1]->GetWlanID());
    CHECK_EQUAL(100, r_reports.Get()[1]->GetTKIPICVErrors());
    CHECK_EQUAL(600, r_reports.Get()[1]->GetTKIPReplays());
}

TEST(RSNAErrorReportFromStationTestsGroup, Validate_RadioID_range) {
    uint8_t client_mac[6] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
    uint8_t bssid[6] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    uint8_t buffer[256] = {};

    // Valid: RadioID = 0 (minimum)
    RSNAErrorReportFromStation elem_0{ client_mac, bssid, 0, 5, 0, 0, 0, 0, 0, 0 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    elem_0.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    CHECK(RSNAErrorReportFromStation::Deserialize(&raw_data) != nullptr);

    // Valid: RadioID = 1
    RSNAErrorReportFromStation elem_1{ client_mac, bssid, 1, 5, 0, 0, 0, 0, 0, 0 };
    raw_data = { buffer, buffer + sizeof(buffer) };
    elem_1.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    CHECK(RSNAErrorReportFromStation::Deserialize(&raw_data) != nullptr);

    // Valid: RadioID = 31 (maximum)
    RSNAErrorReportFromStation elem_31{ client_mac, bssid, 31, 5, 0, 0, 0, 0, 0, 0 };
    raw_data = { buffer, buffer + sizeof(buffer) };
    elem_31.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    CHECK(RSNAErrorReportFromStation::Deserialize(&raw_data) != nullptr);

    // Invalid: RadioID = 32 (above maximum)
    RSNAErrorReportFromStation elem_32{ client_mac, bssid, 32, 5, 0, 0, 0, 0, 0, 0 };
    raw_data = { buffer, buffer + sizeof(buffer) };
    elem_32.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    CHECK(RSNAErrorReportFromStation::Deserialize(&raw_data) == nullptr);
}

TEST(RSNAErrorReportFromStationTestsGroup, Validate_WlanID_range) {
    uint8_t client_mac[6] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
    uint8_t bssid[6] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    uint8_t buffer[256] = {};

    // Valid: WLAN ID = 1 (minimum)
    RSNAErrorReportFromStation elem_min{ client_mac, bssid, 1, 1, 0, 0, 0, 0, 0, 0 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    elem_min.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    CHECK(RSNAErrorReportFromStation::Deserialize(&raw_data) != nullptr);

    // Valid: WLAN ID = 16 (maximum)
    RSNAErrorReportFromStation elem_max{ client_mac, bssid, 1, 16, 0, 0, 0, 0, 0, 0 };
    raw_data = { buffer, buffer + sizeof(buffer) };
    elem_max.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    CHECK(RSNAErrorReportFromStation::Deserialize(&raw_data) != nullptr);

    // Invalid: WLAN ID = 0 (below minimum)
    RSNAErrorReportFromStation elem_zero{ client_mac, bssid, 1, 0, 0, 0, 0, 0, 0, 0 };
    raw_data = { buffer, buffer + sizeof(buffer) };
    elem_zero.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    CHECK(RSNAErrorReportFromStation::Deserialize(&raw_data) == nullptr);

    // Invalid: WLAN ID = 17 (above maximum)
    RSNAErrorReportFromStation elem_high{ client_mac, bssid, 1, 17, 0, 0, 0, 0, 0, 0 };
    raw_data = { buffer, buffer + sizeof(buffer) };
    elem_high.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    CHECK(RSNAErrorReportFromStation::Deserialize(&raw_data) == nullptr);
}

TEST(RSNAErrorReportFromStationTestsGroup, Error_counters_values) {
    uint8_t buffer[256] = {};
    uint8_t client_mac[6] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
    uint8_t bssid[6] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

    // Test with various counter values including max
    RSNAErrorReportFromStation element{ client_mac,       bssid,       5,
                                        10,               0xFFFFFFFF,  0x12345678,
                                        0xABCDEF01,       0x11111111,  0x22222222,
                                        0x33333333 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element.Serialize(&raw_data);

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = RSNAErrorReportFromStation::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(0xFFFFFFFF, deserialized->GetTKIPICVErrors());
    CHECK_EQUAL(0x12345678, deserialized->GetTKIPLocalMICFailures());
    CHECK_EQUAL(0xABCDEF01, deserialized->GetTKIPRemoteMICFailures());
    CHECK_EQUAL(0x11111111, deserialized->GetCCMPReplays());
    CHECK_EQUAL(0x22222222, deserialized->GetCCMPDecryptErrors());
    CHECK_EQUAL(0x33333333, deserialized->GetTKIPReplays());
}
