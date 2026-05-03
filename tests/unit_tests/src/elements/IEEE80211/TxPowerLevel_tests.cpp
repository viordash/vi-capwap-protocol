#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/TxPowerLevel.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(TxPowerLevelTestsGroup){ //
                                    TEST_SETUP(){}

                                    TEST_TEARDOWN(){}
};

TEST(TxPowerLevelTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableTxPowerLevelArray w_levels;

    std::vector<int16_t> levels_0 = { 20 };
    std::vector<int16_t> levels_1 = { 17, 14, 11 };
    std::vector<int16_t> levels_2 = { 23, 20, 17, 14, 11 };

    w_levels.Add({ 1, levels_0 });
    w_levels.Add({ 1, levels_1 });
    w_levels.Add({ 2, levels_2 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_levels.Serialize(&raw_data);
    // Element 0: 4 + 2 + 1*2 = 8 bytes
    // Element 1: 4 + 2 + 3*2 = 12 bytes
    // Element 2: 4 + 2 + 5*2 = 16 bytes
    // Total: 36 bytes
    CHECK_EQUAL(&buffer[0] + 36, raw_data.current);

    ReadableTxPowerLevelArray r_levels;
    CHECK_FALSE(r_levels.IsPresent());

    raw_data = { buffer, buffer + 36 };

    CHECK_TRUE(r_levels.Deserialize(&raw_data));
    CHECK_TRUE(r_levels.IsPresent());
    CHECK_TRUE(r_levels.Deserialize(&raw_data));
    CHECK_TRUE(r_levels.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_levels.Get().size());

    CHECK_EQUAL(1, r_levels.Get()[0]->GetRadioID());
    CHECK_EQUAL(1, r_levels.Get()[0]->GetNumLevels());
    CHECK_EQUAL(2 + 2, r_levels.Get()[0]->GetLength());
    MEMCMP_EQUAL(levels_0.data(), r_levels.Get()[0]->data, 2);

    CHECK_EQUAL(1, r_levels.Get()[1]->GetRadioID());
    CHECK_EQUAL(3, r_levels.Get()[1]->GetNumLevels());
    CHECK_EQUAL(6 + 2, r_levels.Get()[1]->GetLength());
    MEMCMP_EQUAL(levels_1.data(), r_levels.Get()[1]->data, 6);

    CHECK_EQUAL(2, r_levels.Get()[2]->GetRadioID());
    CHECK_EQUAL(5, r_levels.Get()[2]->GetNumLevels());
    CHECK_EQUAL(10 + 2, r_levels.Get()[2]->GetLength());
    MEMCMP_EQUAL(levels_2.data(), r_levels.Get()[2]->data, 10);
}
