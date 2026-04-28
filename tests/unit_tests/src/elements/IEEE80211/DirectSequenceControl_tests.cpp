#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/DirectSequenceControl.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(DirectSequenceControlTestsGroup){ //
                                             TEST_SETUP(){}

                                             TEST_TEARDOWN(){}
};

TEST(DirectSequenceControlTestsGroup, Deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Message Element Header (4 bytes) ----
        0x04, 0x04,     // Type: 1028 (IEEE 802.11 Direct Sequence Control)
        0x00, 0x08,     // Length: 8 bytes

        // ---- Value (8 bytes) ----
        0x01,           // Radio ID: 1
        0x00,           // Reserved: 0
        0x06,           // Current Channel: 6
        0x04,           // Current CCA: EdAndCs (4)
        0x00, 0x00,     // Energy Detect Threshold: 100 (0x00000064)
        0x00, 0x64
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = DirectSequenceControl::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::DirectSequenceControl, element->GetElementType());

    CHECK_EQUAL(1, element->GetRadioID());
    CHECK_EQUAL(6, element->GetCurrentChannel());
    CHECK_EQUAL(DirectSequenceControl::CCAMode::EdAndCs, element->GetCurrentCCA());
    CHECK_EQUAL(100, element->GetEnergyDetectThreshold());
}

TEST(DirectSequenceControlTestsGroup, Serialize) {
    uint8_t buffer[256] = {};
    DirectSequenceControl element_0{ 5, 11, DirectSequenceControl::CCAMode::CsOnly, 200 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 12, raw_data.current);
    // clang-format off
    const uint8_t reference[] = {
        0x04, 0x04,     // Type: 1028
        0x00, 0x08,     // Length: 8
        0x05,           // Radio ID: 5
        0x00,           // Reserved: 0
        0x0B,           // Current Channel: 11
        0x02,           // Current CCA: CsOnly (2)
        0x00, 0x00,     // Energy Detect Threshold: 200 (0x000000C8)
        0x00, 0xC8
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = DirectSequenceControl::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(&buffer[0] + 12, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::DirectSequenceControl, element->GetElementType());
    CHECK_EQUAL(5, element->GetRadioID());
    CHECK_EQUAL(11, element->GetCurrentChannel());
    CHECK_EQUAL(DirectSequenceControl::CCAMode::CsOnly, element->GetCurrentCCA());
    CHECK_EQUAL(200, element->GetEnergyDetectThreshold());
}

TEST(DirectSequenceControlTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableDirectSequenceControlArray w_ctrls;

    w_ctrls.Add({ 1, 1, DirectSequenceControl::CCAMode::EdOnly, 100 });
    w_ctrls.Add({ 2, 6, DirectSequenceControl::CCAMode::CsOnly, 200 });
    w_ctrls.Add({ 3, 11, DirectSequenceControl::CCAMode::EdAndCs, 300 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_ctrls.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 36, raw_data.current);

    // clang-format off
    uint8_t reference[] = {
        // Element 0: RadioID=1, Channel=1, CCA=EdOnly, Threshold=100
        0x04, 0x04, 0x00, 0x08, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x64,
        // Element 1: RadioID=2, Channel=6, CCA=CsOnly, Threshold=200
        0x04, 0x04, 0x00, 0x08, 0x02, 0x00, 0x06, 0x02, 0x00, 0x00, 0x00, 0xC8,
        // Element 2: RadioID=3, Channel=11, CCA=EdAndCs, Threshold=300
        0x04, 0x04, 0x00, 0x08, 0x03, 0x00, 0x0B, 0x04, 0x00, 0x00, 0x01, 0x2C
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    ReadableDirectSequenceControlArray r_ctrls;

    raw_data = { reference, reference + sizeof(reference) };

    CHECK_TRUE(r_ctrls.Deserialize(&raw_data));
    CHECK_TRUE(r_ctrls.Deserialize(&raw_data));
    CHECK_TRUE(r_ctrls.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_ctrls.Get().size());

    CHECK_EQUAL(1, r_ctrls.Get()[0]->GetRadioID());
    CHECK_EQUAL(1, r_ctrls.Get()[0]->GetCurrentChannel());
    CHECK_EQUAL(DirectSequenceControl::CCAMode::EdOnly, r_ctrls.Get()[0]->GetCurrentCCA());
    CHECK_EQUAL(100, r_ctrls.Get()[0]->GetEnergyDetectThreshold());

    CHECK_EQUAL(2, r_ctrls.Get()[1]->GetRadioID());
    CHECK_EQUAL(6, r_ctrls.Get()[1]->GetCurrentChannel());
    CHECK_EQUAL(DirectSequenceControl::CCAMode::CsOnly, r_ctrls.Get()[1]->GetCurrentCCA());
    CHECK_EQUAL(200, r_ctrls.Get()[1]->GetEnergyDetectThreshold());

    CHECK_EQUAL(3, r_ctrls.Get()[2]->GetRadioID());
    CHECK_EQUAL(11, r_ctrls.Get()[2]->GetCurrentChannel());
    CHECK_EQUAL(DirectSequenceControl::CCAMode::EdAndCs, r_ctrls.Get()[2]->GetCurrentCCA());
    CHECK_EQUAL(300, r_ctrls.Get()[2]->GetEnergyDetectThreshold());
}

TEST(DirectSequenceControlTestsGroup, Add_array_of_items_is_unique_by_RadioID) {
    uint8_t buffer[2048] = {};

    WritableDirectSequenceControlArray w_ctrls;

    // Add same RadioID multiple times - should replace
    w_ctrls.Add({ 1, 1, DirectSequenceControl::CCAMode::EdOnly, 100 });
    w_ctrls.Add({ 1, 6, DirectSequenceControl::CCAMode::CsOnly, 200 });
    w_ctrls.Add({ 1, 11, DirectSequenceControl::CCAMode::EdAndCs, 300 });
    w_ctrls.Add({ 2, 1, DirectSequenceControl::CCAMode::CsWithTimer, 400 });
    w_ctrls.Add({ 2, 6, DirectSequenceControl::CCAMode::HrCsAndEd, 500 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_ctrls.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableDirectSequenceControlArray r_ctrls;

    CHECK_TRUE(r_ctrls.Deserialize(&raw_data));
    CHECK_TRUE(r_ctrls.Deserialize(&raw_data));
    CHECK_FALSE(r_ctrls.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_ctrls.Get().size());

    // Should have last values for each RadioID
    CHECK_EQUAL(1, r_ctrls.Get()[0]->GetRadioID());
    CHECK_EQUAL(11, r_ctrls.Get()[0]->GetCurrentChannel());
    CHECK_EQUAL(DirectSequenceControl::CCAMode::EdAndCs, r_ctrls.Get()[0]->GetCurrentCCA());
    CHECK_EQUAL(300, r_ctrls.Get()[0]->GetEnergyDetectThreshold());

    CHECK_EQUAL(2, r_ctrls.Get()[1]->GetRadioID());
    CHECK_EQUAL(6, r_ctrls.Get()[1]->GetCurrentChannel());
    CHECK_EQUAL(DirectSequenceControl::CCAMode::HrCsAndEd, r_ctrls.Get()[1]->GetCurrentCCA());
    CHECK_EQUAL(500, r_ctrls.Get()[1]->GetEnergyDetectThreshold());
}

TEST(DirectSequenceControlTestsGroup, Validate_RadioID_range) {
    // clang-format off
    // Valid RadioID = 0
    uint8_t data_valid_zero[] = {
        0x04, 0x04, 0x00, 0x08, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x64
    };
    // Valid RadioID = 1
    uint8_t data_valid_min[] = {
        0x04, 0x04, 0x00, 0x08, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x64
    };
    // Valid RadioID = 31
    uint8_t data_valid_max[] = {
        0x04, 0x04, 0x00, 0x08, 0x1F, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x64
    };
    // Invalid RadioID = 32
    uint8_t data_invalid_high[] = {
        0x04, 0x04, 0x00, 0x08, 0x20, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x64
    };
    // clang-format on

    RawData raw_data = { data_valid_zero, data_valid_zero + sizeof(data_valid_zero) };
    CHECK(DirectSequenceControl::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_min, data_valid_min + sizeof(data_valid_min) };
    CHECK(DirectSequenceControl::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_max, data_valid_max + sizeof(data_valid_max) };
    CHECK(DirectSequenceControl::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid_high, data_invalid_high + sizeof(data_invalid_high) };
    CHECK(DirectSequenceControl::Deserialize(&raw_data) == nullptr);
}

TEST(DirectSequenceControlTestsGroup, Validate_CCAMode) {
    // clang-format off
    // Valid CCA = EdOnly (1)
    uint8_t data_cca_1[] = {
        0x04, 0x04, 0x00, 0x08, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x64
    };
    // Valid CCA = CsOnly (2)
    uint8_t data_cca_2[] = {
        0x04, 0x04, 0x00, 0x08, 0x01, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x64
    };
    // Valid CCA = EdAndCs (4)
    uint8_t data_cca_4[] = {
        0x04, 0x04, 0x00, 0x08, 0x01, 0x00, 0x01, 0x04, 0x00, 0x00, 0x00, 0x64
    };
    // Valid CCA = CsWithTimer (8)
    uint8_t data_cca_8[] = {
        0x04, 0x04, 0x00, 0x08, 0x01, 0x00, 0x01, 0x08, 0x00, 0x00, 0x00, 0x64
    };
    // Valid CCA = HrCsAndEd (16)
    uint8_t data_cca_16[] = {
        0x04, 0x04, 0x00, 0x08, 0x01, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x64
    };
    // Invalid CCA = 0
    uint8_t data_cca_invalid_0[] = {
        0x04, 0x04, 0x00, 0x08, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x64
    };
    // Invalid CCA = 3
    uint8_t data_cca_invalid_3[] = {
        0x04, 0x04, 0x00, 0x08, 0x01, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x64
    };
    // Invalid CCA = 17
    uint8_t data_cca_invalid_17[] = {
        0x04, 0x04, 0x00, 0x08, 0x01, 0x00, 0x01, 0x11, 0x00, 0x00, 0x00, 0x64
    };
    // clang-format on

    RawData raw_data = { data_cca_1, data_cca_1 + sizeof(data_cca_1) };
    CHECK(DirectSequenceControl::Deserialize(&raw_data) != nullptr);

    raw_data = { data_cca_2, data_cca_2 + sizeof(data_cca_2) };
    CHECK(DirectSequenceControl::Deserialize(&raw_data) != nullptr);

    raw_data = { data_cca_4, data_cca_4 + sizeof(data_cca_4) };
    CHECK(DirectSequenceControl::Deserialize(&raw_data) != nullptr);

    raw_data = { data_cca_8, data_cca_8 + sizeof(data_cca_8) };
    CHECK(DirectSequenceControl::Deserialize(&raw_data) != nullptr);

    raw_data = { data_cca_16, data_cca_16 + sizeof(data_cca_16) };
    CHECK(DirectSequenceControl::Deserialize(&raw_data) != nullptr);

    raw_data = { data_cca_invalid_0, data_cca_invalid_0 + sizeof(data_cca_invalid_0) };
    CHECK(DirectSequenceControl::Deserialize(&raw_data) == nullptr);

    raw_data = { data_cca_invalid_3, data_cca_invalid_3 + sizeof(data_cca_invalid_3) };
    CHECK(DirectSequenceControl::Deserialize(&raw_data) == nullptr);

    raw_data = { data_cca_invalid_17, data_cca_invalid_17 + sizeof(data_cca_invalid_17) };
    CHECK(DirectSequenceControl::Deserialize(&raw_data) == nullptr);
}

TEST(DirectSequenceControlTestsGroup, Large_EnergyDetectThreshold) {
    uint8_t buffer[256] = {};
    // Use max uint32_t value
    DirectSequenceControl element_0{ 1, 6, DirectSequenceControl::CCAMode::EdOnly, 0xFFFFFFFF };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = DirectSequenceControl::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(0xFFFFFFFF, element->GetEnergyDetectThreshold());
}
