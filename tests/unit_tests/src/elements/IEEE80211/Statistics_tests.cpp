#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/Statistics.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(StatisticsTestsGroup){ //
                                  TEST_SETUP(){}

                                  TEST_TEARDOWN(){}
};

TEST(StatisticsTestsGroup, Deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Message Element Header (4 bytes) ----
        0x04, 0x0F,     // Type: 1039 (IEEE 802.11 Statistics)
        0x00, 0x50,     // Length: 80 bytes

        // ---- Value (80 bytes) ----
        0x01,                         // Radio ID: 1
        0x00, 0x00, 0x00,             // Reserved: 0

        0x00, 0x00, 0x00, 0x64,       // Tx Fragment Count: 100
        0x00, 0x00, 0x00, 0xC8,       // Multicast Tx Count: 200
        0x00, 0x00, 0x00, 0x0A,       // Failed Count: 10
        0x00, 0x00, 0x00, 0x14,       // Retry Count: 20
        0x00, 0x00, 0x00, 0x05,       // Multiple Retry Count: 5
        0x00, 0x00, 0x00, 0x03,       // Frame Duplicate Count: 3
        0x00, 0x00, 0x01, 0xF4,       // RTS Success Count: 500
        0x00, 0x00, 0x00, 0x02,       // RTS Failure Count: 2
        0x00, 0x00, 0x00, 0x07,       // ACK Failure Count: 7
        0x00, 0x00, 0x00, 0x96,       // Rx Fragment Count: 150
        0x00, 0x00, 0x01, 0x2C,       // Multicast RX Count: 300
        0x00, 0x00, 0x00, 0x04,       // FCS Error Count: 4
        0x00, 0x00, 0x03, 0xE8,       // Tx Frame Count: 1000
        0x00, 0x00, 0x00, 0x01,       // Decryption Errors: 1
        0x00, 0x00, 0x00, 0x02,       // Discarded QoS Fragment Count: 2
        0x00, 0x00, 0x00, 0x19,       // Associated Station Count: 25
        0x00, 0x00, 0x00, 0x32,       // QoS CF Polls Received Count: 50
        0x00, 0x00, 0x00, 0x06,       // QoS CF Polls Unused Count: 6
        0x00, 0x00, 0x00, 0x08        // QoS CF Polls Unusable Count: 8
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = Statistics::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::Statistics, element->GetElementType());

    CHECK_EQUAL(1, element->GetRadioID());
    CHECK_EQUAL(100, element->GetTxFragmentCount());
    CHECK_EQUAL(200, element->GetMulticastTxCount());
    CHECK_EQUAL(10, element->GetFailedCount());
    CHECK_EQUAL(20, element->GetRetryCount());
    CHECK_EQUAL(5, element->GetMultipleRetryCount());
    CHECK_EQUAL(3, element->GetFrameDuplicateCount());
    CHECK_EQUAL(500, element->GetRTSSuccessCount());
    CHECK_EQUAL(2, element->GetRTSFailureCount());
    CHECK_EQUAL(7, element->GetACKFailureCount());
    CHECK_EQUAL(150, element->GetRxFragmentCount());
    CHECK_EQUAL(300, element->GetMulticastRxCount());
    CHECK_EQUAL(4, element->GetFCSErrorCount());
    CHECK_EQUAL(1000, element->GetTxFrameCount());
    CHECK_EQUAL(1, element->GetDecryptionErrors());
    CHECK_EQUAL(2, element->GetDiscardedQoSFragmentCount());
    CHECK_EQUAL(25, element->GetAssociatedStationCount());
    CHECK_EQUAL(50, element->GetQoSCFPollsReceivedCount());
    CHECK_EQUAL(6, element->GetQoSCFPollsUnusedCount());
    CHECK_EQUAL(8, element->GetQoSCFPollsUnusableCount());
}

TEST(StatisticsTestsGroup, Serialize) {
    uint8_t buffer[256] = {};
    Statistics element{ 5, 1000, 2000, 50, 100, 25, 10, 800, 5, 15, 500, 600, 8, 5000, 3, 4, 30, 100, 12, 16 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 84, raw_data.current);

    // clang-format off
    const uint8_t reference[] = {
        0x04, 0x0F,                   // Type: 1039
        0x00, 0x50,                   // Length: 80

        0x05,                         // Radio ID: 5
        0x00, 0x00, 0x00,             // Reserved: 0

        0x00, 0x00, 0x03, 0xE8,       // Tx Fragment Count: 1000
        0x00, 0x00, 0x07, 0xD0,       // Multicast Tx Count: 2000
        0x00, 0x00, 0x00, 0x32,       // Failed Count: 50
        0x00, 0x00, 0x00, 0x64,       // Retry Count: 100
        0x00, 0x00, 0x00, 0x19,       // Multiple Retry Count: 25
        0x00, 0x00, 0x00, 0x0A,       // Frame Duplicate Count: 10
        0x00, 0x00, 0x03, 0x20,       // RTS Success Count: 800
        0x00, 0x00, 0x00, 0x05,       // RTS Failure Count: 5
        0x00, 0x00, 0x00, 0x0F,       // ACK Failure Count: 15
        0x00, 0x00, 0x01, 0xF4,       // Rx Fragment Count: 500
        0x00, 0x00, 0x02, 0x58,       // Multicast RX Count: 600
        0x00, 0x00, 0x00, 0x08,       // FCS Error Count: 8
        0x00, 0x00, 0x13, 0x88,       // Tx Frame Count: 5000
        0x00, 0x00, 0x00, 0x03,       // Decryption Errors: 3
        0x00, 0x00, 0x00, 0x04,       // Discarded QoS Fragment Count: 4
        0x00, 0x00, 0x00, 0x1E,       // Associated Station Count: 30
        0x00, 0x00, 0x00, 0x64,       // QoS CF Polls Received Count: 100
        0x00, 0x00, 0x00, 0x0C,       // QoS CF Polls Unused Count: 12
        0x00, 0x00, 0x00, 0x10        // QoS CF Polls Unusable Count: 16
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = Statistics::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(&buffer[0] + 84, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::Statistics, deserialized->GetElementType());
    CHECK_EQUAL(5, deserialized->GetRadioID());
    CHECK_EQUAL(1000, deserialized->GetTxFragmentCount());
    CHECK_EQUAL(2000, deserialized->GetMulticastTxCount());
    CHECK_EQUAL(50, deserialized->GetFailedCount());
    CHECK_EQUAL(100, deserialized->GetRetryCount());
    CHECK_EQUAL(25, deserialized->GetMultipleRetryCount());
    CHECK_EQUAL(10, deserialized->GetFrameDuplicateCount());
    CHECK_EQUAL(800, deserialized->GetRTSSuccessCount());
    CHECK_EQUAL(5, deserialized->GetRTSFailureCount());
    CHECK_EQUAL(15, deserialized->GetACKFailureCount());
    CHECK_EQUAL(500, deserialized->GetRxFragmentCount());
    CHECK_EQUAL(600, deserialized->GetMulticastRxCount());
    CHECK_EQUAL(8, deserialized->GetFCSErrorCount());
    CHECK_EQUAL(5000, deserialized->GetTxFrameCount());
    CHECK_EQUAL(3, deserialized->GetDecryptionErrors());
    CHECK_EQUAL(4, deserialized->GetDiscardedQoSFragmentCount());
    CHECK_EQUAL(30, deserialized->GetAssociatedStationCount());
    CHECK_EQUAL(100, deserialized->GetQoSCFPollsReceivedCount());
    CHECK_EQUAL(12, deserialized->GetQoSCFPollsUnusedCount());
    CHECK_EQUAL(16, deserialized->GetQoSCFPollsUnusableCount());
}

TEST(StatisticsTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableStatisticsArray w_stats;

    w_stats.Add({ 1, 100, 200, 10, 20, 5, 3, 500, 2, 7, 150, 300, 4, 1000, 1, 2, 25, 50, 6, 8 });
    w_stats.Add({ 2, 200, 400, 20, 40, 10, 6, 1000, 4, 14, 300, 600, 8, 2000, 2, 4, 50, 100, 12, 16 });
    w_stats.Add({ 3, 300, 600, 30, 60, 15, 9, 1500, 6, 21, 450, 900, 12, 3000, 3, 6, 75, 150, 18, 24 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_stats.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 252, raw_data.current); // 3 * 84 = 252

    ReadableStatisticsArray r_stats;

    raw_data = { buffer, buffer + 252 };

    CHECK_TRUE(r_stats.Deserialize(&raw_data));
    CHECK_TRUE(r_stats.Deserialize(&raw_data));
    CHECK_TRUE(r_stats.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_stats.Get().size());

    CHECK_EQUAL(1, r_stats.Get()[0]->GetRadioID());
    CHECK_EQUAL(100, r_stats.Get()[0]->GetTxFragmentCount());
    CHECK_EQUAL(25, r_stats.Get()[0]->GetAssociatedStationCount());

    CHECK_EQUAL(2, r_stats.Get()[1]->GetRadioID());
    CHECK_EQUAL(200, r_stats.Get()[1]->GetTxFragmentCount());
    CHECK_EQUAL(50, r_stats.Get()[1]->GetAssociatedStationCount());

    CHECK_EQUAL(3, r_stats.Get()[2]->GetRadioID());
    CHECK_EQUAL(300, r_stats.Get()[2]->GetTxFragmentCount());
    CHECK_EQUAL(75, r_stats.Get()[2]->GetAssociatedStationCount());
}

TEST(StatisticsTestsGroup, Add_array_of_items_is_unique_by_RadioID) {
    uint8_t buffer[2048] = {};

    WritableStatisticsArray w_stats;

    // Add same RadioID multiple times - should replace
    w_stats.Add({ 1, 100, 200, 10, 20, 5, 3, 500, 2, 7, 150, 300, 4, 1000, 1, 2, 25, 50, 6, 8 });
    w_stats.Add({ 1, 200, 400, 20, 40, 10, 6, 1000, 4, 14, 300, 600, 8, 2000, 2, 4, 50, 100, 12, 16 });
    w_stats.Add({ 1, 300, 600, 30, 60, 15, 9, 1500, 6, 21, 450, 900, 12, 3000, 3, 6, 75, 150, 18, 24 });
    w_stats.Add({ 2, 400, 800, 40, 80, 20, 12, 2000, 8, 28, 600, 1200, 16, 4000, 4, 8, 100, 200, 24, 32 });
    w_stats.Add({ 2, 500, 1000, 50, 100, 25, 15, 2500, 10, 35, 750, 1500, 20, 5000, 5, 10, 125, 250, 30, 40 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_stats.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableStatisticsArray r_stats;

    CHECK_TRUE(r_stats.Deserialize(&raw_data));
    CHECK_TRUE(r_stats.Deserialize(&raw_data));
    CHECK_FALSE(r_stats.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_stats.Get().size());

    // Should have last values for each RadioID
    CHECK_EQUAL(1, r_stats.Get()[0]->GetRadioID());
    CHECK_EQUAL(300, r_stats.Get()[0]->GetTxFragmentCount());
    CHECK_EQUAL(75, r_stats.Get()[0]->GetAssociatedStationCount());

    CHECK_EQUAL(2, r_stats.Get()[1]->GetRadioID());
    CHECK_EQUAL(500, r_stats.Get()[1]->GetTxFragmentCount());
    CHECK_EQUAL(125, r_stats.Get()[1]->GetAssociatedStationCount());
}

TEST(StatisticsTestsGroup, Validate_RadioID_range) {
    // clang-format off
    // Valid RadioID = 0
    uint8_t data_valid_zero[] = {
        0x04, 0x0F, 0x00, 0x50,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };
    // Valid RadioID = 1
    uint8_t data_valid_min[] = {
        0x04, 0x0F, 0x00, 0x50,
        0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };
    // Valid RadioID = 31
    uint8_t data_valid_max[] = {
        0x04, 0x0F, 0x00, 0x50,
        0x1F, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };
    // Invalid RadioID = 32
    uint8_t data_invalid_high[] = {
        0x04, 0x0F, 0x00, 0x50,
        0x20, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };
    // clang-format on

    RawData raw_data = { data_valid_zero, data_valid_zero + sizeof(data_valid_zero) };
    CHECK(Statistics::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_min, data_valid_min + sizeof(data_valid_min) };
    CHECK(Statistics::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_max, data_valid_max + sizeof(data_valid_max) };
    CHECK(Statistics::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid_high, data_invalid_high + sizeof(data_invalid_high) };
    CHECK(Statistics::Deserialize(&raw_data) == nullptr);
}

TEST(StatisticsTestsGroup, All_counters_at_max_value) {
    uint8_t buffer[256] = {};
    Statistics element{ 1,
                        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element.Serialize(&raw_data);

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = Statistics::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(0xFFFFFFFF, deserialized->GetTxFragmentCount());
    CHECK_EQUAL(0xFFFFFFFF, deserialized->GetMulticastTxCount());
    CHECK_EQUAL(0xFFFFFFFF, deserialized->GetFailedCount());
    CHECK_EQUAL(0xFFFFFFFF, deserialized->GetRetryCount());
    CHECK_EQUAL(0xFFFFFFFF, deserialized->GetMultipleRetryCount());
    CHECK_EQUAL(0xFFFFFFFF, deserialized->GetFrameDuplicateCount());
    CHECK_EQUAL(0xFFFFFFFF, deserialized->GetRTSSuccessCount());
    CHECK_EQUAL(0xFFFFFFFF, deserialized->GetRTSFailureCount());
    CHECK_EQUAL(0xFFFFFFFF, deserialized->GetACKFailureCount());
    CHECK_EQUAL(0xFFFFFFFF, deserialized->GetRxFragmentCount());
    CHECK_EQUAL(0xFFFFFFFF, deserialized->GetMulticastRxCount());
    CHECK_EQUAL(0xFFFFFFFF, deserialized->GetFCSErrorCount());
    CHECK_EQUAL(0xFFFFFFFF, deserialized->GetTxFrameCount());
    CHECK_EQUAL(0xFFFFFFFF, deserialized->GetDecryptionErrors());
    CHECK_EQUAL(0xFFFFFFFF, deserialized->GetDiscardedQoSFragmentCount());
    CHECK_EQUAL(0xFFFFFFFF, deserialized->GetAssociatedStationCount());
    CHECK_EQUAL(0xFFFFFFFF, deserialized->GetQoSCFPollsReceivedCount());
    CHECK_EQUAL(0xFFFFFFFF, deserialized->GetQoSCFPollsUnusedCount());
    CHECK_EQUAL(0xFFFFFFFF, deserialized->GetQoSCFPollsUnusableCount());
}
