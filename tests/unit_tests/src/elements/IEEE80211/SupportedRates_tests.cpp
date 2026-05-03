#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/SupportedRates.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(SupportedRatesTestsGroup){ //
                                      TEST_SETUP(){}

                                      TEST_TEARDOWN(){}
};

TEST(SupportedRatesTestsGroup, Deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Message Element Header (4 bytes) ----
        0x04, 0x10,     // Type: 1040 (IEEE 802.11 Supported Rates)
        0x00, 0x05,     // Length: 5 bytes (1 RadioID + 4 rates)

        // ---- Value (5 bytes) ----
        0x01,           // Radio ID: 1
        0x82,           // Rate: 2 Mbps (basic)
        0x84,           // Rate: 4 Mbps (basic)
        0x0B,           // Rate: 5.5 Mbps
        0x16            // Rate: 11 Mbps
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = SupportedRates::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::SupportedRates, element->GetElementType());

    CHECK_EQUAL(1, element->GetRadioID());
    CHECK_EQUAL(4, element->GetRatesCount());
    CHECK_EQUAL(0x82, element->rates[0]);
    CHECK_EQUAL(0x84, element->rates[1]);
    CHECK_EQUAL(0x0B, element->rates[2]);
    CHECK_EQUAL(0x16, element->rates[3]);
}

TEST(SupportedRatesTestsGroup, Serialize) {
    uint8_t buffer[256] = {};
    WritableSupportedRatesArray w_rates;

    std::vector<uint8_t> rates = { 0x82, 0x84, 0x8B, 0x96 };
    w_rates.Add({ 5, rates });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_rates.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 9, raw_data.current);

    // clang-format off
    const uint8_t reference[] = {
        0x04, 0x10,     // Type: 1040
        0x00, 0x05,     // Length: 5 (RadioID + 4 rates)
        0x05,           // Radio ID: 5
        0x82, 0x84, 0x8B, 0x96  // Rates
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = SupportedRates::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(&buffer[0] + 9, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::SupportedRates, deserialized->GetElementType());
    CHECK_EQUAL(5, deserialized->GetRadioID());
    CHECK_EQUAL(4, deserialized->GetRatesCount());
}

TEST(SupportedRatesTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableSupportedRatesArray w_rates;

    std::vector<uint8_t> rates_0 = { 0x82, 0x84 };
    std::vector<uint8_t> rates_1 = { 0x0C, 0x12, 0x18, 0x24 };
    std::vector<uint8_t> rates_2 = { 0x30, 0x48, 0x60, 0x6C, 0x8C, 0x98 };

    w_rates.Add({ 1, rates_0 });
    w_rates.Add({ 1, rates_1 });
    w_rates.Add({ 2, rates_2 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_rates.Serialize(&raw_data);
    // Element 0: 4 + 1 + 2 = 7 bytes
    // Element 1: 4 + 1 + 4 = 9 bytes
    // Element 2: 4 + 1 + 6 = 11 bytes
    // Total: 27 bytes
    CHECK_EQUAL(&buffer[0] + 27, raw_data.current);

    ReadableSupportedRatesArray r_rates;
    CHECK_FALSE(r_rates.IsPresent());

    raw_data = { buffer, buffer + 27 };

    CHECK_TRUE(r_rates.Deserialize(&raw_data));
    CHECK_TRUE(r_rates.IsPresent());
    CHECK_TRUE(r_rates.Deserialize(&raw_data));
    CHECK_TRUE(r_rates.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_rates.Get().size());

    CHECK_EQUAL(1, r_rates.Get()[0]->GetRadioID());
    CHECK_EQUAL(2, r_rates.Get()[0]->GetRatesCount());

    CHECK_EQUAL(1, r_rates.Get()[1]->GetRadioID());
    CHECK_EQUAL(4, r_rates.Get()[1]->GetRatesCount());

    CHECK_EQUAL(2, r_rates.Get()[2]->GetRadioID());
    CHECK_EQUAL(6, r_rates.Get()[2]->GetRatesCount());
}

TEST(SupportedRatesTestsGroup, Validate_RadioID_range) {
    // clang-format off
    // Valid RadioID = 0
    uint8_t data_valid_zero[] = {
        0x04, 0x10, 0x00, 0x03, 0x00, 0x82, 0x84
    };
    // Valid RadioID = 31
    uint8_t data_valid_max[] = {
        0x04, 0x10, 0x00, 0x03, 0x1F, 0x82, 0x84
    };
    // Invalid RadioID = 32
    uint8_t data_invalid_high[] = {
        0x04, 0x10, 0x00, 0x03, 0x20, 0x82, 0x84
    };
    // clang-format on

    RawData raw_data = { data_valid_zero, data_valid_zero + sizeof(data_valid_zero) };
    CHECK(SupportedRates::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_max, data_valid_max + sizeof(data_valid_max) };
    CHECK(SupportedRates::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid_high, data_invalid_high + sizeof(data_invalid_high) };
    CHECK(SupportedRates::Deserialize(&raw_data) == nullptr);
}

TEST(SupportedRatesTestsGroup, Validate_length_range) {
    // clang-format off
    // Invalid: only 1 rate (minimum is 2)
    uint8_t data_too_short[] = {
        0x04, 0x10, 0x00, 0x02, 0x01, 0x82
    };
    // Valid: 2 rates (minimum)
    uint8_t data_min_rates[] = {
        0x04, 0x10, 0x00, 0x03, 0x01, 0x82, 0x84
    };
    // Valid: 8 rates (maximum)
    uint8_t data_max_rates[] = {
        0x04, 0x10, 0x00, 0x09, 0x01, 0x82, 0x84, 0x8B, 0x96, 0x0C, 0x12, 0x18, 0x24
    };
    // Invalid: 9 rates (exceeds maximum)
    uint8_t data_too_long[] = {
        0x04, 0x10, 0x00, 0x0A, 0x01, 0x82, 0x84, 0x8B, 0x96, 0x0C, 0x12, 0x18, 0x24, 0x30
    };
    // clang-format on

    RawData raw_data = { data_too_short, data_too_short + sizeof(data_too_short) };
    CHECK(SupportedRates::Deserialize(&raw_data) == nullptr);

    raw_data = { data_min_rates, data_min_rates + sizeof(data_min_rates) };
    CHECK(SupportedRates::Deserialize(&raw_data) != nullptr);

    raw_data = { data_max_rates, data_max_rates + sizeof(data_max_rates) };
    CHECK(SupportedRates::Deserialize(&raw_data) != nullptr);

    raw_data = { data_too_long, data_too_long + sizeof(data_too_long) };
    CHECK(SupportedRates::Deserialize(&raw_data) == nullptr);
}

TEST(SupportedRatesTestsGroup, Deserialize_wrong_type) {
    // clang-format off
    uint8_t data[] = {
        0x04, 0x11,     // Wrong Type: 1041 (not SupportedRates)
        0x00, 0x03,
        0x01, 0x82, 0x84
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    CHECK(SupportedRates::Deserialize(&raw_data) == nullptr);
}

TEST(SupportedRatesTestsGroup, Deserialize_insufficient_data) {
    // clang-format off
    uint8_t data[] = {
        0x04, 0x10, 0x00, 0x05, 0x01, 0x82, 0x84  // Length says 5 but only 3 bytes of value
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    CHECK(SupportedRates::Deserialize(&raw_data) == nullptr);
}
