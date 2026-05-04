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
    CHECK_FALSE(r_ops.IsPresent());

    raw_data = { buffer, buffer + 60 };

    CHECK_TRUE(r_ops.Deserialize(&raw_data));
    CHECK_TRUE(r_ops.IsPresent());
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
    CHECK_FALSE(r_ops.IsPresent());

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
