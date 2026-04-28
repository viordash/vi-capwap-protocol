#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/MACOperation.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(MACOperationTestsGroup){ //
                                    TEST_SETUP(){}

                                    TEST_TEARDOWN(){}
};

TEST(MACOperationTestsGroup, Deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Message Element Header (4 bytes) ----
        0x04, 0x06,     // Type: 1030 (IEEE 802.11 MAC Operation)
        0x00, 0x10,     // Length: 16 bytes

        // ---- Value (16 bytes) ----
        0x01,           // Radio ID: 1
        0x00,           // Reserved: 0
        0x09, 0x2B,     // RTS Threshold: 2347 (default)
        0x07,           // Short Retry: 7 (default)
        0x04,           // Long Retry: 4 (default)
        0x09, 0x2A,     // Fragmentation Threshold: 2346
        0x00, 0x00,     // Tx MSDU Lifetime: 512 (0x00000200)
        0x02, 0x00,
        0x00, 0x00,     // Rx MSDU Lifetime: 512 (0x00000200)
        0x02, 0x00
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = MACOperation::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::MACOperation, element->GetElementType());

    CHECK_EQUAL(1, element->GetRadioID());
    CHECK_EQUAL(2347, element->GetRTSThreshold());
    CHECK_EQUAL(7, element->GetShortRetry());
    CHECK_EQUAL(4, element->GetLongRetry());
    CHECK_EQUAL(2346, element->GetFragmentationThreshold());
    CHECK_EQUAL(512, element->GetTxMSDULifetime());
    CHECK_EQUAL(512, element->GetRxMSDULifetime());
}

TEST(MACOperationTestsGroup, Serialize) {
    uint8_t buffer[256] = {};
    MACOperation element_0{ 5, 2000, 10, 8, 1500, 1024, 2048 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 20, raw_data.current);

    // clang-format off
    const uint8_t reference[] = {
        0x04, 0x06,     // Type: 1030
        0x00, 0x10,     // Length: 16
        0x05,           // Radio ID: 5
        0x00,           // Reserved: 0
        0x07, 0xD0,     // RTS Threshold: 2000
        0x0A,           // Short Retry: 10
        0x08,           // Long Retry: 8
        0x05, 0xDC,     // Fragmentation Threshold: 1500
        0x00, 0x00,     // Tx MSDU Lifetime: 1024 (0x00000400)
        0x04, 0x00,
        0x00, 0x00,     // Rx MSDU Lifetime: 2048 (0x00000800)
        0x08, 0x00
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = MACOperation::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(&buffer[0] + 20, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::MACOperation, element->GetElementType());
    CHECK_EQUAL(5, element->GetRadioID());
    CHECK_EQUAL(2000, element->GetRTSThreshold());
    CHECK_EQUAL(10, element->GetShortRetry());
    CHECK_EQUAL(8, element->GetLongRetry());
    CHECK_EQUAL(1500, element->GetFragmentationThreshold());
    CHECK_EQUAL(1024, element->GetTxMSDULifetime());
    CHECK_EQUAL(2048, element->GetRxMSDULifetime());
}

TEST(MACOperationTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableMACOperationArray w_ops;

    w_ops.Add({ 1, 2347, 7, 4, 2346, 512, 512 });
    w_ops.Add({ 2, 1000, 5, 3, 1000, 256, 256 });
    w_ops.Add({ 3, 500, 3, 2, 500, 128, 128 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_ops.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 60, raw_data.current);

    ReadableMACOperationArray r_ops;

    raw_data = { buffer, buffer + 60 };

    CHECK_TRUE(r_ops.Deserialize(&raw_data));
    CHECK_TRUE(r_ops.Deserialize(&raw_data));
    CHECK_TRUE(r_ops.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_ops.Get().size());

    CHECK_EQUAL(1, r_ops.Get()[0]->GetRadioID());
    CHECK_EQUAL(2347, r_ops.Get()[0]->GetRTSThreshold());
    CHECK_EQUAL(7, r_ops.Get()[0]->GetShortRetry());
    CHECK_EQUAL(4, r_ops.Get()[0]->GetLongRetry());
    CHECK_EQUAL(2346, r_ops.Get()[0]->GetFragmentationThreshold());
    CHECK_EQUAL(512, r_ops.Get()[0]->GetTxMSDULifetime());
    CHECK_EQUAL(512, r_ops.Get()[0]->GetRxMSDULifetime());

    CHECK_EQUAL(2, r_ops.Get()[1]->GetRadioID());
    CHECK_EQUAL(1000, r_ops.Get()[1]->GetRTSThreshold());
    CHECK_EQUAL(5, r_ops.Get()[1]->GetShortRetry());
    CHECK_EQUAL(3, r_ops.Get()[1]->GetLongRetry());
    CHECK_EQUAL(1000, r_ops.Get()[1]->GetFragmentationThreshold());
    CHECK_EQUAL(256, r_ops.Get()[1]->GetTxMSDULifetime());
    CHECK_EQUAL(256, r_ops.Get()[1]->GetRxMSDULifetime());

    CHECK_EQUAL(3, r_ops.Get()[2]->GetRadioID());
    CHECK_EQUAL(500, r_ops.Get()[2]->GetRTSThreshold());
    CHECK_EQUAL(3, r_ops.Get()[2]->GetShortRetry());
    CHECK_EQUAL(2, r_ops.Get()[2]->GetLongRetry());
    CHECK_EQUAL(500, r_ops.Get()[2]->GetFragmentationThreshold());
    CHECK_EQUAL(128, r_ops.Get()[2]->GetTxMSDULifetime());
    CHECK_EQUAL(128, r_ops.Get()[2]->GetRxMSDULifetime());
}

TEST(MACOperationTestsGroup, Add_array_of_items_is_unique_by_RadioID) {
    uint8_t buffer[2048] = {};

    WritableMACOperationArray w_ops;

    // Add same RadioID multiple times - should replace
    w_ops.Add({ 1, 1000, 5, 3, 1000, 256, 256 });
    w_ops.Add({ 1, 2000, 6, 4, 2000, 512, 512 });
    w_ops.Add({ 1, 2347, 7, 4, 2346, 512, 512 });
    w_ops.Add({ 2, 500, 3, 2, 500, 128, 128 });
    w_ops.Add({ 2, 1500, 8, 5, 1500, 1024, 1024 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_ops.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableMACOperationArray r_ops;

    CHECK_TRUE(r_ops.Deserialize(&raw_data));
    CHECK_TRUE(r_ops.Deserialize(&raw_data));
    CHECK_FALSE(r_ops.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_ops.Get().size());

    // Should have last values for each RadioID
    CHECK_EQUAL(1, r_ops.Get()[0]->GetRadioID());
    CHECK_EQUAL(2347, r_ops.Get()[0]->GetRTSThreshold());
    CHECK_EQUAL(7, r_ops.Get()[0]->GetShortRetry());

    CHECK_EQUAL(2, r_ops.Get()[1]->GetRadioID());
    CHECK_EQUAL(1500, r_ops.Get()[1]->GetRTSThreshold());
    CHECK_EQUAL(8, r_ops.Get()[1]->GetShortRetry());
}

TEST(MACOperationTestsGroup, Validate_RadioID_range) {
    // clang-format off
    // Valid RadioID = 0
    uint8_t data_valid_zero[] = {
        0x04, 0x06, 0x00, 0x10,
        0x00, 0x00, 0x09, 0x2B, 0x07, 0x04, 0x09, 0x2A,
        0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00
    };
    // Valid RadioID = 1
    uint8_t data_valid_min[] = {
        0x04, 0x06, 0x00, 0x10,
        0x01, 0x00, 0x09, 0x2B, 0x07, 0x04, 0x09, 0x2A,
        0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00
    };
    // Valid RadioID = 31
    uint8_t data_valid_max[] = {
        0x04, 0x06, 0x00, 0x10,
        0x1F, 0x00, 0x09, 0x2B, 0x07, 0x04, 0x09, 0x2A,
        0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00
    };
    // Invalid RadioID = 32
    uint8_t data_invalid_high[] = {
        0x04, 0x06, 0x00, 0x10,
        0x20, 0x00, 0x09, 0x2B, 0x07, 0x04, 0x09, 0x2A,
        0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00
    };
    // clang-format on

    RawData raw_data = { data_valid_zero, data_valid_zero + sizeof(data_valid_zero) };
    CHECK(MACOperation::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_min, data_valid_min + sizeof(data_valid_min) };
    CHECK(MACOperation::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_max, data_valid_max + sizeof(data_valid_max) };
    CHECK(MACOperation::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid_high, data_invalid_high + sizeof(data_invalid_high) };
    CHECK(MACOperation::Deserialize(&raw_data) == nullptr);
}

TEST(MACOperationTestsGroup, Default_values) {
    // Test with RFC default values
    uint8_t buffer[256] = {};
    MACOperation element{ 1, 2347, 7, 4, 2346, 512, 512 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element.Serialize(&raw_data);

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = MACOperation::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(2347, deserialized->GetRTSThreshold());
    CHECK_EQUAL(7, deserialized->GetShortRetry());
    CHECK_EQUAL(4, deserialized->GetLongRetry());
    CHECK_EQUAL(2346, deserialized->GetFragmentationThreshold());
    CHECK_EQUAL(512, deserialized->GetTxMSDULifetime());
    CHECK_EQUAL(512, deserialized->GetRxMSDULifetime());
}
