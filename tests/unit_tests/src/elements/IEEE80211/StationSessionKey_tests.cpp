#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/StationSessionKey.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(StationSessionKeyTestsGroup){ //
                                         TEST_SETUP(){}

                                         TEST_TEARDOWN(){}
};

TEST(StationSessionKeyTestsGroup, Serialize_Deserialize_array) {
    uint8_t buffer[2048] = {};
    uint8_t mac1[6] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
    uint8_t mac2[6] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    uint8_t tsc1[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
    uint8_t tsc2[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 };
    uint8_t rsc1[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x10 };
    uint8_t rsc2[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x20 };

    std::vector<uint8_t> key1 = { 0x01, 0x02, 0x03, 0x04 };
    std::vector<uint8_t> key2 = { 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80 };

    WritableStationSessionKeyArray w_keys;
    w_keys.Add({ mac1, 0x8000, tsc1, rsc1, key1 });
    w_keys.Add({ mac2, 0x4000, tsc2, rsc2, key2 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_keys.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 60, raw_data.current); // (24+4) + (24+8) = 60

    ReadableStationSessionKeyArray r_keys;
    CHECK_FALSE(r_keys.IsPresent());

    raw_data = { buffer, buffer + 60 };

    CHECK_TRUE(r_keys.Deserialize(&raw_data));
    CHECK_TRUE(r_keys.IsPresent());
    CHECK_TRUE(r_keys.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_keys.Get().size());

    CHECK_EQUAL(0x11, r_keys.Get()[0]->GetMACAddress()[0]);
    CHECK_EQUAL(0x8000, r_keys.Get()[0]->GetFlags());
    CHECK_TRUE(r_keys.Get()[0]->GetAKMOnlyFlag());
    CHECK_EQUAL(4, r_keys.Get()[0]->GetKeyLength());
    CHECK_EQUAL(4 + 20, r_keys.Get()[0]->GetLength());
    MEMCMP_EQUAL(key1.data(), r_keys.Get()[0]->data, 4);
    MEMCMP_EQUAL(mac1, r_keys.Get()[0]->GetMACAddress(), 6);

    CHECK_EQUAL(0xAA, r_keys.Get()[1]->GetMACAddress()[0]);
    CHECK_EQUAL(0x4000, r_keys.Get()[1]->GetFlags());
    CHECK_TRUE(r_keys.Get()[1]->GetACCryptoFlag());
    CHECK_EQUAL(8, r_keys.Get()[1]->GetKeyLength());
    CHECK_EQUAL(8 + 20, r_keys.Get()[1]->GetLength());
    MEMCMP_EQUAL(key2.data(), r_keys.Get()[1]->data, 4);
    MEMCMP_EQUAL(mac2, r_keys.Get()[1]->GetMACAddress(), 6);
}

TEST(StationSessionKeyTestsGroup, Add_array_of_items_is_unique_by_mac_address) {
    uint8_t buffer[2048] = {};

    WritableStationSessionKeyArray w_keys;

    // Add same MAC address multiple times - should replace
    const uint8_t mac_0[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 };
    const uint8_t tsc_0[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    const uint8_t rsc_0[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    std::vector<uint8_t> key_0 = { 0xAA, 0xBB, 0xCC, 0xDD };
    w_keys.Add({ mac_0, 0x0000, tsc_0, rsc_0, key_0 });

    std::vector<uint8_t> key_1 = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
    w_keys.Add({ mac_0, 0x8000, tsc_0, rsc_0, key_1 });

    const uint8_t mac_1[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    std::vector<uint8_t> key_2 = { 0xFF, 0xEE };
    w_keys.Add({ mac_1, 0x4000, tsc_0, rsc_0, key_2 });

    std::vector<uint8_t> key_3 = { 0x01, 0x02, 0x03 };
    w_keys.Add({ mac_1, 0xC000, tsc_0, rsc_0, key_3 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_keys.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableStationSessionKeyArray r_keys;
    CHECK_FALSE(r_keys.IsPresent());

    CHECK_TRUE(r_keys.Deserialize(&raw_data));
    CHECK_TRUE(r_keys.IsPresent());
    CHECK_TRUE(r_keys.Deserialize(&raw_data));
    CHECK_FALSE(r_keys.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_keys.Get().size());

    // Should have the last values for each MAC address
    MEMCMP_EQUAL(mac_0, r_keys.Get()[0]->GetMACAddress(), 6);
    CHECK_EQUAL(0x8000, r_keys.Get()[0]->GetFlags());
    CHECK_EQUAL(6, r_keys.Get()[0]->GetKeyLength());
    MEMCMP_EQUAL(key_1.data(), r_keys.Get()[0]->data, 6);

    MEMCMP_EQUAL(mac_1, r_keys.Get()[1]->GetMACAddress(), 6);
    CHECK_EQUAL(0xC000, r_keys.Get()[1]->GetFlags());
    CHECK_EQUAL(3, r_keys.Get()[1]->GetKeyLength());
    MEMCMP_EQUAL(key_3.data(), r_keys.Get()[1]->data, 3);
}
