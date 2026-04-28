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

TEST(StationSessionKeyTestsGroup, Deserialize_with_key) {
    // clang-format off
    uint8_t data[] = {
        // ---- Message Element Header (4 bytes) ----
        0x04, 0x0E,     // Type: 1038 (IEEE 802.11 Station Session Key)
        0x00, 0x30,     // Length: 48 bytes (20 fixed + 28 key)

        // ---- Value (48 bytes) ----
        // MAC Address (6 bytes)
        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,

        // Flags (2 bytes): A=1, C=0, Reserved=0
        0x80, 0x00,

        // Pairwise TSC (6 bytes)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01,

        // Pairwise RSC (6 bytes)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x02,

        // Key (28 bytes) - example TKIP key (256 bits = 32 bytes, but using 28 for test)
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
        0x19, 0x1A, 0x1B, 0x1C
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = StationSessionKey::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::StationSessionKey, element->GetElementType());

    CHECK_EQUAL(0xAA, element->GetMACAddress()[0]);
    CHECK_EQUAL(0xBB, element->GetMACAddress()[1]);
    CHECK_EQUAL(0xCC, element->GetMACAddress()[2]);
    CHECK_EQUAL(0xDD, element->GetMACAddress()[3]);
    CHECK_EQUAL(0xEE, element->GetMACAddress()[4]);
    CHECK_EQUAL(0xFF, element->GetMACAddress()[5]);

    CHECK_EQUAL(0x8000, element->GetFlags());
    CHECK_TRUE(element->GetAKMOnlyFlag());
    CHECK_FALSE(element->GetACCryptoFlag());

    CHECK_EQUAL(0x01, element->GetPairwiseTSC()[5]);
    CHECK_EQUAL(0x02, element->GetPairwiseRSC()[5]);

    CHECK_EQUAL(28, element->GetKeyLength());
    CHECK_EQUAL(0x01, element->key_data[0]);
    CHECK_EQUAL(0x1C, element->key_data[27]);
}

TEST(StationSessionKeyTestsGroup, Deserialize_without_key) {
    // clang-format off
    uint8_t data[] = {
        // ---- Message Element Header (4 bytes) ----
        0x04, 0x0E,     // Type: 1038
        0x00, 0x14,     // Length: 20 bytes (no key)

        // ---- Value (20 bytes) ----
        // MAC Address (6 bytes)
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66,

        // Flags (2 bytes): A=0, C=1
        0x40, 0x00,

        // Pairwise TSC (6 bytes)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

        // Pairwise RSC (6 bytes)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = StationSessionKey::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);

    CHECK_EQUAL(0x11, element->GetMACAddress()[0]);
    CHECK_EQUAL(0x4000, element->GetFlags());
    CHECK_FALSE(element->GetAKMOnlyFlag());
    CHECK_TRUE(element->GetACCryptoFlag());
    CHECK_EQUAL(0, element->GetKeyLength());
}

TEST(StationSessionKeyTestsGroup, Serialize_with_key) {
    uint8_t buffer[256] = {};
    uint8_t mac[6] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    uint8_t tsc[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
    uint8_t rsc[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 };
    std::vector<uint8_t> key = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
                                 0x0C, 0x0D, 0x0E, 0x0F, 0x10 };

    WritableStationSessionKeyArray::Item item{ mac, 0x8000, tsc, rsc, std::move(key) };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    item.header.Serialize(&raw_data);
    memcpy(raw_data.current, item.key_data.data(), item.key_data.size());
    raw_data.current += item.key_data.size();

    CHECK_EQUAL(&buffer[0] + 40, raw_data.current); // 24 header + 16 key

    // clang-format off
    const uint8_t reference[] = {
        0x04, 0x0E,     // Type: 1038
        0x00, 0x24,     // Length: 36 (20 fixed + 16 key)

        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,  // MAC
        0x80, 0x00,                          // Flags
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01,  // TSC
        0x00, 0x00, 0x00, 0x00, 0x00, 0x02,  // RSC
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = StationSessionKey::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(&buffer[0] + 40, raw_data.current);
    CHECK_EQUAL(0xAA, deserialized->GetMACAddress()[0]);
    CHECK_EQUAL(0x8000, deserialized->GetFlags());
    CHECK_EQUAL(16, deserialized->GetKeyLength());
}

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
    w_keys.Add({ mac1, 0x8000, tsc1, rsc1, std::move(key1) });
    w_keys.Add({ mac2, 0x4000, tsc2, rsc2, std::move(key2) });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_keys.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 60, raw_data.current); // (24+4) + (24+8) = 60

    ReadableStationSessionKeyArray r_keys;
    raw_data = { buffer, buffer + 60 };

    CHECK_TRUE(r_keys.Deserialize(&raw_data));
    CHECK_TRUE(r_keys.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_keys.Get().size());

    CHECK_EQUAL(0x11, r_keys.Get()[0]->GetMACAddress()[0]);
    CHECK_EQUAL(0x8000, r_keys.Get()[0]->GetFlags());
    CHECK_TRUE(r_keys.Get()[0]->GetAKMOnlyFlag());
    CHECK_EQUAL(4, r_keys.Get()[0]->GetKeyLength());

    CHECK_EQUAL(0xAA, r_keys.Get()[1]->GetMACAddress()[0]);
    CHECK_EQUAL(0x4000, r_keys.Get()[1]->GetFlags());
    CHECK_TRUE(r_keys.Get()[1]->GetACCryptoFlag());
    CHECK_EQUAL(8, r_keys.Get()[1]->GetKeyLength());
}

TEST(StationSessionKeyTestsGroup, Validate_flags_reserved_bits) {
    // clang-format off
    // Valid: A=1, C=0, reserved bits = 0
    uint8_t data_valid[] = {
        0x04, 0x0E, 0x00, 0x14,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
        0x80, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    // Invalid: reserved bit 0 is set
    uint8_t data_invalid_reserved[] = {
        0x04, 0x0E, 0x00, 0x14,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
        0x00, 0x01,  // Reserved bit 0 set
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    // clang-format on

    RawData raw_data = { data_valid, data_valid + sizeof(data_valid) };
    CHECK(StationSessionKey::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid_reserved, data_invalid_reserved + sizeof(data_invalid_reserved) };
    CHECK(StationSessionKey::Deserialize(&raw_data) == nullptr);
}

TEST(StationSessionKeyTestsGroup, Both_flags_set) {
    // clang-format off
    uint8_t data[] = {
        0x04, 0x0E, 0x00, 0x14,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
        0xC0, 0x00,  // Both A and C set
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    // clang-format on

    RawData raw_data{ data, data + sizeof(data) };
    auto element = StationSessionKey::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(0xC000, element->GetFlags());
    CHECK_TRUE(element->GetAKMOnlyFlag());
    CHECK_TRUE(element->GetACCryptoFlag());
}

TEST(StationSessionKeyTestsGroup, CCMP_key_128bit) {
    // Test with 128-bit CCMP key (16 bytes)
    uint8_t buffer[256] = {};
    uint8_t mac[6] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    uint8_t tsc[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
    uint8_t rsc[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 };

    // 128-bit CCMP key
    std::vector<uint8_t> ccmp_key = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
                                      0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10 };

    WritableStationSessionKeyArray::Item item{ mac, 0x0000, tsc, rsc, std::move(ccmp_key) };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    item.header.Serialize(&raw_data);
    memcpy(raw_data.current, item.key_data.data(), item.key_data.size());
    raw_data.current += item.key_data.size();

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = StationSessionKey::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(16, deserialized->GetKeyLength());
}

TEST(StationSessionKeyTestsGroup, TKIP_key_256bit) {
    // Test with 256-bit TKIP key (32 bytes)
    uint8_t buffer[256] = {};
    uint8_t mac[6] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    uint8_t tsc[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
    uint8_t rsc[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 };

    // 256-bit TKIP key
    std::vector<uint8_t> tkip_key = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                      0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
                                      0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                                      0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20 };

    WritableStationSessionKeyArray::Item item{ mac, 0x0000, tsc, rsc, std::move(tkip_key) };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    item.header.Serialize(&raw_data);
    memcpy(raw_data.current, item.key_data.data(), item.key_data.size());
    raw_data.current += item.key_data.size();

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = StationSessionKey::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(32, deserialized->GetKeyLength());
}
