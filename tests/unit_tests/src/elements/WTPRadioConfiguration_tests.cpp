#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/WTPRadioConfiguration.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(WTPRadioConfigurationTestsGroup){ //
                                             TEST_SETUP(){}

                                             TEST_TEARDOWN(){}
};

TEST(WTPRadioConfigurationTestsGroup, Deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Message Element Header (4 bytes) ----
        0x04, 0x16,     // Type: 1046 (IEEE 802.11 WTP Radio Configuration)
        0x00, 0x10,     // Length: 16 bytes

        // ---- Value (16 bytes) ----
        0x01,           // Radio ID: 1
        0x01,           // Short Preamble: 1 (enabled)
        0x04,           // Num BSSIDs: 4
        0x02,           // DTIM Period: 2
        0xAA, 0xBB,     // BSSID: AA:BB:CC:DD:EE:FF
        0xCC, 0xDD,
        0xEE, 0xFF,
        0x00, 0x64,     // Beacon Period: 100 TUs
        'U', 'S', ' ', 0x00  // Country String: "US " + reserved
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = WTPRadioConfiguration::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::WTPRadioConfiguration, element->GetElementType());

    CHECK_EQUAL(1, element->RadioID);
    CHECK_EQUAL(1, element->ShortPreamble);
    CHECK_EQUAL(4, element->NumBSSIDs);
    CHECK_EQUAL(2, element->DTIMPeriod);
    const uint8_t expected_bssid[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    MEMCMP_EQUAL(expected_bssid, element->BSSID, sizeof(expected_bssid));
    CHECK_EQUAL(100, element->BeaconPeriod.Get());
    CHECK_EQUAL('U', element->CountryString[0]);
    CHECK_EQUAL('S', element->CountryString[1]);
    CHECK_EQUAL(' ', element->CountryString[2]);
}

TEST(WTPRadioConfigurationTestsGroup, Serialize) {
    uint8_t buffer[256] = {};
    const uint8_t bssid[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    WTPRadioConfiguration element{ 5, 0, 8, 3, bssid, 200, "DE " };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 20, raw_data.current);

    // clang-format off
    const uint8_t reference[] = {
        0x04, 0x16,     // Type: 1046
        0x00, 0x10,     // Length: 16
        0x05,           // Radio ID: 5
        0x00,           // Short Preamble: 0 (disabled)
        0x08,           // Num BSSIDs: 8
        0x03,           // DTIM Period: 3
        0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E,  // BSSID
        0x00, 0xC8,     // Beacon Period: 200 TUs
        'D', 'E', ' ', 0x00  // Country String
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = WTPRadioConfiguration::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(&buffer[0] + 20, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::WTPRadioConfiguration, deserialized->GetElementType());
    CHECK_EQUAL(5, deserialized->RadioID);
    CHECK_EQUAL(0, deserialized->ShortPreamble);
    CHECK_EQUAL(8, deserialized->NumBSSIDs);
    CHECK_EQUAL(3, deserialized->DTIMPeriod);
    MEMCMP_EQUAL(bssid, deserialized->BSSID, sizeof(bssid));
    CHECK_EQUAL(200, deserialized->BeaconPeriod.Get());
}

TEST(WTPRadioConfigurationTestsGroup, Validate_RadioID_range) {
    // clang-format off
    // Valid RadioID = 0
    uint8_t data_valid_zero[] = {
        0x04, 0x16, 0x00, 0x10,
        0x00, 0x00, 0x04, 0x02,
        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
        0x00, 0x64,
        'U', 'S', ' ', 0x00
    };
    // Valid RadioID = 31
    uint8_t data_valid_max[] = {
        0x04, 0x16, 0x00, 0x10,
        0x1F, 0x00, 0x04, 0x02,
        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
        0x00, 0x64,
        'U', 'S', ' ', 0x00
    };
    // Invalid RadioID = 32
    uint8_t data_invalid_high[] = {
        0x04, 0x16, 0x00, 0x10,
        0x20, 0x00, 0x04, 0x02,
        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
        0x00, 0x64,
        'U', 'S', ' ', 0x00
    };
    // clang-format on

    RawData raw_data = { data_valid_zero, data_valid_zero + sizeof(data_valid_zero) };
    CHECK(WTPRadioConfiguration::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_max, data_valid_max + sizeof(data_valid_max) };
    CHECK(WTPRadioConfiguration::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid_high, data_invalid_high + sizeof(data_invalid_high) };
    CHECK(WTPRadioConfiguration::Deserialize(&raw_data) == nullptr);
}

TEST(WTPRadioConfigurationTestsGroup, Validate_ShortPreamble_range) {
    // clang-format off
    // Valid ShortPreamble = 0
    uint8_t data_preamble_0[] = {
        0x04, 0x16, 0x00, 0x10,
        0x01, 0x00, 0x04, 0x02,
        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
        0x00, 0x64,
        'U', 'S', ' ', 0x00
    };
    // Valid ShortPreamble = 1
    uint8_t data_preamble_1[] = {
        0x04, 0x16, 0x00, 0x10,
        0x01, 0x01, 0x04, 0x02,
        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
        0x00, 0x64,
        'U', 'S', ' ', 0x00
    };
    // Invalid ShortPreamble = 2
    uint8_t data_preamble_2[] = {
        0x04, 0x16, 0x00, 0x10,
        0x01, 0x02, 0x04, 0x02,
        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
        0x00, 0x64,
        'U', 'S', ' ', 0x00
    };
    // clang-format on

    RawData raw_data = { data_preamble_0, data_preamble_0 + sizeof(data_preamble_0) };
    CHECK(WTPRadioConfiguration::Deserialize(&raw_data) != nullptr);

    raw_data = { data_preamble_1, data_preamble_1 + sizeof(data_preamble_1) };
    CHECK(WTPRadioConfiguration::Deserialize(&raw_data) != nullptr);

    raw_data = { data_preamble_2, data_preamble_2 + sizeof(data_preamble_2) };
    CHECK(WTPRadioConfiguration::Deserialize(&raw_data) == nullptr);
}

TEST(WTPRadioConfigurationTestsGroup, Validate_NumBSSIDs_range) {
    // clang-format off
    // Valid NumBSSIDs = 1 (minimum)
    uint8_t data_num_1[] = {
        0x04, 0x16, 0x00, 0x10,
        0x01, 0x00, 0x01, 0x02,
        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
        0x00, 0x64,
        'U', 'S', ' ', 0x00
    };
    // Valid NumBSSIDs = 16 (maximum)
    uint8_t data_num_16[] = {
        0x04, 0x16, 0x00, 0x10,
        0x01, 0x00, 0x10, 0x02,
        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
        0x00, 0x64,
        'U', 'S', ' ', 0x00
    };
    // Invalid NumBSSIDs = 0 (below minimum)
    uint8_t data_num_0[] = {
        0x04, 0x16, 0x00, 0x10,
        0x01, 0x00, 0x00, 0x02,
        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
        0x00, 0x64,
        'U', 'S', ' ', 0x00
    };
    // Invalid NumBSSIDs = 17 (above maximum)
    uint8_t data_num_17[] = {
        0x04, 0x16, 0x00, 0x10,
        0x01, 0x00, 0x11, 0x02,
        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
        0x00, 0x64,
        'U', 'S', ' ', 0x00
    };
    // clang-format on

    RawData raw_data = { data_num_1, data_num_1 + sizeof(data_num_1) };
    CHECK(WTPRadioConfiguration::Deserialize(&raw_data) != nullptr);

    raw_data = { data_num_16, data_num_16 + sizeof(data_num_16) };
    CHECK(WTPRadioConfiguration::Deserialize(&raw_data) != nullptr);

    raw_data = { data_num_0, data_num_0 + sizeof(data_num_0) };
    CHECK(WTPRadioConfiguration::Deserialize(&raw_data) == nullptr);

    raw_data = { data_num_17, data_num_17 + sizeof(data_num_17) };
    CHECK(WTPRadioConfiguration::Deserialize(&raw_data) == nullptr);
}

TEST(WTPRadioConfigurationTestsGroup, Deserialize_wrong_type) {
    // clang-format off
    uint8_t data[] = {
        0x04, 0x15,     // Wrong Type: 1045 (not WTPRadioConfiguration)
        0x00, 0x10,
        0x01, 0x00, 0x04, 0x02,
        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
        0x00, 0x64,
        'U', 'S', ' ', 0x00
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    CHECK(WTPRadioConfiguration::Deserialize(&raw_data) == nullptr);
}

TEST(WTPRadioConfigurationTestsGroup, Deserialize_wrong_length) {
    // clang-format off
    uint8_t data[] = {
        0x04, 0x16,
        0x00, 0x12,     // Wrong Length: 18 (should be 16)
        0x01, 0x00, 0x04, 0x02,
        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
        0x00, 0x64,
        'U', 'S', ' ', 0x00, 0x00, 0x00
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    CHECK(WTPRadioConfiguration::Deserialize(&raw_data) == nullptr);
}

TEST(WTPRadioConfigurationTestsGroup, Deserialize_insufficient_data) {
    // clang-format off
    uint8_t data[] = {
        0x04, 0x16, 0x00, 0x10, 0x01, 0x00, 0x04, 0x02  // Missing bytes
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    CHECK(WTPRadioConfiguration::Deserialize(&raw_data) == nullptr);
}

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

    raw_data = { buffer, buffer + 60 };

    CHECK_TRUE(r_configs.Deserialize(&raw_data));
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
