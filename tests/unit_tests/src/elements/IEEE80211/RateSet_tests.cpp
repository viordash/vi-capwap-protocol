#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/RateSet.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(RateSetTestsGroup){ //
                               TEST_SETUP(){}

                               TEST_TEARDOWN(){}
};

TEST(RateSetTestsGroup, Deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Message Element Header (4 bytes) ----
        0x04, 0x0A,     // Type: 1034 (IEEE 802.11 Rate Set)
        0x00, 0x05,     // Length: 5 bytes

        // ---- Value (5 bytes) ----
        0x03,                               // Radio ID: 3
        0x82, 0x84, 0x8B, 0x96             // Rate Set: 4 bytes
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = RateSet::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::RateSet, element->GetElementType());

    CHECK_EQUAL(3, element->GetRadioID());
    CHECK_EQUAL(4, element->GetRateSetData().size());
    CHECK_EQUAL(0x82, element->GetRateSetData()[0]);
    CHECK_EQUAL(0x84, element->GetRateSetData()[1]);
    CHECK_EQUAL(0x8B, element->GetRateSetData()[2]);
    CHECK_EQUAL(0x96, element->GetRateSetData()[3]);
}

TEST(RateSetTestsGroup, Serialize) {
    uint8_t buffer[256] = {};
    std::vector<uint8_t> rate_set_data = { 0x82, 0x84, 0x8B, 0x96, 0x0C, 0x12 };

    RateSet element{ 5, rate_set_data };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 11, raw_data.current); // 4 header + 1 radio_id + 6 rate_set

    // clang-format off
    const uint8_t reference[] = {
        0x04, 0x0A,     // Type: 1034
        0x00, 0x07,     // Length: 7 (1 + 6)

        0x05,                                       // Radio ID: 5
        0x82, 0x84, 0x8B, 0x96, 0x0C, 0x12        // Rate Set: 6 bytes
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = RateSet::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(&buffer[0] + 11, raw_data.current);
    CHECK_EQUAL(5, deserialized->GetRadioID());
    CHECK_EQUAL(6, deserialized->GetRateSetData().size());
    CHECK_EQUAL(0x82, deserialized->GetRateSetData()[0]);
    CHECK_EQUAL(0x12, deserialized->GetRateSetData()[5]);
}

TEST(RateSetTestsGroup, Serialize_Deserialize_array) {
    uint8_t buffer[2048] = {};
    std::vector<uint8_t> rate_set1 = { 0x82, 0x84 };
    std::vector<uint8_t> rate_set2 = { 0x8B, 0x96, 0x0C, 0x12, 0x18, 0x24, 0x30, 0x48 };

    WritableRateSetArray w_rate_sets;
    w_rate_sets.Add(1, rate_set1);
    w_rate_sets.Add(2, rate_set2);

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_rate_sets.Serialize(&raw_data);
    // First: 4 (header) + 1 (radio_id) + 2 (rate_set) = 7
    // Second: 4 (header) + 1 (radio_id) + 8 (rate_set) = 13
    // Total: 7 + 13 = 20
    CHECK_EQUAL(&buffer[0] + 20, raw_data.current);

    ReadableRateSetArray r_rate_sets;
    raw_data = { buffer, buffer + 20 };

    CHECK_TRUE(r_rate_sets.Deserialize(&raw_data));
    CHECK_TRUE(r_rate_sets.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_rate_sets.Get().size());

    CHECK_EQUAL(1, r_rate_sets.Get()[0]->GetRadioID());
    CHECK_EQUAL(2, r_rate_sets.Get()[0]->GetRateSetData().size());
    CHECK_EQUAL(0x82, r_rate_sets.Get()[0]->GetRateSetData()[0]);
    CHECK_EQUAL(0x84, r_rate_sets.Get()[0]->GetRateSetData()[1]);

    CHECK_EQUAL(2, r_rate_sets.Get()[1]->GetRadioID());
    CHECK_EQUAL(8, r_rate_sets.Get()[1]->GetRateSetData().size());
    CHECK_EQUAL(0x8B, r_rate_sets.Get()[1]->GetRateSetData()[0]);
    CHECK_EQUAL(0x48, r_rate_sets.Get()[1]->GetRateSetData()[7]);
}

TEST(RateSetTestsGroup, Validate_RadioID_range) {
    uint8_t buffer[256] = {};
    std::vector<uint8_t> rate_set = { 0x82, 0x84 };

    // Valid: RadioID = 0 (minimum)
    RateSet elem_0{ 0, rate_set };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    elem_0.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    CHECK(RateSet::Deserialize(&raw_data) != nullptr);

    // Valid: RadioID = 1
    RateSet elem_1{ 1, rate_set };
    raw_data = { buffer, buffer + sizeof(buffer) };
    elem_1.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    CHECK(RateSet::Deserialize(&raw_data) != nullptr);

    // Valid: RadioID = 31 (maximum)
    RateSet elem_31{ 31, rate_set };
    raw_data = { buffer, buffer + sizeof(buffer) };
    elem_31.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    CHECK(RateSet::Deserialize(&raw_data) != nullptr);

    // Invalid: RadioID = 32 (above maximum)
    RateSet elem_32{ 32, rate_set };
    raw_data = { buffer, buffer + sizeof(buffer) };
    elem_32.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    CHECK(RateSet::Deserialize(&raw_data) == nullptr);
}

TEST(RateSetTestsGroup, Validate_RateSet_length) {
    uint8_t buffer[256] = {};

    // Valid: minimum length (2 bytes)
    std::vector<uint8_t> rate_set_min = { 0x82, 0x84 };
    RateSet elem_min{ 1, rate_set_min };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    elem_min.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    CHECK(RateSet::Deserialize(&raw_data) != nullptr);

    // Valid: maximum length (8 bytes)
    std::vector<uint8_t> rate_set_max = { 0x82, 0x84, 0x8B, 0x96, 0x0C, 0x12, 0x18, 0x24 };
    RateSet elem_max{ 1, rate_set_max };
    raw_data = { buffer, buffer + sizeof(buffer) };
    elem_max.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    CHECK(RateSet::Deserialize(&raw_data) != nullptr);

    // Invalid: length too short (1 byte) - manually create invalid data
    // clang-format off
    uint8_t invalid_short[] = {
        0x04, 0x0A,     // Type: 1034
        0x00, 0x02,     // Length: 2 (1 radio_id + 1 rate_set - too short!)
        0x01,           // Radio ID: 1
        0x82            // Rate Set: only 1 byte (invalid, minimum is 2)
    };
    // clang-format on
    raw_data = { invalid_short, invalid_short + sizeof(invalid_short) };
    CHECK(RateSet::Deserialize(&raw_data) == nullptr);

    // Invalid: length too long (9 bytes) - manually create invalid data
    // clang-format off
    uint8_t invalid_long[] = {
        0x04, 0x0A,     // Type: 1034
        0x00, 0x0A,     // Length: 10 (1 radio_id + 9 rate_set - too long!)
        0x01,           // Radio ID: 1
        0x82, 0x84, 0x8B, 0x96, 0x0C, 0x12, 0x18, 0x24, 0x30  // 9 bytes (invalid, max is 8)
    };
    // clang-format on
    raw_data = { invalid_long, invalid_long + sizeof(invalid_long) };
    CHECK(RateSet::Deserialize(&raw_data) == nullptr);
}

TEST(RateSetTestsGroup, RateSet_values) {
    uint8_t buffer[256] = {};

    // Test with various rate values
    std::vector<uint8_t> rate_set = { 0x02, 0x04, 0x0B, 0x16, 0x0C };
    RateSet element{ 10, rate_set };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element.Serialize(&raw_data);

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = RateSet::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(10, deserialized->GetRadioID());
    CHECK_EQUAL(5, deserialized->GetRateSetData().size());
    CHECK_EQUAL(0x02, deserialized->GetRateSetData()[0]);
    CHECK_EQUAL(0x04, deserialized->GetRateSetData()[1]);
    CHECK_EQUAL(0x0B, deserialized->GetRateSetData()[2]);
    CHECK_EQUAL(0x16, deserialized->GetRateSetData()[3]);
    CHECK_EQUAL(0x0C, deserialized->GetRateSetData()[4]);
}
