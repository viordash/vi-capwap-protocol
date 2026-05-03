#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/Station.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(StationTestsGroup){ //
                               TEST_SETUP(){}

                               TEST_TEARDOWN(){}
};

TEST(StationTestsGroup, Serialize_Deserialize_array) {
    uint8_t buffer[2048] = {};
    uint8_t mac1[6] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
    uint8_t mac2[6] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

    std::vector<uint8_t> rates1 = { 0x82, 0x84 };
    std::vector<uint8_t> rates2 = { 0x8B, 0x96, 0x8C, 0x98, 0xB0, 0xC8 };

    WritableStationArray w_stations;
    w_stations.Add({ 1, 100, 0x00, mac1, 0x1234, 5, rates1 });
    w_stations.Add({ 2, 200, 0x01, mac2, 0x5678, 10, rates2 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_stations.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 42, raw_data.current); // (17+2) + (17+6) = 42

    ReadableStationArray r_stations;
    CHECK_FALSE(r_stations.IsPresent());

    raw_data = { buffer, buffer + 42 };

    CHECK_TRUE(r_stations.Deserialize(&raw_data));
    CHECK_TRUE(r_stations.IsPresent());
    CHECK_TRUE(r_stations.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_stations.Get().size());

    CHECK_EQUAL(1, r_stations.Get()[0]->GetRadioID());
    CHECK_EQUAL(100, r_stations.Get()[0]->GetAssociationID());
    CHECK_EQUAL(5, r_stations.Get()[0]->GetWlanID());
    CHECK_EQUAL(2, r_stations.Get()[0]->GetSupportedRatesLength());
    CHECK_EQUAL(2 + 13, r_stations.Get()[0]->GetLength());
    MEMCMP_EQUAL(rates1.data(), r_stations.Get()[0]->data, 2);

    CHECK_EQUAL(2, r_stations.Get()[1]->GetRadioID());
    CHECK_EQUAL(200, r_stations.Get()[1]->GetAssociationID());
    CHECK_EQUAL(10, r_stations.Get()[1]->GetWlanID());
    CHECK_EQUAL(6, r_stations.Get()[1]->GetSupportedRatesLength());
    CHECK_EQUAL(6 + 13, r_stations.Get()[1]->GetLength());
    MEMCMP_EQUAL(rates2.data(), r_stations.Get()[1]->data, 6);
}
