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

TEST(StationTestsGroup, Deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Message Element Header (4 bytes) ----
        0x04, 0x0C,     // Type: 1036 (IEEE 802.11 Station)
        0x00, 0x12,     // Length: 18 bytes (13 fixed + 5 rates)

        // ---- Value (18 bytes) ----
        0x01,                     // Radio ID: 1
        0x00, 0x64,               // Association ID: 100
        0x00,                     // Flags
        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,  // MAC Address
        0x31, 0x04,               // Capabilities: 0x3104
        0x05,                     // WLAN ID: 5
        // Supported Rates (5 bytes): 2, 4, 11, 22, 12 Mbps
        0x82, 0x84, 0x8B, 0x96, 0x8C
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = Station::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::Station, element->GetElementType());

    CHECK_EQUAL(1, element->GetRadioID());
    CHECK_EQUAL(100, element->GetAssociationID());
    CHECK_EQUAL(0, element->GetFlags());
    CHECK_EQUAL(0xAA, element->GetMACAddress()[0]);
    CHECK_EQUAL(0xFF, element->GetMACAddress()[5]);
    CHECK_EQUAL(0x3104, element->GetCapabilities());
    CHECK_EQUAL(5, element->GetWlanID());
    CHECK_EQUAL(5, element->GetSupportedRatesLength());
    CHECK_EQUAL(0x82, element->supported_rates_data[0]);
    CHECK_EQUAL(0x8C, element->supported_rates_data[4]);
}

TEST(StationTestsGroup, Serialize) {
    uint8_t buffer[256] = {};
    uint8_t mac[6] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
    std::vector<uint8_t> rates = { 0x82, 0x84, 0x8B, 0x96 };

    WritableStationArray::Item item{ 2, 200, 0x00, mac, 0x1234, 10, rates };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    item.header.Serialize(&raw_data);
    memcpy(raw_data.current, item.supported_rates_data.data(), item.supported_rates_data.size());
    raw_data.current += item.supported_rates_data.size();

    CHECK_EQUAL(&buffer[0] + 21, raw_data.current); // 17 header + 4 rates

    // clang-format off
    const uint8_t reference[] = {
        0x04, 0x0C,     // Type: 1036
        0x00, 0x11,     // Length: 17 (13 fixed + 4 rates)

        0x02,                     // Radio ID: 2
        0x00, 0xC8,               // Association ID: 200
        0x00,                     // Flags
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66,  // MAC
        0x12, 0x34,               // Capabilities: 0x3412
        0x0A,                     // WLAN ID: 10
        0x82, 0x84, 0x8B, 0x96    // Supported Rates
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = Station::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(&buffer[0] + 21, raw_data.current);
    CHECK_EQUAL(2, deserialized->GetRadioID());
    CHECK_EQUAL(200, deserialized->GetAssociationID());
    CHECK_EQUAL(10, deserialized->GetWlanID());
    CHECK_EQUAL(4, deserialized->GetSupportedRatesLength());
}

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
    raw_data = { buffer, buffer + 42 };

    CHECK_TRUE(r_stations.Deserialize(&raw_data));
    CHECK_TRUE(r_stations.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_stations.Get().size());

    CHECK_EQUAL(1, r_stations.Get()[0]->GetRadioID());
    CHECK_EQUAL(100, r_stations.Get()[0]->GetAssociationID());
    CHECK_EQUAL(5, r_stations.Get()[0]->GetWlanID());
    CHECK_EQUAL(2, r_stations.Get()[0]->GetSupportedRatesLength());

    CHECK_EQUAL(2, r_stations.Get()[1]->GetRadioID());
    CHECK_EQUAL(200, r_stations.Get()[1]->GetAssociationID());
    CHECK_EQUAL(10, r_stations.Get()[1]->GetWlanID());
    CHECK_EQUAL(6, r_stations.Get()[1]->GetSupportedRatesLength());
}

TEST(StationTestsGroup, Validate_RadioID_range) {
    // clang-format off
    // Valid: RadioID = 0 (minimum)
    uint8_t data_valid_zero[] = {
        0x04, 0x0C, 0x00, 0x0E,
        0x00, 0x00, 0x64, 0x00,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
        0x00, 0x00, 0x05, 0x82
    };

    // Valid: RadioID = 1
    uint8_t data_valid_one[] = {
        0x04, 0x0C, 0x00, 0x0E,
        0x01, 0x00, 0x64, 0x00,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
        0x00, 0x00, 0x05, 0x82
    };

    // Valid: RadioID = 31 (maximum)
    uint8_t data_valid_max[] = {
        0x04, 0x0C, 0x00, 0x0E,
        0x1F, 0x00, 0x64, 0x00,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
        0x00, 0x00, 0x05, 0x82
    };

    // Invalid: RadioID = 32 (above maximum)
    uint8_t data_invalid_high[] = {
        0x04, 0x0C, 0x00, 0x0E,
        0x20, 0x00, 0x64, 0x00,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
        0x00, 0x00, 0x05, 0x82
    };
    // clang-format on

    RawData raw_data = { data_valid_zero, data_valid_zero + sizeof(data_valid_zero) };
    CHECK(Station::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_one, data_valid_one + sizeof(data_valid_one) };
    CHECK(Station::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_max, data_valid_max + sizeof(data_valid_max) };
    CHECK(Station::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid_high, data_invalid_high + sizeof(data_invalid_high) };
    CHECK(Station::Deserialize(&raw_data) == nullptr);
}

TEST(StationTestsGroup, Validate_WlanID_range) {
    // clang-format off
    // Valid: WLAN ID = 1 (minimum)
    uint8_t data_valid_min[] = {
        0x04, 0x0C, 0x00, 0x0E,
        0x01, 0x00, 0x64, 0x00,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
        0x00, 0x00, 0x01, 0x82
    };

    // Valid: WLAN ID = 16 (maximum)
    uint8_t data_valid_max[] = {
        0x04, 0x0C, 0x00, 0x0E,
        0x01, 0x00, 0x64, 0x00,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
        0x00, 0x00, 0x10, 0x82
    };

    // Invalid: WLAN ID = 0 (below minimum)
    uint8_t data_invalid_zero[] = {
        0x04, 0x0C, 0x00, 0x0E,
        0x01, 0x00, 0x64, 0x00,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
        0x00, 0x00, 0x00, 0x82
    };

    // Invalid: WLAN ID = 17 (above maximum)
    uint8_t data_invalid_high[] = {
        0x04, 0x0C, 0x00, 0x0E,
        0x01, 0x00, 0x64, 0x00,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
        0x00, 0x00, 0x11, 0x82
    };
    // clang-format on

    RawData raw_data = { data_valid_min, data_valid_min + sizeof(data_valid_min) };
    CHECK(Station::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_max, data_valid_max + sizeof(data_valid_max) };
    CHECK(Station::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid_zero, data_invalid_zero + sizeof(data_invalid_zero) };
    CHECK(Station::Deserialize(&raw_data) == nullptr);

    raw_data = { data_invalid_high, data_invalid_high + sizeof(data_invalid_high) };
    CHECK(Station::Deserialize(&raw_data) == nullptr);
}

TEST(StationTestsGroup, Minimum_supported_rates) {
    // Minimum of 1 byte for supported rates
    uint8_t buffer[256] = {};
    uint8_t mac[6] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    std::vector<uint8_t> rates = { 0x82 };

    WritableStationArray::Item item{ 1, 100, 0x00, mac, 0x1234, 5, rates };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    item.header.Serialize(&raw_data);
    memcpy(raw_data.current, item.supported_rates_data.data(), item.supported_rates_data.size());
    raw_data.current += item.supported_rates_data.size();

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = Station::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(1, deserialized->GetSupportedRatesLength());
}

TEST(StationTestsGroup, Association_ID_values) {
    uint8_t buffer[256] = {};
    uint8_t mac[6] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    std::vector<uint8_t> rates = { 0x82, 0x84 };

    // Test with various Association IDs
    uint16_t assoc_ids[] = { 0, 1, 100, 1000, 0xFFFF };

    for (auto assoc_id : assoc_ids) {
        std::vector<uint8_t> rates_copy = { 0x82, 0x84 };
        WritableStationArray::Item item{ 1, assoc_id, 0x00, mac, 0x1234, 5, rates_copy };
        RawData raw_data{ buffer, buffer + sizeof(buffer) };

        item.header.Serialize(&raw_data);
        memcpy(raw_data.current,
               item.supported_rates_data.data(),
               item.supported_rates_data.size());
        raw_data.current += item.supported_rates_data.size();

        raw_data = { buffer, buffer + sizeof(buffer) };
        auto deserialized = Station::Deserialize(&raw_data);
        CHECK(deserialized != nullptr);
        CHECK_EQUAL(assoc_id, deserialized->GetAssociationID());
    }
}
