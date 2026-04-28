#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/InformationElement.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(InformationElementTestsGroup){ //
                                          TEST_SETUP(){}

                                          TEST_TEARDOWN(){}
};

TEST(InformationElementTestsGroup, Deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Message Element Header (4 bytes) ----
        0x04, 0x05,     // Type: 1029 (IEEE 802.11 Information Element)
        0x00, 0x07,     // Length: 7 bytes (3 header + 4 IE data)

        // ---- Value ----
        0x01,           // Radio ID: 1
        0x02,           // WLAN ID: 2
        0xC0,           // Flags: B=1, P=1 (0xC0)

        // IEEE 802.11 IE: SSID element example
        0x00,           // Element ID: 0 (SSID)
        0x02,           // Length: 2
        0x41, 0x42      // Value: "AB"
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = InformationElement::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::InformationElement, element->GetElementType());

    CHECK_EQUAL(1, element->GetRadioID());
    CHECK_EQUAL(2, element->GetWlanID());
    CHECK_EQUAL(0xC0, element->GetFlags());
    CHECK_TRUE(element->GetBeaconFlag());
    CHECK_TRUE(element->GetProbeResponseFlag());
    CHECK_EQUAL(4, element->GetIELength());

    // Check IE data
    CHECK_EQUAL(0x00, element->ie_data[0]); // Element ID
    CHECK_EQUAL(0x02, element->ie_data[1]); // Length
    CHECK_EQUAL(0x41, element->ie_data[2]); // 'A'
    CHECK_EQUAL(0x42, element->ie_data[3]); // 'B'
}

TEST(InformationElementTestsGroup, Serialize) {
    uint8_t buffer[256] = {};
    std::vector<uint8_t> ie_data = { 0x00, 0x04, 0x54, 0x45, 0x53, 0x54 }; // SSID: "TEST"
    WritableInformationElementArray::Item item{ 5, 3, 0x80, std::move(ie_data) };

    WritableInformationElementArray w_ies;
    w_ies.Add(std::move(item));

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_ies.Serialize(&raw_data);

    CHECK_EQUAL(&buffer[0] + 13, raw_data.current);

    // clang-format off
    const uint8_t reference[] = {
        0x04, 0x05,     // Type: 1029
        0x00, 0x09,     // Length: 9 (3 + 6)
        0x05,           // Radio ID: 5
        0x03,           // WLAN ID: 3
        0x80,           // Flags: B=1, P=0
        0x00, 0x04,     // IE: SSID, length 4
        0x54, 0x45, 0x53, 0x54  // "TEST"
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = InformationElement::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(&buffer[0] + 13, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::InformationElement, element->GetElementType());
    CHECK_EQUAL(5, element->GetRadioID());
    CHECK_EQUAL(3, element->GetWlanID());
    CHECK_EQUAL(0x80, element->GetFlags());
    CHECK_TRUE(element->GetBeaconFlag());
    CHECK_FALSE(element->GetProbeResponseFlag());
    CHECK_EQUAL(6, element->GetIELength());
}

TEST(InformationElementTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableInformationElementArray w_ies;

    std::vector<uint8_t> ie1 = { 0x00, 0x02, 0x41, 0x42 };         // SSID: "AB"
    std::vector<uint8_t> ie2 = { 0x01, 0x04, 0x82, 0x84, 0x8B, 0x96 }; // Supported Rates
    std::vector<uint8_t> ie3 = { 0x03, 0x01, 0x06 };               // DS Parameter Set: Channel 6

    w_ies.Add({ 1, 1, 0xC0, std::move(ie1) }); // B=1, P=1
    w_ies.Add({ 1, 2, 0x80, std::move(ie2) }); // B=1, P=0
    w_ies.Add({ 2, 1, 0x40, std::move(ie3) }); // B=0, P=1

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_ies.Serialize(&raw_data);
    // Element sizes: (7+4)=11, (7+6)=13, (7+3)=10 = 34 total
    CHECK_EQUAL(&buffer[0] + 34, raw_data.current);

    ReadableInformationElementArray r_ies;

    raw_data = { buffer, buffer + 34 };

    CHECK_TRUE(r_ies.Deserialize(&raw_data));
    CHECK_TRUE(r_ies.Deserialize(&raw_data));
    CHECK_TRUE(r_ies.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_ies.Get().size());

    CHECK_EQUAL(1, r_ies.Get()[0]->GetRadioID());
    CHECK_EQUAL(1, r_ies.Get()[0]->GetWlanID());
    CHECK_EQUAL(0xC0, r_ies.Get()[0]->GetFlags());
    CHECK_EQUAL(4, r_ies.Get()[0]->GetIELength());

    CHECK_EQUAL(1, r_ies.Get()[1]->GetRadioID());
    CHECK_EQUAL(2, r_ies.Get()[1]->GetWlanID());
    CHECK_EQUAL(0x80, r_ies.Get()[1]->GetFlags());
    CHECK_EQUAL(6, r_ies.Get()[1]->GetIELength());

    CHECK_EQUAL(2, r_ies.Get()[2]->GetRadioID());
    CHECK_EQUAL(1, r_ies.Get()[2]->GetWlanID());
    CHECK_EQUAL(0x40, r_ies.Get()[2]->GetFlags());
    CHECK_EQUAL(3, r_ies.Get()[2]->GetIELength());
}

TEST(InformationElementTestsGroup, Validate_RadioID_range) {
    // clang-format off
    // Valid RadioID = 0
    uint8_t data_valid_zero[] = {
        0x04, 0x05, 0x00, 0x05, 0x00, 0x01, 0x80, 0x00, 0x00
    };
    // Valid RadioID = 1
    uint8_t data_valid_min[] = {
        0x04, 0x05, 0x00, 0x05, 0x01, 0x01, 0x80, 0x00, 0x00
    };
    // Valid RadioID = 31
    uint8_t data_valid_max[] = {
        0x04, 0x05, 0x00, 0x05, 0x1F, 0x01, 0x80, 0x00, 0x00
    };
    // Invalid RadioID = 32
    uint8_t data_invalid_high[] = {
        0x04, 0x05, 0x00, 0x05, 0x20, 0x01, 0x80, 0x00, 0x00
    };
    // clang-format on

    RawData raw_data = { data_valid_zero, data_valid_zero + sizeof(data_valid_zero) };
    CHECK(InformationElement::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_min, data_valid_min + sizeof(data_valid_min) };
    CHECK(InformationElement::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_max, data_valid_max + sizeof(data_valid_max) };
    CHECK(InformationElement::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid_high, data_invalid_high + sizeof(data_invalid_high) };
    CHECK(InformationElement::Deserialize(&raw_data) == nullptr);
}

TEST(InformationElementTestsGroup, Validate_WlanID_range) {
    // clang-format off
    // Valid WlanID = 1
    uint8_t data_valid_min[] = {
        0x04, 0x05, 0x00, 0x05, 0x01, 0x01, 0x80, 0x00, 0x00
    };
    // Valid WlanID = 16
    uint8_t data_valid_max[] = {
        0x04, 0x05, 0x00, 0x05, 0x01, 0x10, 0x80, 0x00, 0x00
    };
    // Invalid WlanID = 0
    uint8_t data_invalid_zero[] = {
        0x04, 0x05, 0x00, 0x05, 0x01, 0x00, 0x80, 0x00, 0x00
    };
    // Invalid WlanID = 17
    uint8_t data_invalid_high[] = {
        0x04, 0x05, 0x00, 0x05, 0x01, 0x11, 0x80, 0x00, 0x00
    };
    // clang-format on

    RawData raw_data = { data_valid_min, data_valid_min + sizeof(data_valid_min) };
    CHECK(InformationElement::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_max, data_valid_max + sizeof(data_valid_max) };
    CHECK(InformationElement::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid_zero, data_invalid_zero + sizeof(data_invalid_zero) };
    CHECK(InformationElement::Deserialize(&raw_data) == nullptr);

    raw_data = { data_invalid_high, data_invalid_high + sizeof(data_invalid_high) };
    CHECK(InformationElement::Deserialize(&raw_data) == nullptr);
}

TEST(InformationElementTestsGroup, Validate_Flags_reserved_bits) {
    // clang-format off
    // Valid: B=1, P=1, reserved=0 (0xC0)
    uint8_t data_valid_both[] = {
        0x04, 0x05, 0x00, 0x05, 0x01, 0x01, 0xC0, 0x00, 0x00
    };
    // Valid: B=1, P=0, reserved=0 (0x80)
    uint8_t data_valid_beacon[] = {
        0x04, 0x05, 0x00, 0x05, 0x01, 0x01, 0x80, 0x00, 0x00
    };
    // Valid: B=0, P=1, reserved=0 (0x40)
    uint8_t data_valid_probe[] = {
        0x04, 0x05, 0x00, 0x05, 0x01, 0x01, 0x40, 0x00, 0x00
    };
    // Valid: B=0, P=0, reserved=0 (0x00)
    uint8_t data_valid_none[] = {
        0x04, 0x05, 0x00, 0x05, 0x01, 0x01, 0x00, 0x00, 0x00
    };
    // Invalid: reserved bit set (0xC1)
    uint8_t data_invalid_reserved[] = {
        0x04, 0x05, 0x00, 0x05, 0x01, 0x01, 0xC1, 0x00, 0x00
    };
    // Invalid: multiple reserved bits set (0xFF)
    uint8_t data_invalid_all_bits[] = {
        0x04, 0x05, 0x00, 0x05, 0x01, 0x01, 0xFF, 0x00, 0x00
    };
    // clang-format on

    RawData raw_data = { data_valid_both, data_valid_both + sizeof(data_valid_both) };
    CHECK(InformationElement::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_beacon, data_valid_beacon + sizeof(data_valid_beacon) };
    CHECK(InformationElement::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_probe, data_valid_probe + sizeof(data_valid_probe) };
    CHECK(InformationElement::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_none, data_valid_none + sizeof(data_valid_none) };
    CHECK(InformationElement::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid_reserved, data_invalid_reserved + sizeof(data_invalid_reserved) };
    CHECK(InformationElement::Deserialize(&raw_data) == nullptr);

    raw_data = { data_invalid_all_bits, data_invalid_all_bits + sizeof(data_invalid_all_bits) };
    CHECK(InformationElement::Deserialize(&raw_data) == nullptr);
}

TEST(InformationElementTestsGroup, Empty_IE_data) {
    // clang-format off
    // Valid: minimal element with no IE data
    uint8_t data[] = {
        0x04, 0x05,     // Type: 1029
        0x00, 0x03,     // Length: 3 (header only, no IE data)
        0x01,           // Radio ID: 1
        0x01,           // WLAN ID: 1
        0x80            // Flags: B=1
    };
    // clang-format on

    RawData raw_data = { data, data + sizeof(data) };
    auto element = InformationElement::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(0, element->GetIELength());
}
