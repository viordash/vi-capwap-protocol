#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/AssignedWtpBssid.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(AssignedWtpBssidTestsGroup){ //
                                        TEST_SETUP(){}

                                        TEST_TEARDOWN(){}
};

TEST(AssignedWtpBssidTestsGroup, Deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Message Element Header (4 bytes) ----
        0x04, 0x02,     // Type: 1026 (IEEE 802.11 Assigned WTP BSSID)
        0x00, 0x08,     // Length: 8 bytes

        // ---- Value (8 bytes) ----
        0x01,           // Radio ID: 1
        0x02,           // WLAN ID: 2
        0xAA, 0xBB,     // BSSID: AA:BB:CC:DD:EE:FF
        0xCC, 0xDD,
        0xEE, 0xFF
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = AssignedWtpBssid::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::AssignedWtpBssid, element->GetElementType());

    CHECK_EQUAL(1, element->RadioID);
    CHECK_EQUAL(2, element->WlanID);
    const uint8_t expected_bssid[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    MEMCMP_EQUAL(expected_bssid, element->BSSID, sizeof(expected_bssid));
}

TEST(AssignedWtpBssidTestsGroup, Serialize) {
    uint8_t buffer[256] = {};
    const uint8_t bssid[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    AssignedWtpBssid element_0{ 5, 3, bssid };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 12, raw_data.current);
    const uint8_t reference[] = { 0x04, 0x02, 0x00, 0x08, 0x05, 0x03,
                                  0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = AssignedWtpBssid::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(&buffer[0] + 12, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::AssignedWtpBssid, element->GetElementType());
    CHECK_EQUAL(5, element->RadioID);
    CHECK_EQUAL(3, element->WlanID);
    MEMCMP_EQUAL(bssid, element->BSSID, sizeof(bssid));
}

TEST(AssignedWtpBssidTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableAssignedWtpBssidArray w_bssids;

    const uint8_t bssid_0[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 };
    const uint8_t bssid_1[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    const uint8_t bssid_2[] = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC };

    w_bssids.Add({ 1, 1, bssid_0 });
    w_bssids.Add({ 1, 2, bssid_1 });
    w_bssids.Add({ 2, 1, bssid_2 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_bssids.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 36, raw_data.current);

    // clang-format off
    uint8_t reference[] = {
        // Element 0: RadioID=1, WlanID=1
        0x04, 0x02, 0x00, 0x08, 0x01, 0x01, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
        // Element 1: RadioID=1, WlanID=2
        0x04, 0x02, 0x00, 0x08, 0x01, 0x02, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
        // Element 2: RadioID=2, WlanID=1
        0x04, 0x02, 0x00, 0x08, 0x02, 0x01, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    ReadableAssignedWtpBssidArray r_bssids;

    raw_data = { reference, reference + sizeof(reference) };

    CHECK_TRUE(r_bssids.Deserialize(&raw_data));
    CHECK_TRUE(r_bssids.Deserialize(&raw_data));
    CHECK_TRUE(r_bssids.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_bssids.Get().size());

    CHECK_EQUAL(1, r_bssids.Get()[0]->RadioID);
    CHECK_EQUAL(1, r_bssids.Get()[0]->WlanID);
    MEMCMP_EQUAL(bssid_0, r_bssids.Get()[0]->BSSID, sizeof(bssid_0));

    CHECK_EQUAL(1, r_bssids.Get()[1]->RadioID);
    CHECK_EQUAL(2, r_bssids.Get()[1]->WlanID);
    MEMCMP_EQUAL(bssid_1, r_bssids.Get()[1]->BSSID, sizeof(bssid_1));

    CHECK_EQUAL(2, r_bssids.Get()[2]->RadioID);
    CHECK_EQUAL(1, r_bssids.Get()[2]->WlanID);
    MEMCMP_EQUAL(bssid_2, r_bssids.Get()[2]->BSSID, sizeof(bssid_2));
}

TEST(AssignedWtpBssidTestsGroup, Add_array_of_items_is_unique) {
    uint8_t buffer[2048] = {};

    WritableAssignedWtpBssidArray w_bssids;

    const uint8_t bssid_0[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 };
    const uint8_t bssid_1[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

    // Add same RadioID+WlanID multiple times
    w_bssids.Add({ 1, 1, bssid_0 });
    w_bssids.Add({ 1, 1, bssid_0 });
    w_bssids.Add({ 1, 1, bssid_0 });
    w_bssids.Add({ 2, 2, bssid_1 });
    w_bssids.Add({ 2, 2, bssid_1 });
    w_bssids.Add({ 2, 2, bssid_1 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_bssids.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableAssignedWtpBssidArray r_bssids;

    CHECK_TRUE(r_bssids.Deserialize(&raw_data));
    CHECK_TRUE(r_bssids.Deserialize(&raw_data));
    CHECK_FALSE(r_bssids.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_bssids.Get().size());

    CHECK_EQUAL(1, r_bssids.Get()[0]->RadioID);
    CHECK_EQUAL(1, r_bssids.Get()[0]->WlanID);
    MEMCMP_EQUAL(bssid_0, r_bssids.Get()[0]->BSSID, sizeof(bssid_0));

    CHECK_EQUAL(2, r_bssids.Get()[1]->RadioID);
    CHECK_EQUAL(2, r_bssids.Get()[1]->WlanID);
    MEMCMP_EQUAL(bssid_1, r_bssids.Get()[1]->BSSID, sizeof(bssid_1));
}

TEST(AssignedWtpBssidTestsGroup, Validate_RadioID_range) {
    // clang-format off
    // Valid RadioID = 1
    uint8_t data_valid_min[] = {
        0x04, 0x02, 0x00, 0x08, 0x01, 0x01, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
    };
    // Valid RadioID = 31
    uint8_t data_valid_max[] = {
        0x04, 0x02, 0x00, 0x08, 0x1F, 0x01, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
    };
    // Invalid RadioID = 32
    uint8_t data_invalid_high[] = {
        0x04, 0x02, 0x00, 0x08, 0x20, 0x01, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
    };
    // clang-format on

    RawData raw_data = { data_valid_min, data_valid_min + sizeof(data_valid_min) };
    CHECK(AssignedWtpBssid::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_max, data_valid_max + sizeof(data_valid_max) };
    CHECK(AssignedWtpBssid::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid_high, data_invalid_high + sizeof(data_invalid_high) };
    CHECK(AssignedWtpBssid::Deserialize(&raw_data) == nullptr);
}

TEST(AssignedWtpBssidTestsGroup, Validate_WlanID_range) {
    // clang-format off
    // Valid WlanID = 1
    uint8_t data_valid_min[] = {
        0x04, 0x02, 0x00, 0x08, 0x01, 0x01, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
    };
    // Valid WlanID = 16
    uint8_t data_valid_max[] = {
        0x04, 0x02, 0x00, 0x08, 0x01, 0x10, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
    };
    // Invalid WlanID = 0
    uint8_t data_invalid_zero[] = {
        0x04, 0x02, 0x00, 0x08, 0x01, 0x00, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
    };
    // Invalid WlanID = 17
    uint8_t data_invalid_high[] = {
        0x04, 0x02, 0x00, 0x08, 0x01, 0x11, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
    };
    // clang-format on

    RawData raw_data = { data_valid_min, data_valid_min + sizeof(data_valid_min) };
    CHECK(AssignedWtpBssid::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_max, data_valid_max + sizeof(data_valid_max) };
    CHECK(AssignedWtpBssid::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid_zero, data_invalid_zero + sizeof(data_invalid_zero) };
    CHECK(AssignedWtpBssid::Deserialize(&raw_data) == nullptr);

    raw_data = { data_invalid_high, data_invalid_high + sizeof(data_invalid_high) };
    CHECK(AssignedWtpBssid::Deserialize(&raw_data) == nullptr);
}
