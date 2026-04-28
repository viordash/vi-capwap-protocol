#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/OFDMControl.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(OFDMControlTestsGroup){ //
                                   TEST_SETUP(){}

                                   TEST_TEARDOWN(){}
};

TEST(OFDMControlTestsGroup, Deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Message Element Header (4 bytes) ----
        0x04, 0x09,     // Type: 1033 (IEEE 802.11 OFDM Control)
        0x00, 0x08,     // Length: 8 bytes

        // ---- Value (8 bytes) ----
        0x05,           // Radio ID: 5
        0x00,           // Reserved: 0
        0x24,           // Current Channel: 36 (5.180 GHz)
        0x03,           // Band Support: 0x03 (bits 0,1 - 5.15-5.35 GHz)
        0x00, 0x00, 0x00, 0xC8  // TI Threshold: 200
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = OFDMControl::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::OFDMControl, element->GetElementType());

    CHECK_EQUAL(5, element->GetRadioID());
    CHECK_EQUAL(36, element->GetCurrentChannel());
    CHECK_EQUAL(0x03, element->GetBandSupport());
    CHECK_EQUAL(200, element->GetTIThreshold());
}

TEST(OFDMControlTestsGroup, Serialize) {
    uint8_t buffer[256] = {};

    OFDMControl element{ 10, 40, 0x07, 500 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 12, raw_data.current);

    // clang-format off
    const uint8_t reference[] = {
        0x04, 0x09,     // Type: 1033
        0x00, 0x08,     // Length: 8

        0x0A,           // Radio ID: 10
        0x00,           // Reserved: 0
        0x28,           // Current Channel: 40
        0x07,           // Band Support: 0x07
        0x00, 0x00, 0x01, 0xF4  // TI Threshold: 500
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = OFDMControl::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(&buffer[0] + 12, raw_data.current);
    CHECK_EQUAL(10, deserialized->GetRadioID());
    CHECK_EQUAL(40, deserialized->GetCurrentChannel());
    CHECK_EQUAL(0x07, deserialized->GetBandSupport());
    CHECK_EQUAL(500, deserialized->GetTIThreshold());
}

TEST(OFDMControlTestsGroup, Serialize_Deserialize_array) {
    uint8_t buffer[2048] = {};

    WritableOFDMControlArray w_controls;
    w_controls.Add({ 1, 36, 0x01, 100 });
    w_controls.Add({ 2, 40, 0x02, 200 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_controls.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 24, raw_data.current); // 2 × 12 = 24

    ReadableOFDMControlArray r_controls;
    raw_data = { buffer, buffer + 24 };

    CHECK_TRUE(r_controls.Deserialize(&raw_data));
    CHECK_TRUE(r_controls.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_controls.Get().size());

    CHECK_EQUAL(1, r_controls.Get()[0]->GetRadioID());
    CHECK_EQUAL(36, r_controls.Get()[0]->GetCurrentChannel());
    CHECK_EQUAL(0x01, r_controls.Get()[0]->GetBandSupport());
    CHECK_EQUAL(100, r_controls.Get()[0]->GetTIThreshold());

    CHECK_EQUAL(2, r_controls.Get()[1]->GetRadioID());
    CHECK_EQUAL(40, r_controls.Get()[1]->GetCurrentChannel());
    CHECK_EQUAL(0x02, r_controls.Get()[1]->GetBandSupport());
    CHECK_EQUAL(200, r_controls.Get()[1]->GetTIThreshold());
}

TEST(OFDMControlTestsGroup, Validate_RadioID_range) {
    uint8_t buffer[256] = {};

    // Valid: RadioID = 0 (minimum)
    OFDMControl elem_0{ 0, 36, 0x01, 100 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    elem_0.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    CHECK(OFDMControl::Deserialize(&raw_data) != nullptr);

    // Valid: RadioID = 1
    OFDMControl elem_1{ 1, 36, 0x01, 100 };
    raw_data = { buffer, buffer + sizeof(buffer) };
    elem_1.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    CHECK(OFDMControl::Deserialize(&raw_data) != nullptr);

    // Valid: RadioID = 31 (maximum)
    OFDMControl elem_31{ 31, 36, 0x01, 100 };
    raw_data = { buffer, buffer + sizeof(buffer) };
    elem_31.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    CHECK(OFDMControl::Deserialize(&raw_data) != nullptr);

    // Invalid: RadioID = 32 (above maximum)
    OFDMControl elem_32{ 32, 36, 0x01, 100 };
    raw_data = { buffer, buffer + sizeof(buffer) };
    elem_32.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    CHECK(OFDMControl::Deserialize(&raw_data) == nullptr);
}

TEST(OFDMControlTestsGroup, Band_support_flags) {
    uint8_t buffer[256] = {};

    // Test all band support flags
    uint8_t band_all = OFDMControl::BAND_5_15_5_25_GHZ | OFDMControl::BAND_5_25_5_35_GHZ |
                       OFDMControl::BAND_5_725_5_825_GHZ | OFDMControl::BAND_5_47_5_725_GHZ |
                       OFDMControl::BAND_LOWER_JP_5_25_GHZ | OFDMControl::BAND_5_03_5_091_GHZ |
                       OFDMControl::BAND_4_94_4_99_GHZ;

    OFDMControl element{ 5, 36, band_all, 1000 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element.Serialize(&raw_data);

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = OFDMControl::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(0x7F, deserialized->GetBandSupport()); // All 7 bits set

    // Test individual flags
    CHECK_EQUAL(0x01, OFDMControl::BAND_5_15_5_25_GHZ);
    CHECK_EQUAL(0x02, OFDMControl::BAND_5_25_5_35_GHZ);
    CHECK_EQUAL(0x04, OFDMControl::BAND_5_725_5_825_GHZ);
    CHECK_EQUAL(0x08, OFDMControl::BAND_5_47_5_725_GHZ);
    CHECK_EQUAL(0x10, OFDMControl::BAND_LOWER_JP_5_25_GHZ);
    CHECK_EQUAL(0x20, OFDMControl::BAND_5_03_5_091_GHZ);
    CHECK_EQUAL(0x40, OFDMControl::BAND_4_94_4_99_GHZ);
}

TEST(OFDMControlTestsGroup, Channel_and_threshold_values) {
    uint8_t buffer[256] = {};

    // Test with various channel and threshold values
    OFDMControl element{ 15, 165, 0x3F, 0xFFFFFFFF };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element.Serialize(&raw_data);

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = OFDMControl::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(15, deserialized->GetRadioID());
    CHECK_EQUAL(165, deserialized->GetCurrentChannel());
    CHECK_EQUAL(0x3F, deserialized->GetBandSupport());
    CHECK_EQUAL(0xFFFFFFFF, deserialized->GetTIThreshold());
}
