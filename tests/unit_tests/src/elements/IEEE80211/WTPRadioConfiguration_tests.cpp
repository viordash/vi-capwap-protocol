#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/WTPRadioConfiguration.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(WTPRadioConfigurationTestsGroup){ //
                                             TEST_SETUP(){}

                                             TEST_TEARDOWN(){}
};

TEST(WTPRadioConfigurationTestsGroup, Country_string_handling) {
    const uint8_t bssid[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    // Test with 2-char country code
    WTPRadioConfiguration elem1{ 1, 0, 1, 1, bssid, 100, "US" };
    CHECK_EQUAL('U', elem1.CountryString[0]);
    CHECK_EQUAL('S', elem1.CountryString[1]);
    CHECK_EQUAL(0, elem1.CountryString[2]);

    // Test with 3-char country code
    WTPRadioConfiguration elem2{ 1, 0, 1, 1, bssid, 100, "USA" };
    CHECK_EQUAL('U', elem2.CountryString[0]);
    CHECK_EQUAL('S', elem2.CountryString[1]);
    CHECK_EQUAL('A', elem2.CountryString[2]);

    // Test with nullptr
    WTPRadioConfiguration elem3{ 1, 0, 1, 1, bssid, 100, nullptr };
    CHECK_EQUAL(0, elem3.CountryString[0]);
}

TEST(WTPRadioConfigurationTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableWTPRadioConfigurationArray w_configs;

    const uint8_t bssid1[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x01 };
    const uint8_t bssid2[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x02 };
    const uint8_t bssid3[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x03 };

    w_configs.Add({ 1, 1, 4, 2, bssid1, 100, "US " });
    w_configs.Add({ 2, 0, 8, 3, bssid2, 200, "DE " });
    w_configs.Add({ 3, 1, 2, 1, bssid3, 150, "JP " });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_configs.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 60, raw_data.current); // 3 * 20 = 60

    ReadableWTPRadioConfigurationArray r_configs;
    CHECK_FALSE(r_configs.IsPresent());

    raw_data = { buffer, buffer + 60 };

    CHECK_TRUE(r_configs.Deserialize(&raw_data));
    CHECK_TRUE(r_configs.IsPresent());
    CHECK_TRUE(r_configs.Deserialize(&raw_data));
    CHECK_TRUE(r_configs.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_configs.Get().size());

    CHECK_EQUAL(1, r_configs.Get()[0]->RadioID);
    CHECK_EQUAL(1, r_configs.Get()[0]->ShortPreamble);
    CHECK_EQUAL(4, r_configs.Get()[0]->NumBSSIDs);
    CHECK_EQUAL(2, r_configs.Get()[0]->DTIMPeriod);
    MEMCMP_EQUAL(bssid1, r_configs.Get()[0]->BSSID, 6);
    CHECK_EQUAL(100, r_configs.Get()[0]->BeaconPeriod.Get());

    CHECK_EQUAL(2, r_configs.Get()[1]->RadioID);
    CHECK_EQUAL(0, r_configs.Get()[1]->ShortPreamble);
    CHECK_EQUAL(8, r_configs.Get()[1]->NumBSSIDs);
    CHECK_EQUAL(3, r_configs.Get()[1]->DTIMPeriod);
    MEMCMP_EQUAL(bssid2, r_configs.Get()[1]->BSSID, 6);
    CHECK_EQUAL(200, r_configs.Get()[1]->BeaconPeriod.Get());

    CHECK_EQUAL(3, r_configs.Get()[2]->RadioID);
    CHECK_EQUAL(1, r_configs.Get()[2]->ShortPreamble);
    CHECK_EQUAL(2, r_configs.Get()[2]->NumBSSIDs);
    CHECK_EQUAL(1, r_configs.Get()[2]->DTIMPeriod);
    MEMCMP_EQUAL(bssid3, r_configs.Get()[2]->BSSID, 6);
    CHECK_EQUAL(150, r_configs.Get()[2]->BeaconPeriod.Get());
}

TEST(WTPRadioConfigurationTestsGroup, Add_array_of_items_is_unique_by_radio_id) {
    uint8_t buffer[2048] = {};

    WritableWTPRadioConfigurationArray w_configs;

    // Add same RadioID multiple times - should replace
    const uint8_t bssid_0[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 };
    w_configs.Add({ 1, 1, 1, 1, bssid_0, 100, "US" });

    const uint8_t bssid_1[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    w_configs.Add({ 1, 0, 2, 2, bssid_1, 200, "GB" });

    const uint8_t bssid_2[] = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC };
    w_configs.Add({ 2, 1, 3, 3, bssid_2, 300, "DE" });

    const uint8_t bssid_3[] = { 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54 };
    w_configs.Add({ 2, 0, 4, 4, bssid_3, 400, "FR" });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_configs.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableWTPRadioConfigurationArray r_configs;
    CHECK_FALSE(r_configs.IsPresent());

    CHECK_TRUE(r_configs.Deserialize(&raw_data));
    CHECK_TRUE(r_configs.IsPresent());
    CHECK_TRUE(r_configs.Deserialize(&raw_data));
    CHECK_FALSE(r_configs.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_configs.Get().size());

    // Should have the last values for each RadioID
    CHECK_EQUAL(1, r_configs.Get()[0]->RadioID);
    CHECK_EQUAL(0, r_configs.Get()[0]->ShortPreamble);
    CHECK_EQUAL(2, r_configs.Get()[0]->NumBSSIDs);
    CHECK_EQUAL(2, r_configs.Get()[0]->DTIMPeriod);
    CHECK_EQUAL(200, r_configs.Get()[0]->BeaconPeriod.Get());
    MEMCMP_EQUAL(bssid_1, r_configs.Get()[0]->BSSID, 6);

    CHECK_EQUAL(2, r_configs.Get()[1]->RadioID);
    CHECK_EQUAL(0, r_configs.Get()[1]->ShortPreamble);
    CHECK_EQUAL(4, r_configs.Get()[1]->NumBSSIDs);
    CHECK_EQUAL(4, r_configs.Get()[1]->DTIMPeriod);
    CHECK_EQUAL(400, r_configs.Get()[1]->BeaconPeriod.Get());
    MEMCMP_EQUAL(bssid_3, r_configs.Get()[1]->BSSID, 6);
}
