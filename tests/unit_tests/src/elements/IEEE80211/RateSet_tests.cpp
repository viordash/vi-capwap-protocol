#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/RateSet.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(RateSetTestsGroup){ //
                               TEST_SETUP(){}

                               TEST_TEARDOWN(){}
};

TEST(RateSetTestsGroup, Serialize_Deserialize_array) {
    uint8_t buffer[2048] = {};
    std::vector<uint8_t> rate_set1 = { 0x82, 0x84 };
    std::vector<uint8_t> rate_set2 = { 0x8B, 0x96, 0x0C, 0x12, 0x18, 0x24, 0x30, 0x48 };

    WritableRateSetArray w_rate_sets;
    w_rate_sets.Add(1, rate_set1);
    w_rate_sets.Add(2, rate_set2);

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_rate_sets.Serialize(&raw_data);
    // First: 4 (header) + 1 (radio_id) + 2 (rate_set) = 7
    // Second: 4 (header) + 1 (radio_id) + 8 (rate_set) = 13
    // Total: 7 + 13 = 20
    CHECK_EQUAL(&buffer[0] + 20, raw_data.current);

    ReadableRateSetArray r_rate_sets;
    CHECK_FALSE(r_rate_sets.IsPresent());

    raw_data = { buffer, buffer + 20 };

    CHECK_TRUE(r_rate_sets.Deserialize(&raw_data));
    CHECK_TRUE(r_rate_sets.IsPresent());
    CHECK_TRUE(r_rate_sets.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_rate_sets.Get().size());

    CHECK_EQUAL(1, r_rate_sets.Get()[0]->GetRadioID());
    CHECK_EQUAL(2 + 1, r_rate_sets.Get()[0]->GetLength());
    MEMCMP_EQUAL(rate_set1.data(), r_rate_sets.Get()[0]->data, 2);

    CHECK_EQUAL(2, r_rate_sets.Get()[1]->GetRadioID());
    CHECK_EQUAL(8 + 1, r_rate_sets.Get()[1]->GetLength());
    MEMCMP_EQUAL(rate_set2.data(), r_rate_sets.Get()[1]->data, 8);
}