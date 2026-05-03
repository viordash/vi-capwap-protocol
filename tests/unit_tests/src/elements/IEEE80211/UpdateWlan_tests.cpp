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
    CHECK_FALSE(r_wlans.IsPresent());

    raw_data = { buffer, buffer + 42 };

    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_TRUE(r_wlans.IsPresent());
    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_wlans.Get().size());

    CHECK_EQUAL(1, r_wlans.Get()[0]->GetRadioID());
    CHECK_EQUAL(1, r_wlans.Get()[0]->GetWlanID());
    CHECK_EQUAL(4, r_wlans.Get()[0]->GetKeyLength());
    CHECK_EQUAL(4 + 8, r_wlans.Get()[0]->GetLength());
    MEMCMP_EQUAL(key_0.data(), r_wlans.Get()[0]->data, 4);

    CHECK_EQUAL(1, r_wlans.Get()[1]->GetRadioID());
    CHECK_EQUAL(2, r_wlans.Get()[1]->GetWlanID());
    CHECK_EQUAL(0, r_wlans.Get()[1]->GetKeyLength());
    CHECK_EQUAL(0 + 8, r_wlans.Get()[1]->GetLength());

    CHECK_EQUAL(2, r_wlans.Get()[2]->GetRadioID());
    CHECK_EQUAL(1, r_wlans.Get()[2]->GetWlanID());
    CHECK_EQUAL(2, r_wlans.Get()[2]->GetKeyLength());
    CHECK_EQUAL(2 + 8, r_wlans.Get()[2]->GetLength());
    MEMCMP_EQUAL(key_2.data(), r_wlans.Get()[2]->data, 2);
}

TEST(UpdateWlanTestsGroup, Add_array_of_items_is_unique_by_radio_and_wlan_id) {
    uint8_t buffer[2048] = {};

    WritableUpdateWlanArray w_wlans;

    // Add same RadioID + WlanID multiple times - should replace
    std::vector<uint8_t> key_0 = { 0x01, 0x02, 0x03, 0x04 };
    w_wlans.Add({ 1, 1, 0x0001, 0, UpdateWlan::PerStation, key_0 });

    std::vector<uint8_t> key_1 = { 0xAA, 0xBB };
    w_wlans.Add({ 1, 1, 0x0002, 0, UpdateWlan::SharedWEP, key_1 });

    // Different WlanID - should be a new entry
    std::vector<uint8_t> key_2 = { 0xCC, 0xDD, 0xEE };
    w_wlans.Add({ 1, 2, 0x0003, 0, UpdateWlan::PerStation, key_2 });

    std::vector<uint8_t> key_3 = { 0xFF };
    w_wlans.Add({ 1, 2, 0x0004, 0, UpdateWlan::CompletedRekeying, key_3 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_wlans.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableUpdateWlanArray r_wlans;
    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_FALSE(r_wlans.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_wlans.Get().size());

    // First entry should have the replaced values
    CHECK_EQUAL(1, r_wlans.Get()[0]->GetRadioID());
    CHECK_EQUAL(1, r_wlans.Get()[0]->GetWlanID());
    CHECK_EQUAL(0x0002, r_wlans.Get()[0]->GetCapability());
    CHECK_EQUAL(UpdateWlan::SharedWEP, r_wlans.Get()[0]->GetKeyStatus());
    CHECK_EQUAL(2, r_wlans.Get()[0]->GetKeyLength());
    MEMCMP_EQUAL(key_1.data(), r_wlans.Get()[0]->data, 2);

    // Second entry should have the replaced values
    CHECK_EQUAL(1, r_wlans.Get()[1]->GetRadioID());
    CHECK_EQUAL(2, r_wlans.Get()[1]->GetWlanID());
    CHECK_EQUAL(0x0004, r_wlans.Get()[1]->GetCapability());
    CHECK_EQUAL(UpdateWlan::CompletedRekeying, r_wlans.Get()[1]->GetKeyStatus());
    CHECK_EQUAL(1, r_wlans.Get()[1]->GetKeyLength());
    MEMCMP_EQUAL(key_3.data(), r_wlans.Get()[1]->data, 1);
}
