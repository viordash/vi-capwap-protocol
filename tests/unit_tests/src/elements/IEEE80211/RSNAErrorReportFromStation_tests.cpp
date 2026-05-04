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
    CHECK_FALSE(r_reports.IsPresent());
    raw_data = { buffer, buffer + 88 };

    CHECK_TRUE(r_reports.Deserialize(&raw_data));
    CHECK_TRUE(r_reports.IsPresent());
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

TEST(RSNAErrorReportFromStationTestsGroup, Add_array_of_items_is_unique_by_mac_address) {
    uint8_t buffer[2048] = {};

    WritableRSNAErrorReportFromStationArray w_errors;

    // Add same MAC address multiple times - should replace
    const uint8_t client_mac_0[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 };
    const uint8_t bssid_0[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    w_errors.Add({ client_mac_0, bssid_0, 1, 1, 10, 20, 30, 40, 50, 60 });
    w_errors.Add({ client_mac_0, bssid_0, 1, 1, 100, 200, 300, 400, 500, 600 });

    const uint8_t client_mac_1[] = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC };
    const uint8_t bssid_1[] = { 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54 };
    w_errors.Add({ client_mac_1, bssid_1, 2, 2, 1, 2, 3, 4, 5, 6 });
    w_errors.Add({ client_mac_1, bssid_1, 2, 2, 11, 22, 33, 44, 55, 66 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_errors.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableRSNAErrorReportFromStationArray r_errors;
    CHECK_FALSE(r_errors.IsPresent());

    CHECK_TRUE(r_errors.Deserialize(&raw_data));
    CHECK_TRUE(r_errors.IsPresent());
    CHECK_TRUE(r_errors.Deserialize(&raw_data));
    CHECK_FALSE(r_errors.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_errors.Get().size());

    // Should have the last values for each MAC address
    MEMCMP_EQUAL(client_mac_0, r_errors.Get()[0]->GetClientMACAddress(), 6);
    CHECK_EQUAL(100, r_errors.Get()[0]->GetTKIPICVErrors());
    CHECK_EQUAL(200, r_errors.Get()[0]->GetTKIPLocalMICFailures());

    MEMCMP_EQUAL(client_mac_1, r_errors.Get()[1]->GetClientMACAddress(), 6);
    CHECK_EQUAL(11, r_errors.Get()[1]->GetTKIPICVErrors());
    CHECK_EQUAL(22, r_errors.Get()[1]->GetTKIPLocalMICFailures());
}
