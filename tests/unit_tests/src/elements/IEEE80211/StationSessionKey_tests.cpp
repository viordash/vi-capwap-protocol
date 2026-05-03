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
