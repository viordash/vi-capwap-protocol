#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/WTPRadioFailAlarmIndication.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(WTPRadioFailAlarmIndicationTestsGroup){ //
                                                   TEST_SETUP(){}

                                                   TEST_TEARDOWN(){}
};

TEST(WTPRadioFailAlarmIndicationTestsGroup, Deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Message Element Header (4 bytes) ----
        0x04, 0x17,     // Type: 1047 (IEEE 802.11 WTP Radio Fail Alarm Indication)
        0x00, 0x04,     // Length: 4 bytes

        // ---- Value (4 bytes) ----
        0x01,           // Radio ID: 1
        0x01,           // Type: Receiver (1)
        0x03,           // Status: Major (3)
        0x00            // Pad: 0
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = WTPRadioFailAlarmIndication::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::WTPRadioFailAlarmIndication, element->GetElementType());

    CHECK_EQUAL(1, element->RadioID);
    CHECK_EQUAL(WTPRadioFailAlarmIndication::Receiver, element->Type);
    CHECK_EQUAL(WTPRadioFailAlarmIndication::Major, element->Status);
    CHECK_EQUAL(0, element->Pad);
}

TEST(WTPRadioFailAlarmIndicationTestsGroup, Serialize) {
    uint8_t buffer[256] = {};
    WTPRadioFailAlarmIndication element{ 5,
                                         WTPRadioFailAlarmIndication::Transmitter,
                                         WTPRadioFailAlarmIndication::Critical };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 8, raw_data.current);

    // clang-format off
    const uint8_t reference[] = {
        0x04, 0x17,     // Type: 1047
        0x00, 0x04,     // Length: 4
        0x05,           // Radio ID: 5
        0x02,           // Type: Transmitter (2)
        0x04,           // Status: Critical (4)
        0x00            // Pad: 0
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = WTPRadioFailAlarmIndication::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(&buffer[0] + 8, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::WTPRadioFailAlarmIndication,
                deserialized->GetElementType());
    CHECK_EQUAL(5, deserialized->RadioID);
    CHECK_EQUAL(WTPRadioFailAlarmIndication::Transmitter, deserialized->Type);
    CHECK_EQUAL(WTPRadioFailAlarmIndication::Critical, deserialized->Status);
}

TEST(WTPRadioFailAlarmIndicationTestsGroup, Validate_RadioID_range) {
    // clang-format off
    // Valid RadioID = 0
    uint8_t data_valid_zero[] = {
        0x04, 0x17, 0x00, 0x04, 0x00, 0x01, 0x01, 0x00
    };
    // Valid RadioID = 31
    uint8_t data_valid_max[] = {
        0x04, 0x17, 0x00, 0x04, 0x1F, 0x01, 0x01, 0x00
    };
    // Invalid RadioID = 32
    uint8_t data_invalid_high[] = {
        0x04, 0x17, 0x00, 0x04, 0x20, 0x01, 0x01, 0x00
    };
    // clang-format on

    RawData raw_data = { data_valid_zero, data_valid_zero + sizeof(data_valid_zero) };
    CHECK(WTPRadioFailAlarmIndication::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_max, data_valid_max + sizeof(data_valid_max) };
    CHECK(WTPRadioFailAlarmIndication::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid_high, data_invalid_high + sizeof(data_invalid_high) };
    CHECK(WTPRadioFailAlarmIndication::Deserialize(&raw_data) == nullptr);
}

TEST(WTPRadioFailAlarmIndicationTestsGroup, Validate_Type_range) {
    // clang-format off
    // Valid Type = 0 (Reserved)
    uint8_t data_reserved[] = {
        0x04, 0x17, 0x00, 0x04, 0x01, 0x00, 0x01, 0x00
    };
    // Valid Type = 1 (Receiver)
    uint8_t data_receiver[] = {
        0x04, 0x17, 0x00, 0x04, 0x01, 0x01, 0x01, 0x00
    };
    // Valid Type = 2 (Transmitter)
    uint8_t data_transmitter[] = {
        0x04, 0x17, 0x00, 0x04, 0x01, 0x02, 0x01, 0x00
    };
    // Invalid Type = 3
    uint8_t data_invalid[] = {
        0x04, 0x17, 0x00, 0x04, 0x01, 0x03, 0x01, 0x00
    };
    // clang-format on

    RawData raw_data = { data_reserved, data_reserved + sizeof(data_reserved) };
    CHECK(WTPRadioFailAlarmIndication::Deserialize(&raw_data) == nullptr);

    raw_data = { data_receiver, data_receiver + sizeof(data_receiver) };
    CHECK(WTPRadioFailAlarmIndication::Deserialize(&raw_data) != nullptr);

    raw_data = { data_transmitter, data_transmitter + sizeof(data_transmitter) };
    CHECK(WTPRadioFailAlarmIndication::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid, data_invalid + sizeof(data_invalid) };
    CHECK(WTPRadioFailAlarmIndication::Deserialize(&raw_data) == nullptr);
}

TEST(WTPRadioFailAlarmIndicationTestsGroup, Validate_Status_range) {
    // clang-format off
    // Valid Status = 0 (Reserved)
    uint8_t data_reserved[] = {
        0x04, 0x17, 0x00, 0x04, 0x01, 0x01, 0x00, 0x00
    };
    // Valid Status = 1 (Cleared)
    uint8_t data_cleared[] = {
        0x04, 0x17, 0x00, 0x04, 0x01, 0x01, 0x01, 0x00
    };
    // Valid Status = 4 (Critical)
    uint8_t data_critical[] = {
        0x04, 0x17, 0x00, 0x04, 0x01, 0x01, 0x04, 0x00
    };
    // Invalid Status = 5
    uint8_t data_invalid[] = {
        0x04, 0x17, 0x00, 0x04, 0x01, 0x01, 0x05, 0x00
    };
    // clang-format on

    RawData raw_data = { data_reserved, data_reserved + sizeof(data_reserved) };
    CHECK(WTPRadioFailAlarmIndication::Deserialize(&raw_data) == nullptr);

    raw_data = { data_cleared, data_cleared + sizeof(data_cleared) };
    CHECK(WTPRadioFailAlarmIndication::Deserialize(&raw_data) != nullptr);

    raw_data = { data_critical, data_critical + sizeof(data_critical) };
    CHECK(WTPRadioFailAlarmIndication::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid, data_invalid + sizeof(data_invalid) };
    CHECK(WTPRadioFailAlarmIndication::Deserialize(&raw_data) == nullptr);
}

TEST(WTPRadioFailAlarmIndicationTestsGroup, Deserialize_insufficient_data) {
    // clang-format off
    uint8_t data[] = {
        0x04, 0x17, 0x00, 0x04, 0x01, 0x01, 0x01  // Missing last byte
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    CHECK(WTPRadioFailAlarmIndication::Deserialize(&raw_data) == nullptr);
}

TEST(WTPRadioFailAlarmIndicationTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableWTPRadioFailAlarmIndicationArray w_alarms;

    w_alarms.Add({ 1, WTPRadioFailAlarmIndication::Receiver, WTPRadioFailAlarmIndication::Minor });
    w_alarms.Add(
        { 1, WTPRadioFailAlarmIndication::Transmitter, WTPRadioFailAlarmIndication::Major });
    w_alarms.Add(
        { 2, WTPRadioFailAlarmIndication::Receiver, WTPRadioFailAlarmIndication::Critical });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_alarms.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 24, raw_data.current); // 3 * 8 = 24

    // clang-format off
    uint8_t reference[] = {
        // Element 0: RadioID=1, Type=Receiver, Status=Minor
        0x04, 0x17, 0x00, 0x04, 0x01, 0x01, 0x02, 0x00,
        // Element 1: RadioID=1, Type=Transmitter, Status=Major
        0x04, 0x17, 0x00, 0x04, 0x01, 0x02, 0x03, 0x00,
        // Element 2: RadioID=2, Type=Receiver, Status=Critical
        0x04, 0x17, 0x00, 0x04, 0x02, 0x01, 0x04, 0x00
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    ReadableWTPRadioFailAlarmIndicationArray r_alarms;

    raw_data = { reference, reference + sizeof(reference) };

    CHECK_TRUE(r_alarms.Deserialize(&raw_data));
    CHECK_TRUE(r_alarms.Deserialize(&raw_data));
    CHECK_TRUE(r_alarms.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_alarms.Get().size());

    CHECK_EQUAL(1, r_alarms.Get()[0]->RadioID);
    CHECK_EQUAL(WTPRadioFailAlarmIndication::Receiver, r_alarms.Get()[0]->Type);
    CHECK_EQUAL(WTPRadioFailAlarmIndication::Minor, r_alarms.Get()[0]->Status);

    CHECK_EQUAL(1, r_alarms.Get()[1]->RadioID);
    CHECK_EQUAL(WTPRadioFailAlarmIndication::Transmitter, r_alarms.Get()[1]->Type);
    CHECK_EQUAL(WTPRadioFailAlarmIndication::Major, r_alarms.Get()[1]->Status);

    CHECK_EQUAL(2, r_alarms.Get()[2]->RadioID);
    CHECK_EQUAL(WTPRadioFailAlarmIndication::Receiver, r_alarms.Get()[2]->Type);
    CHECK_EQUAL(WTPRadioFailAlarmIndication::Critical, r_alarms.Get()[2]->Status);
}
