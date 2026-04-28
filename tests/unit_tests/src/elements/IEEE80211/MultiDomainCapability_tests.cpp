#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/MultiDomainCapability.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(MultiDomainCapabilityTestsGroup){ //
                                             TEST_SETUP(){}

                                             TEST_TEARDOWN(){}
};

TEST(MultiDomainCapabilityTestsGroup, Deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Message Element Header (4 bytes) ----
        0x04, 0x08,     // Type: 1032 (IEEE 802.11 Multi-Domain Capability)
        0x00, 0x08,     // Length: 8 bytes

        // ---- Value (8 bytes) ----
        0x03,           // Radio ID: 3
        0x00,           // Reserved: 0
        0x00, 0x24,     // First Channel #: 36
        0x00, 0x08,     // Number of Channels: 8
        0x00, 0x14      // Max Tx Power Level: 20 dBm
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = MultiDomainCapability::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::MultiDomainCapability, element->GetElementType());

    CHECK_EQUAL(3, element->GetRadioID());
    CHECK_EQUAL(36, element->GetFirstChannel());
    CHECK_EQUAL(8, element->GetNumberOfChannels());
    CHECK_EQUAL(20, element->GetMaxTxPowerLevel());
}

TEST(MultiDomainCapabilityTestsGroup, Serialize) {
    uint8_t buffer[256] = {};

    MultiDomainCapability element{ 5, 1, 11, 30 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 12, raw_data.current);

    // clang-format off
    const uint8_t reference[] = {
        0x04, 0x08,     // Type: 1032
        0x00, 0x08,     // Length: 8

        0x05,           // Radio ID: 5
        0x00,           // Reserved: 0
        0x00, 0x01,     // First Channel #: 1
        0x00, 0x0B,     // Number of Channels: 11
        0x00, 0x1E      // Max Tx Power Level: 30
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = MultiDomainCapability::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(&buffer[0] + 12, raw_data.current);
    CHECK_EQUAL(5, deserialized->GetRadioID());
    CHECK_EQUAL(1, deserialized->GetFirstChannel());
    CHECK_EQUAL(11, deserialized->GetNumberOfChannels());
    CHECK_EQUAL(30, deserialized->GetMaxTxPowerLevel());
}

TEST(MultiDomainCapabilityTestsGroup, Serialize_Deserialize_array) {
    uint8_t buffer[2048] = {};

    WritableMultiDomainCapabilityArray w_capabilities;
    w_capabilities.Add({ 1, 1, 13, 20 });
    w_capabilities.Add({ 2, 36, 4, 17 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_capabilities.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 24, raw_data.current); // 2 × 12 = 24

    ReadableMultiDomainCapabilityArray r_capabilities;
    raw_data = { buffer, buffer + 24 };

    CHECK_TRUE(r_capabilities.Deserialize(&raw_data));
    CHECK_TRUE(r_capabilities.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_capabilities.Get().size());

    CHECK_EQUAL(1, r_capabilities.Get()[0]->GetRadioID());
    CHECK_EQUAL(1, r_capabilities.Get()[0]->GetFirstChannel());
    CHECK_EQUAL(13, r_capabilities.Get()[0]->GetNumberOfChannels());
    CHECK_EQUAL(20, r_capabilities.Get()[0]->GetMaxTxPowerLevel());

    CHECK_EQUAL(2, r_capabilities.Get()[1]->GetRadioID());
    CHECK_EQUAL(36, r_capabilities.Get()[1]->GetFirstChannel());
    CHECK_EQUAL(4, r_capabilities.Get()[1]->GetNumberOfChannels());
    CHECK_EQUAL(17, r_capabilities.Get()[1]->GetMaxTxPowerLevel());
}

TEST(MultiDomainCapabilityTestsGroup, Validate_RadioID_range) {
    uint8_t buffer[256] = {};

    // Valid: RadioID = 0 (minimum)
    MultiDomainCapability elem_0{ 0, 36, 4, 20 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    elem_0.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    CHECK(MultiDomainCapability::Deserialize(&raw_data) != nullptr);

    // Valid: RadioID = 1
    MultiDomainCapability elem_1{ 1, 36, 4, 20 };
    raw_data = { buffer, buffer + sizeof(buffer) };
    elem_1.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    CHECK(MultiDomainCapability::Deserialize(&raw_data) != nullptr);

    // Valid: RadioID = 31 (maximum)
    MultiDomainCapability elem_31{ 31, 36, 4, 20 };
    raw_data = { buffer, buffer + sizeof(buffer) };
    elem_31.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    CHECK(MultiDomainCapability::Deserialize(&raw_data) != nullptr);

    // Invalid: RadioID = 32 (above maximum)
    MultiDomainCapability elem_32{ 32, 36, 4, 20 };
    raw_data = { buffer, buffer + sizeof(buffer) };
    elem_32.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    CHECK(MultiDomainCapability::Deserialize(&raw_data) == nullptr);
}

TEST(MultiDomainCapabilityTestsGroup, Regulatory_constraints_2_4GHz) {
    uint8_t buffer[256] = {};

    // Example: 2.4 GHz band (channels 1-13, max 20 dBm)
    MultiDomainCapability element{ 1, 1, 13, 20 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element.Serialize(&raw_data);

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = MultiDomainCapability::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(1, deserialized->GetRadioID());
    CHECK_EQUAL(1, deserialized->GetFirstChannel());
    CHECK_EQUAL(13, deserialized->GetNumberOfChannels());
    CHECK_EQUAL(20, deserialized->GetMaxTxPowerLevel());
}

TEST(MultiDomainCapabilityTestsGroup, Regulatory_constraints_5GHz) {
    uint8_t buffer[256] = {};

    // Example: 5 GHz band (channels 36-48, max 17 dBm)
    MultiDomainCapability element{ 2, 36, 4, 17 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element.Serialize(&raw_data);

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = MultiDomainCapability::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(2, deserialized->GetRadioID());
    CHECK_EQUAL(36, deserialized->GetFirstChannel());
    CHECK_EQUAL(4, deserialized->GetNumberOfChannels());
    CHECK_EQUAL(17, deserialized->GetMaxTxPowerLevel());
}

TEST(MultiDomainCapabilityTestsGroup, Max_values) {
    uint8_t buffer[256] = {};

    // Test with maximum values
    MultiDomainCapability element{ 31, 0xFFFF, 0xFFFF, 0xFFFF };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element.Serialize(&raw_data);

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = MultiDomainCapability::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(31, deserialized->GetRadioID());
    CHECK_EQUAL(0xFFFF, deserialized->GetFirstChannel());
    CHECK_EQUAL(0xFFFF, deserialized->GetNumberOfChannels());
    CHECK_EQUAL(0xFFFF, deserialized->GetMaxTxPowerLevel());
}
