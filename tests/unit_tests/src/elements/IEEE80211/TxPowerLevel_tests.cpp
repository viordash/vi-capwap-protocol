#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/TxPowerLevel.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(TxPowerLevelTestsGroup){ //
                                    TEST_SETUP(){}

                                    TEST_TEARDOWN(){}
};

TEST(TxPowerLevelTestsGroup, Deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Message Element Header (4 bytes) ----
        0x04, 0x12,     // Type: 1042 (IEEE 802.11 Tx Power Level)
        0x00, 0x08,     // Length: 8 bytes (2 + 3 * 2)

        // ---- Value (8 bytes) ----
        0x01,           // Radio ID: 1
        0x03,           // Num Levels: 3
        0x00, 0x14,     // Power Level 0: 20 dBm
        0x00, 0x11,     // Power Level 1: 17 dBm
        0x00, 0x0E      // Power Level 2: 14 dBm
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = TxPowerLevel::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::TxPowerLevel, element->GetElementType());

    CHECK_EQUAL(1, element->GetRadioID());
    CHECK_EQUAL(3, element->GetNumLevels());
    CHECK_EQUAL(20, element->power_levels[0].Get());
    CHECK_EQUAL(17, element->power_levels[1].Get());
    CHECK_EQUAL(14, element->power_levels[2].Get());
}

TEST(TxPowerLevelTestsGroup, Deserialize_negative_power) {
    // clang-format off
    uint8_t data[] = {
        0x04, 0x12,     // Type: 1042
        0x00, 0x06,     // Length: 6 bytes (2 + 2 * 2)

        0x01,           // Radio ID: 1
        0x02,           // Num Levels: 2
        0xFF, 0xF6,     // Power Level 0: -10 dBm (signed)
        0xFF, 0xEC      // Power Level 1: -20 dBm (signed)
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = TxPowerLevel::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(-10, element->power_levels[0].Get());
    CHECK_EQUAL(-20, element->power_levels[1].Get());
}

TEST(TxPowerLevelTestsGroup, Serialize) {
    uint8_t buffer[256] = {};
    WritableTxPowerLevelArray w_levels;

    std::vector<int16_t> levels = { 20, 17, 14, 11 };
    w_levels.Add({ 5, levels });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_levels.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 14, raw_data.current); // 4 header + 2 + 4*2 = 14

    // clang-format off
    const uint8_t reference[] = {
        0x04, 0x12,     // Type: 1042
        0x00, 0x0A,     // Length: 10 (2 + 4*2)
        0x05,           // Radio ID: 5
        0x04,           // Num Levels: 4
        0x00, 0x14,     // 20 dBm
        0x00, 0x11,     // 17 dBm
        0x00, 0x0E,     // 14 dBm
        0x00, 0x0B      // 11 dBm
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = TxPowerLevel::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(&buffer[0] + 14, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::TxPowerLevel, deserialized->GetElementType());
    CHECK_EQUAL(5, deserialized->GetRadioID());
    CHECK_EQUAL(4, deserialized->GetNumLevels());
}

TEST(TxPowerLevelTestsGroup, Serialize_negative_power) {
    uint8_t buffer[256] = {};
    WritableTxPowerLevelArray w_levels;

    std::vector<int16_t> levels = { -5, -10 };
    w_levels.Add({ 1, levels });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_levels.Serialize(&raw_data);

    // clang-format off
    const uint8_t reference[] = {
        0x04, 0x12,     // Type: 1042
        0x00, 0x06,     // Length: 6 (2 + 2*2)
        0x01,           // Radio ID: 1
        0x02,           // Num Levels: 2
        0xFF, 0xFB,     // -5 dBm
        0xFF, 0xF6      // -10 dBm
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));
}

TEST(TxPowerLevelTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableTxPowerLevelArray w_levels;

    std::vector<int16_t> levels_0 = { 20 };
    std::vector<int16_t> levels_1 = { 17, 14, 11 };
    std::vector<int16_t> levels_2 = { 23, 20, 17, 14, 11 };

    w_levels.Add({ 1, levels_0 });
    w_levels.Add({ 1, levels_1 });
    w_levels.Add({ 2, levels_2 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_levels.Serialize(&raw_data);
    // Element 0: 4 + 2 + 1*2 = 8 bytes
    // Element 1: 4 + 2 + 3*2 = 12 bytes
    // Element 2: 4 + 2 + 5*2 = 16 bytes
    // Total: 36 bytes
    CHECK_EQUAL(&buffer[0] + 36, raw_data.current);

    ReadableTxPowerLevelArray r_levels;

    raw_data = { buffer, buffer + 36 };

    CHECK_TRUE(r_levels.Deserialize(&raw_data));
    CHECK_TRUE(r_levels.Deserialize(&raw_data));
    CHECK_TRUE(r_levels.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_levels.Get().size());

    CHECK_EQUAL(1, r_levels.Get()[0]->GetRadioID());
    CHECK_EQUAL(1, r_levels.Get()[0]->GetNumLevels());

    CHECK_EQUAL(1, r_levels.Get()[1]->GetRadioID());
    CHECK_EQUAL(3, r_levels.Get()[1]->GetNumLevels());

    CHECK_EQUAL(2, r_levels.Get()[2]->GetRadioID());
    CHECK_EQUAL(5, r_levels.Get()[2]->GetNumLevels());
}

TEST(TxPowerLevelTestsGroup, Validate_RadioID_range) {
    // clang-format off
    // Valid RadioID = 0
    uint8_t data_valid_zero[] = {
        0x04, 0x12, 0x00, 0x04, 0x00, 0x01, 0x00, 0x14
    };
    // Valid RadioID = 31
    uint8_t data_valid_max[] = {
        0x04, 0x12, 0x00, 0x04, 0x1F, 0x01, 0x00, 0x14
    };
    // Invalid RadioID = 32
    uint8_t data_invalid_high[] = {
        0x04, 0x12, 0x00, 0x04, 0x20, 0x01, 0x00, 0x14
    };
    // clang-format on

    RawData raw_data = { data_valid_zero, data_valid_zero + sizeof(data_valid_zero) };
    CHECK(TxPowerLevel::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_max, data_valid_max + sizeof(data_valid_max) };
    CHECK(TxPowerLevel::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid_high, data_invalid_high + sizeof(data_invalid_high) };
    CHECK(TxPowerLevel::Deserialize(&raw_data) == nullptr);
}

TEST(TxPowerLevelTestsGroup, Validate_NumLevels_range) {
    // clang-format off
    // Invalid: NumLevels = 0
    uint8_t data_zero_levels[] = {
        0x04, 0x12, 0x00, 0x02, 0x01, 0x00
    };
    // Valid: NumLevels = 1
    uint8_t data_one_level[] = {
        0x04, 0x12, 0x00, 0x04, 0x01, 0x01, 0x00, 0x14
    };
    // Valid: NumLevels = 8 (max)
    uint8_t data_max_levels[] = {
        0x04, 0x12, 0x00, 0x12, 0x01, 0x08,
        0x00, 0x17, 0x00, 0x14, 0x00, 0x11, 0x00, 0x0E,
        0x00, 0x0B, 0x00, 0x08, 0x00, 0x05, 0x00, 0x02
    };
    // Invalid: NumLevels = 9 (exceeds max)
    uint8_t data_too_many_levels[] = {
        0x04, 0x12, 0x00, 0x14, 0x01, 0x09,
        0x00, 0x17, 0x00, 0x14, 0x00, 0x11, 0x00, 0x0E,
        0x00, 0x0B, 0x00, 0x08, 0x00, 0x05, 0x00, 0x02, 0x00, 0x00
    };
    // clang-format on

    RawData raw_data = { data_zero_levels, data_zero_levels + sizeof(data_zero_levels) };
    CHECK(TxPowerLevel::Deserialize(&raw_data) == nullptr);

    raw_data = { data_one_level, data_one_level + sizeof(data_one_level) };
    CHECK(TxPowerLevel::Deserialize(&raw_data) != nullptr);

    raw_data = { data_max_levels, data_max_levels + sizeof(data_max_levels) };
    CHECK(TxPowerLevel::Deserialize(&raw_data) != nullptr);

    raw_data = { data_too_many_levels, data_too_many_levels + sizeof(data_too_many_levels) };
    CHECK(TxPowerLevel::Deserialize(&raw_data) == nullptr);
}

TEST(TxPowerLevelTestsGroup, Validate_length_mismatch) {
    // clang-format off
    // Length doesn't match num_levels
    uint8_t data_length_mismatch[] = {
        0x04, 0x12, 0x00, 0x06, 0x01, 0x02, 0x00, 0x14  // Says 2 levels but only 1 provided
    };
    // clang-format on

    RawData raw_data = { data_length_mismatch,
                         data_length_mismatch + sizeof(data_length_mismatch) };
    CHECK(TxPowerLevel::Deserialize(&raw_data) == nullptr);
}

TEST(TxPowerLevelTestsGroup, Deserialize_insufficient_data) {
    // clang-format off
    uint8_t data[] = {
        0x04, 0x12, 0x00, 0x06, 0x01, 0x02, 0x00  // Missing bytes
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    CHECK(TxPowerLevel::Deserialize(&raw_data) == nullptr);
}
