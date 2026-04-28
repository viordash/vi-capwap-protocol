#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/TxPower.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(TxPowerTestsGroup){ //
                               TEST_SETUP(){}

                               TEST_TEARDOWN(){}
};

TEST(TxPowerTestsGroup, Deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Message Element Header (4 bytes) ----
        0x04, 0x11,     // Type: 1041 (IEEE 802.11 Tx Power)
        0x00, 0x04,     // Length: 4 bytes

        // ---- Value (4 bytes) ----
        0x01,           // Radio ID: 1
        0x00,           // Reserved: 0
        0x00, 0x14      // Current Tx Power: 20 dBm
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = TxPower::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::TxPower, element->GetElementType());

    CHECK_EQUAL(1, element->RadioID);
    CHECK_EQUAL(0, element->Reserved);
    CHECK_EQUAL(20, element->CurrentTxPower.Get());
}

TEST(TxPowerTestsGroup, Serialize) {
    uint8_t buffer[256] = {};
    TxPower element{ 5, 17 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 8, raw_data.current);

    // clang-format off
    const uint8_t reference[] = {
        0x04, 0x11,     // Type: 1041
        0x00, 0x04,     // Length: 4
        0x05,           // Radio ID: 5
        0x00,           // Reserved: 0
        0x00, 0x11      // Current Tx Power: 17 dBm
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = TxPower::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(&buffer[0] + 8, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::TxPower, deserialized->GetElementType());
    CHECK_EQUAL(5, deserialized->RadioID);
    CHECK_EQUAL(17, deserialized->CurrentTxPower.Get());
}

TEST(TxPowerTestsGroup, Validate_RadioID_range) {
    // clang-format off
    // Valid RadioID = 0
    uint8_t data_valid_zero[] = {
        0x04, 0x11, 0x00, 0x04, 0x00, 0x00, 0x00, 0x14
    };
    // Valid RadioID = 31
    uint8_t data_valid_max[] = {
        0x04, 0x11, 0x00, 0x04, 0x1F, 0x00, 0x00, 0x14
    };
    // Invalid RadioID = 32
    uint8_t data_invalid_high[] = {
        0x04, 0x11, 0x00, 0x04, 0x20, 0x00, 0x00, 0x14
    };
    // clang-format on

    RawData raw_data = { data_valid_zero, data_valid_zero + sizeof(data_valid_zero) };
    CHECK(TxPower::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_max, data_valid_max + sizeof(data_valid_max) };
    CHECK(TxPower::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid_high, data_invalid_high + sizeof(data_invalid_high) };
    CHECK(TxPower::Deserialize(&raw_data) == nullptr);
}

TEST(TxPowerTestsGroup, Deserialize_wrong_type) {
    // clang-format off
    uint8_t data[] = {
        0x04, 0x10,     // Wrong Type: 1040 (not TxPower)
        0x00, 0x04,
        0x01, 0x00, 0x00, 0x14
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    CHECK(TxPower::Deserialize(&raw_data) == nullptr);
}

TEST(TxPowerTestsGroup, Deserialize_wrong_length) {
    // clang-format off
    uint8_t data[] = {
        0x04, 0x11,
        0x00, 0x05,     // Wrong Length: 5 (should be 4)
        0x01, 0x00, 0x00, 0x14, 0x00
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    CHECK(TxPower::Deserialize(&raw_data) == nullptr);
}

TEST(TxPowerTestsGroup, Deserialize_insufficient_data) {
    // clang-format off
    uint8_t data[] = {
        0x04, 0x11, 0x00, 0x04, 0x01, 0x00, 0x00  // Missing last byte
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    CHECK(TxPower::Deserialize(&raw_data) == nullptr);
}

TEST(TxPowerTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableTxPowerArray w_tps;

    w_tps.Add({ 1, 20 });
    w_tps.Add({ 1, 17 });
    w_tps.Add({ 2, 23 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_tps.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 24, raw_data.current); // 3 * 8 = 24

    // clang-format off
    uint8_t reference[] = {
        // Element 0: RadioID=1, Power=20
        0x04, 0x11, 0x00, 0x04, 0x01, 0x00, 0x00, 0x14,
        // Element 1: RadioID=1, Power=17
        0x04, 0x11, 0x00, 0x04, 0x01, 0x00, 0x00, 0x11,
        // Element 2: RadioID=2, Power=23
        0x04, 0x11, 0x00, 0x04, 0x02, 0x00, 0x00, 0x17
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    ReadableTxPowerArray r_tps;

    raw_data = { reference, reference + sizeof(reference) };

    CHECK_TRUE(r_tps.Deserialize(&raw_data));
    CHECK_TRUE(r_tps.Deserialize(&raw_data));
    CHECK_TRUE(r_tps.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_tps.Get().size());

    CHECK_EQUAL(1, r_tps.Get()[0]->RadioID);
    CHECK_EQUAL(20, r_tps.Get()[0]->CurrentTxPower.Get());

    CHECK_EQUAL(1, r_tps.Get()[1]->RadioID);
    CHECK_EQUAL(17, r_tps.Get()[1]->CurrentTxPower.Get());

    CHECK_EQUAL(2, r_tps.Get()[2]->RadioID);
    CHECK_EQUAL(23, r_tps.Get()[2]->CurrentTxPower.Get());
}
