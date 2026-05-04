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

TEST(StatisticsTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableStatisticsArray w_stats;

    w_stats.Add({ 1, 100, 200, 10, 20, 5, 3, 500, 2, 7, 150, 300, 4, 1000, 1, 2, 25, 50, 6, 8 });
    w_stats.Add(
        { 2, 200, 400, 20, 40, 10, 6, 1000, 4, 14, 300, 600, 8, 2000, 2, 4, 50, 100, 12, 16 });
    w_stats.Add(
        { 3, 300, 600, 30, 60, 15, 9, 1500, 6, 21, 450, 900, 12, 3000, 3, 6, 75, 150, 18, 24 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_stats.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 252, raw_data.current); // 3 * 84 = 252

    ReadableStatisticsArray r_stats;
    CHECK_FALSE(r_stats.IsPresent());

    raw_data = { buffer, buffer + 252 };

    CHECK_TRUE(r_stats.Deserialize(&raw_data));
    CHECK_TRUE(r_stats.IsPresent());
    CHECK_TRUE(r_stats.Deserialize(&raw_data));
    CHECK_TRUE(r_stats.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_stats.Get().size());

    CHECK_EQUAL(1, r_stats.Get()[0]->GetRadioID());
    CHECK_EQUAL(100, r_stats.Get()[0]->GetTxFragmentCount());
    CHECK_EQUAL(200, r_stats.Get()[0]->GetMulticastTxCount());
    CHECK_EQUAL(10, r_stats.Get()[0]->GetFailedCount());
    CHECK_EQUAL(20, r_stats.Get()[0]->GetRetryCount());
    CHECK_EQUAL(5, r_stats.Get()[0]->GetMultipleRetryCount());
    CHECK_EQUAL(3, r_stats.Get()[0]->GetFrameDuplicateCount());
    CHECK_EQUAL(500, r_stats.Get()[0]->GetRTSSuccessCount());
    CHECK_EQUAL(2, r_stats.Get()[0]->GetRTSFailureCount());
    CHECK_EQUAL(7, r_stats.Get()[0]->GetACKFailureCount());
    CHECK_EQUAL(150, r_stats.Get()[0]->GetRxFragmentCount());
    CHECK_EQUAL(300, r_stats.Get()[0]->GetMulticastRxCount());
    CHECK_EQUAL(4, r_stats.Get()[0]->GetFCSErrorCount());
    CHECK_EQUAL(1000, r_stats.Get()[0]->GetTxFrameCount());
    CHECK_EQUAL(1, r_stats.Get()[0]->GetDecryptionErrors());
    CHECK_EQUAL(2, r_stats.Get()[0]->GetDiscardedQoSFragmentCount());
    CHECK_EQUAL(25, r_stats.Get()[0]->GetAssociatedStationCount());
    CHECK_EQUAL(50, r_stats.Get()[0]->GetQoSCFPollsReceivedCount());
    CHECK_EQUAL(6, r_stats.Get()[0]->GetQoSCFPollsUnusedCount());
    CHECK_EQUAL(8, r_stats.Get()[0]->GetQoSCFPollsUnusableCount());

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
    w_stats.Add(
        { 1, 200, 400, 20, 40, 10, 6, 1000, 4, 14, 300, 600, 8, 2000, 2, 4, 50, 100, 12, 16 });
    w_stats.Add(
        { 1, 300, 600, 30, 60, 15, 9, 1500, 6, 21, 450, 900, 12, 3000, 3, 6, 75, 150, 18, 24 });
    w_stats.Add(
        { 2, 400, 800, 40, 80, 20, 12, 2000, 8, 28, 600, 1200, 16, 4000, 4, 8, 100, 200, 24, 32 });
    w_stats.Add({ 2,   500,  1000, 50,   100, 25, 15,  2500, 10, 35,
                  750, 1500, 20,   5000, 5,   10, 125, 250,  30, 40 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_stats.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableStatisticsArray r_stats;
    CHECK_FALSE(r_stats.IsPresent());

    CHECK_TRUE(r_stats.Deserialize(&raw_data));
    CHECK_TRUE(r_stats.Deserialize(&raw_data));
    CHECK_FALSE(r_stats.Deserialize(&raw_data));
    CHECK_TRUE(r_stats.IsPresent());

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
