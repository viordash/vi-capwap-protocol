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
    CHECK_FALSE(r_bssids.IsPresent());

    raw_data = { reference, reference + sizeof(reference) };

    CHECK_TRUE(r_bssids.Deserialize(&raw_data));
    CHECK_TRUE(r_bssids.IsPresent());
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
    CHECK_FALSE(r_bssids.IsPresent());

    CHECK_TRUE(r_bssids.Deserialize(&raw_data));
    CHECK_TRUE(r_bssids.IsPresent());
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