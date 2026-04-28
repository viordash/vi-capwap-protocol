#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/StationQoSProfile.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(StationQoSProfileTestsGroup){ //
                                         TEST_SETUP(){}

                                         TEST_TEARDOWN(){}
};

TEST(StationQoSProfileTestsGroup, Deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Message Element Header (4 bytes) ----
        0x04, 0x0D,     // Type: 1037 (IEEE 802.11 Station QoS Profile)
        0x00, 0x08,     // Length: 8 bytes

        // ---- Value (8 bytes) ----
        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,  // MAC Address
        0x00,                                 // Reserved
        0x05                                  // 8021p priority: 5
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = StationQoSProfile::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::StationQoSProfile, element->GetElementType());

    CHECK_EQUAL(0xAA, element->GetMACAddress()[0]);
    CHECK_EQUAL(0xBB, element->GetMACAddress()[1]);
    CHECK_EQUAL(0xCC, element->GetMACAddress()[2]);
    CHECK_EQUAL(0xDD, element->GetMACAddress()[3]);
    CHECK_EQUAL(0xEE, element->GetMACAddress()[4]);
    CHECK_EQUAL(0xFF, element->GetMACAddress()[5]);
    CHECK_EQUAL(5, element->Get8021pPriority());
}

TEST(StationQoSProfileTestsGroup, Serialize) {
    uint8_t buffer[256] = {};
    uint8_t mac[6] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
    StationQoSProfile element{ mac, 7 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 12, raw_data.current);

    // clang-format off
    const uint8_t reference[] = {
        0x04, 0x0D,     // Type: 1037
        0x00, 0x08,     // Length: 8

        0x11, 0x22, 0x33, 0x44, 0x55, 0x66,  // MAC
        0x00,                                 // Reserved
        0x07                                  // 8021p: 7
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = StationQoSProfile::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(&buffer[0] + 12, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::StationQoSProfile, deserialized->GetElementType());
    CHECK_EQUAL(0x11, deserialized->GetMACAddress()[0]);
    CHECK_EQUAL(7, deserialized->Get8021pPriority());
}

TEST(StationQoSProfileTestsGroup, Serialize_Deserialize_array) {
    uint8_t buffer[2048] = {};
    uint8_t mac1[6] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
    uint8_t mac2[6] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    uint8_t mac3[6] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 };

    WritableStationQoSProfileArray w_profiles;
    w_profiles.Add({ mac1, 0 });
    w_profiles.Add({ mac2, 3 });
    w_profiles.Add({ mac3, 7 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_profiles.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 36, raw_data.current); // 3 * 12 = 36

    ReadableStationQoSProfileArray r_profiles;
    raw_data = { buffer, buffer + 36 };

    CHECK_TRUE(r_profiles.Deserialize(&raw_data));
    CHECK_TRUE(r_profiles.Deserialize(&raw_data));
    CHECK_TRUE(r_profiles.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_profiles.Get().size());

    CHECK_EQUAL(0x11, r_profiles.Get()[0]->GetMACAddress()[0]);
    CHECK_EQUAL(0, r_profiles.Get()[0]->Get8021pPriority());

    CHECK_EQUAL(0xAA, r_profiles.Get()[1]->GetMACAddress()[0]);
    CHECK_EQUAL(3, r_profiles.Get()[1]->Get8021pPriority());

    CHECK_EQUAL(0x00, r_profiles.Get()[2]->GetMACAddress()[0]);
    CHECK_EQUAL(7, r_profiles.Get()[2]->Get8021pPriority());
}

TEST(StationQoSProfileTestsGroup, Validate_priority_range) {
    // clang-format off
    // Valid: priority = 0 (minimum)
    uint8_t data_valid_min[] = {
        0x04, 0x0D, 0x00, 0x08,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
        0x00, 0x00
    };

    // Valid: priority = 7 (maximum)
    uint8_t data_valid_max[] = {
        0x04, 0x0D, 0x00, 0x08,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
        0x00, 0x07
    };

    // Invalid: priority = 8 (exceeds maximum)
    uint8_t data_invalid[] = {
        0x04, 0x0D, 0x00, 0x08,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
        0x00, 0x08
    };
    // clang-format on

    RawData raw_data = { data_valid_min, data_valid_min + sizeof(data_valid_min) };
    CHECK(StationQoSProfile::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_max, data_valid_max + sizeof(data_valid_max) };
    CHECK(StationQoSProfile::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid, data_invalid + sizeof(data_invalid) };
    CHECK(StationQoSProfile::Deserialize(&raw_data) == nullptr);
}

TEST(StationQoSProfileTestsGroup, All_priority_values) {
    uint8_t buffer[256] = {};
    uint8_t mac[6] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

    // Test all valid priority values (0-7)
    for (uint8_t priority = 0; priority <= 7; priority++) {
        StationQoSProfile element{ mac, priority };
        RawData raw_data{ buffer, buffer + sizeof(buffer) };

        element.Serialize(&raw_data);

        raw_data = { buffer, buffer + sizeof(buffer) };
        auto deserialized = StationQoSProfile::Deserialize(&raw_data);
        CHECK(deserialized != nullptr);
        CHECK_EQUAL(priority, deserialized->Get8021pPriority());
    }
}

TEST(StationQoSProfileTestsGroup, Different_MAC_addresses) {
    uint8_t buffer[256] = {};

    // Test with various MAC addresses
    uint8_t mac1[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    uint8_t mac2[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    uint8_t mac3[6] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB };

    StationQoSProfile element1{ mac1, 0 };
    StationQoSProfile element2{ mac2, 3 };
    StationQoSProfile element3{ mac3, 7 };

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    element1.Serialize(&raw_data);

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = StationQoSProfile::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(0x00, deserialized->GetMACAddress()[0]);
    CHECK_EQUAL(0x00, deserialized->GetMACAddress()[5]);

    raw_data = { buffer, buffer + sizeof(buffer) };
    element2.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    deserialized = StationQoSProfile::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(0xFF, deserialized->GetMACAddress()[0]);
    CHECK_EQUAL(0xFF, deserialized->GetMACAddress()[5]);

    raw_data = { buffer, buffer + sizeof(buffer) };
    element3.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    deserialized = StationQoSProfile::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(0x01, deserialized->GetMACAddress()[0]);
    CHECK_EQUAL(0xAB, deserialized->GetMACAddress()[5]);
}
