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

    std::vector<NetworkU16> levels_0 = { 20 };
    std::vector<NetworkU16> levels_1 = { 17, 14, 11 };
    std::vector<NetworkU16> levels_2 = { 23456, 20, 17890, 14, 11 };

    w_levels.Add({ 1, levels_0 });
    w_levels.Add({ 3, levels_1 });
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
    CHECK_EQUAL(1, r_levels.Get()[0]->GetNumLevels());
    CHECK_EQUAL(20, r_levels.Get()[0]->data[0].Get());

    CHECK_EQUAL(3, r_levels.Get()[1]->GetRadioID());
    CHECK_EQUAL(3, r_levels.Get()[1]->GetNumLevels());
    CHECK_EQUAL(6 + 2, r_levels.Get()[1]->GetLength());
    CHECK_EQUAL(3, r_levels.Get()[1]->GetNumLevels());
    CHECK_EQUAL(17, r_levels.Get()[1]->data[0].Get());
    CHECK_EQUAL(14, r_levels.Get()[1]->data[1].Get());
    CHECK_EQUAL(11, r_levels.Get()[1]->data[2].Get());

    CHECK_EQUAL(2, r_levels.Get()[2]->GetRadioID());
    CHECK_EQUAL(5, r_levels.Get()[2]->GetNumLevels());
    CHECK_EQUAL(10 + 2, r_levels.Get()[2]->GetLength());
    CHECK_EQUAL(5, r_levels.Get()[2]->GetNumLevels());
    CHECK_EQUAL(23456, r_levels.Get()[2]->data[0].Get());
    CHECK_EQUAL(20, r_levels.Get()[2]->data[1].Get());
    CHECK_EQUAL(17890, r_levels.Get()[2]->data[2].Get());
    CHECK_EQUAL(14, r_levels.Get()[2]->data[3].Get());
    CHECK_EQUAL(11, r_levels.Get()[2]->data[4].Get());
}

TEST(TxPowerLevelTestsGroup, Add_array_of_items_is_unique_by_RadioID) {
    uint8_t buffer[2048] = {};

    WritableTxPowerLevelArray w_levels;

    std::vector<NetworkU16> levels_0 = { 20 };
    std::vector<NetworkU16> levels_1 = { 17, 14, 11 };
    std::vector<NetworkU16> levels_2 = { 23, 20, 17, 14, 11 };
    std::vector<NetworkU16> levels_3 = { 25, 22 };

    // Add same RadioID multiple times - should replace
    w_levels.Add({ 1, levels_0 });
    w_levels.Add({ 1, levels_1 }); // Replaces first
    w_levels.Add({ 2, levels_2 });
    w_levels.Add({ 2, levels_3 }); // Replaces second

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_levels.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableTxPowerLevelArray r_levels;
    CHECK_FALSE(r_levels.IsPresent());

    CHECK_TRUE(r_levels.Deserialize(&raw_data));
    CHECK_TRUE(r_levels.IsPresent());
    CHECK_TRUE(r_levels.Deserialize(&raw_data));
    CHECK_FALSE(r_levels.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_levels.Get().size());

    // Should have the last values for each RadioID
    CHECK_EQUAL(1, r_levels.Get()[0]->GetRadioID());
    CHECK_EQUAL(3, r_levels.Get()[0]->GetNumLevels());
    MEMCMP_EQUAL(levels_1.data(), r_levels.Get()[0]->data, 6);

    CHECK_EQUAL(2, r_levels.Get()[1]->GetRadioID());
    CHECK_EQUAL(2, r_levels.Get()[1]->GetNumLevels());
    MEMCMP_EQUAL(levels_3.data(), r_levels.Get()[1]->data, 4);
}
