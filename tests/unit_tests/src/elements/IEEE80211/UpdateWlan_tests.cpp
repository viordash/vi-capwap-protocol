#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/UpdateWlan.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(UpdateWlanTestsGroup){ //
                                  TEST_SETUP(){}

                                  TEST_TEARDOWN(){}
};

TEST(UpdateWlanTestsGroup, Deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Message Element Header (4 bytes) ----
        0x04, 0x14,     // Type: 1044 (IEEE 802.11 Update WLAN)
        0x00, 0x0C,     // Length: 12 bytes (8 fixed + 4 key)

        // ---- Value (12 bytes) ----
        0x01,           // Radio ID: 1
        0x02,           // WLAN ID: 2
        0x00, 0x11,     // Capability: 0x0011
        0x01,           // Key Index: 1
        0x03,           // Key Status: Completed Rekeying
        0x00, 0x04,     // Key Length: 4
        0xAA, 0xBB, 0xCC, 0xDD  // Key
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = UpdateWlan::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::UpdateWlan, element->GetElementType());

    CHECK_EQUAL(1, element->GetRadioID());
    CHECK_EQUAL(2, element->GetWlanID());
    CHECK_EQUAL(0x0011, element->GetCapability());
    CHECK_EQUAL(1, element->GetKeyIndex());
    CHECK_EQUAL(UpdateWlan::CompletedRekeying, element->GetKeyStatus());
    CHECK_EQUAL(4, element->GetKeyLength());
    const uint8_t expected_key[] = { 0xAA, 0xBB, 0xCC, 0xDD };
    MEMCMP_EQUAL(expected_key, element->key, sizeof(expected_key));
}

TEST(UpdateWlanTestsGroup, Deserialize_no_key) {
    // clang-format off
    uint8_t data[] = {
        0x04, 0x14,     // Type: 1044
        0x00, 0x08,     // Length: 8 bytes (no key)

        0x01,           // Radio ID: 1
        0x01,           // WLAN ID: 1
        0x00, 0x00,     // Capability: 0
        0x00,           // Key Index: 0
        0x00,           // Key Status: Per-Station Request
        0x00, 0x00      // Key Length: 0
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = UpdateWlan::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(0, element->GetKeyLength());
}

TEST(UpdateWlanTestsGroup, Serialize) {
    uint8_t buffer[256] = {};
    WritableUpdateWlanArray w_wlans;

    std::vector<uint8_t> key = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };
    w_wlans.Add({ 5, 3, 0x0421, 2, UpdateWlan::BeginRekeying, key });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_wlans.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 20, raw_data.current); // 4 header + 8 fixed + 8 key = 20

    // clang-format off
    const uint8_t reference[] = {
        0x04, 0x14,     // Type: 1044
        0x00, 0x10,     // Length: 16 (8 + 8)
        0x05,           // Radio ID: 5
        0x03,           // WLAN ID: 3
        0x04, 0x21,     // Capability: 0x0421
        0x02,           // Key Index: 2
        0x02,           // Key Status: BeginRekeying (2)
        0x00, 0x08,     // Key Length: 8
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88  // Key
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = UpdateWlan::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(&buffer[0] + 20, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::UpdateWlan, deserialized->GetElementType());
    CHECK_EQUAL(5, deserialized->GetRadioID());
    CHECK_EQUAL(3, deserialized->GetWlanID());
    CHECK_EQUAL(0x0421, deserialized->GetCapability());
}

TEST(UpdateWlanTestsGroup, Serialize_no_key) {
    uint8_t buffer[256] = {};
    WritableUpdateWlanArray w_wlans;

    std::vector<uint8_t> empty_key;
    w_wlans.Add({ 1, 1, 0x0000, 0, UpdateWlan::PerStation, empty_key });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_wlans.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 12, raw_data.current); // 4 header + 8 fixed + 0 key = 12

    // clang-format off
    const uint8_t reference[] = {
        0x04, 0x14,     // Type: 1044
        0x00, 0x08,     // Length: 8
        0x01,           // Radio ID: 1
        0x01,           // WLAN ID: 1
        0x00, 0x00,     // Capability: 0
        0x00,           // Key Index: 0
        0x00,           // Key Status: Per-Station Request
        0x00, 0x00      // Key Length: 0
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));
}

TEST(UpdateWlanTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableUpdateWlanArray w_wlans;

    std::vector<uint8_t> key_0 = { 0x01, 0x02, 0x03, 0x04 };
    std::vector<uint8_t> key_1 = {};
    std::vector<uint8_t> key_2 = { 0xAA, 0xBB };

    w_wlans.Add({ 1, 1, 0x0001, 1, UpdateWlan::BeginRekeying, key_0 });
    w_wlans.Add({ 1, 2, 0x0002, 0, UpdateWlan::PerStation, key_1 });
    w_wlans.Add({ 2, 1, 0x0003, 2, UpdateWlan::CompletedRekeying, key_2 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_wlans.Serialize(&raw_data);
    // Element 0: 4 + 8 + 4 = 16 bytes
    // Element 1: 4 + 8 + 0 = 12 bytes
    // Element 2: 4 + 8 + 2 = 14 bytes
    // Total: 42 bytes
    CHECK_EQUAL(&buffer[0] + 42, raw_data.current);

    ReadableUpdateWlanArray r_wlans;

    raw_data = { buffer, buffer + 42 };

    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_wlans.Get().size());

    CHECK_EQUAL(1, r_wlans.Get()[0]->GetRadioID());
    CHECK_EQUAL(1, r_wlans.Get()[0]->GetWlanID());
    CHECK_EQUAL(4, r_wlans.Get()[0]->GetKeyLength());

    CHECK_EQUAL(1, r_wlans.Get()[1]->GetRadioID());
    CHECK_EQUAL(2, r_wlans.Get()[1]->GetWlanID());
    CHECK_EQUAL(0, r_wlans.Get()[1]->GetKeyLength());

    CHECK_EQUAL(2, r_wlans.Get()[2]->GetRadioID());
    CHECK_EQUAL(1, r_wlans.Get()[2]->GetWlanID());
    CHECK_EQUAL(2, r_wlans.Get()[2]->GetKeyLength());
}

TEST(UpdateWlanTestsGroup, Validate_RadioID_range) {
    // clang-format off
    // Valid RadioID = 0
    uint8_t data_valid_zero[] = {
        0x04, 0x14, 0x00, 0x08, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    // Valid RadioID = 31
    uint8_t data_valid_max[] = {
        0x04, 0x14, 0x00, 0x08, 0x1F, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    // Invalid RadioID = 32
    uint8_t data_invalid_high[] = {
        0x04, 0x14, 0x00, 0x08, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    // clang-format on

    RawData raw_data = { data_valid_zero, data_valid_zero + sizeof(data_valid_zero) };
    CHECK(UpdateWlan::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_max, data_valid_max + sizeof(data_valid_max) };
    CHECK(UpdateWlan::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid_high, data_invalid_high + sizeof(data_invalid_high) };
    CHECK(UpdateWlan::Deserialize(&raw_data) == nullptr);
}

TEST(UpdateWlanTestsGroup, Validate_WlanID_range) {
    // clang-format off
    // Valid WlanID = 1
    uint8_t data_valid_min[] = {
        0x04, 0x14, 0x00, 0x08, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    // Valid WlanID = 16
    uint8_t data_valid_max[] = {
        0x04, 0x14, 0x00, 0x08, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    // Invalid WlanID = 0
    uint8_t data_invalid_zero[] = {
        0x04, 0x14, 0x00, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    // Invalid WlanID = 17
    uint8_t data_invalid_high[] = {
        0x04, 0x14, 0x00, 0x08, 0x01, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    // clang-format on

    RawData raw_data = { data_valid_min, data_valid_min + sizeof(data_valid_min) };
    CHECK(UpdateWlan::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_max, data_valid_max + sizeof(data_valid_max) };
    CHECK(UpdateWlan::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid_zero, data_invalid_zero + sizeof(data_invalid_zero) };
    CHECK(UpdateWlan::Deserialize(&raw_data) == nullptr);

    raw_data = { data_invalid_high, data_invalid_high + sizeof(data_invalid_high) };
    CHECK(UpdateWlan::Deserialize(&raw_data) == nullptr);
}

TEST(UpdateWlanTestsGroup, Validate_KeyStatus_range) {
    // clang-format off
    // Valid KeyStatus = 0 (PerStation)
    uint8_t data_status_0[] = {
        0x04, 0x14, 0x00, 0x08, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    // Valid KeyStatus = 3 (CompletedRekeying)
    uint8_t data_status_3[] = {
        0x04, 0x14, 0x00, 0x08, 0x01, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00
    };
    // Invalid KeyStatus = 4
    uint8_t data_status_4[] = {
        0x04, 0x14, 0x00, 0x08, 0x01, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00
    };
    // clang-format on

    RawData raw_data = { data_status_0, data_status_0 + sizeof(data_status_0) };
    CHECK(UpdateWlan::Deserialize(&raw_data) != nullptr);

    raw_data = { data_status_3, data_status_3 + sizeof(data_status_3) };
    CHECK(UpdateWlan::Deserialize(&raw_data) != nullptr);

    raw_data = { data_status_4, data_status_4 + sizeof(data_status_4) };
    CHECK(UpdateWlan::Deserialize(&raw_data) == nullptr);
}

TEST(UpdateWlanTestsGroup, Validate_length_mismatch) {
    // clang-format off
    // Key length says 4 but length field only allows for 2
    uint8_t data_mismatch[] = {
        0x04, 0x14, 0x00, 0x0A, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xAA, 0xBB
    };
    // clang-format on

    RawData raw_data = { data_mismatch, data_mismatch + sizeof(data_mismatch) };
    CHECK(UpdateWlan::Deserialize(&raw_data) == nullptr);
}

TEST(UpdateWlanTestsGroup, Deserialize_insufficient_data) {
    // clang-format off
    uint8_t data[] = {
        0x04, 0x14, 0x00, 0x0C, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xAA  // Missing key bytes
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    CHECK(UpdateWlan::Deserialize(&raw_data) == nullptr);
}
