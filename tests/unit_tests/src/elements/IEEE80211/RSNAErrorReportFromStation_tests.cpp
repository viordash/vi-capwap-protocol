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
