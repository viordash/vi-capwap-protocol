#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/TxPower.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(TxPowerTestsGroup){ //
                               TEST_SETUP(){}

                               TEST_TEARDOWN(){}
};

TEST(TxPowerTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableTxPowerArray w_tps;

    w_tps.Add({ 1, 20 });
    w_tps.Add({ 3, 17 });
    w_tps.Add({ 2, 23 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_tps.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 24, raw_data.current); // 3 * 8 = 24

    // clang-format off
    uint8_t reference[] = {
        // Element 0: RadioID=1, Power=20
        0x04, 0x11, 0x00, 0x04, 0x01, 0x00, 0x00, 0x14,
        // Element 3: RadioID=1, Power=17
        0x04, 0x11, 0x00, 0x04, 0x03, 0x00, 0x00, 0x11,
        // Element 2: RadioID=2, Power=23
        0x04, 0x11, 0x00, 0x04, 0x02, 0x00, 0x00, 0x17
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    ReadableTxPowerArray r_tps;
    CHECK_FALSE(r_tps.IsPresent());

    raw_data = { reference, reference + sizeof(reference) };

    CHECK_TRUE(r_tps.Deserialize(&raw_data));
    CHECK_TRUE(r_tps.IsPresent());
    CHECK_TRUE(r_tps.Deserialize(&raw_data));
    CHECK_TRUE(r_tps.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_tps.Get().size());

    CHECK_EQUAL(1, r_tps.Get()[0]->RadioID);
    CHECK_EQUAL(20, r_tps.Get()[0]->CurrentTxPower.Get());

    CHECK_EQUAL(3, r_tps.Get()[1]->RadioID);
    CHECK_EQUAL(17, r_tps.Get()[1]->CurrentTxPower.Get());

    CHECK_EQUAL(2, r_tps.Get()[2]->RadioID);
    CHECK_EQUAL(23, r_tps.Get()[2]->CurrentTxPower.Get());
}

TEST(TxPowerTestsGroup, Add_array_of_items_is_unique_by_RadioID) {
    uint8_t buffer[2048] = {};

    WritableTxPowerArray w_tps;

    // Add same RadioID multiple times - should replace
    w_tps.Add({ 1, 20 });
    w_tps.Add({ 1, 17 }); // Replaces first
    w_tps.Add({ 2, 23 });
    w_tps.Add({ 2, 25 }); // Replaces second

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_tps.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableTxPowerArray r_tps;
    CHECK_FALSE(r_tps.IsPresent());

    CHECK_TRUE(r_tps.Deserialize(&raw_data));
    CHECK_TRUE(r_tps.IsPresent());
    CHECK_TRUE(r_tps.Deserialize(&raw_data));
    CHECK_FALSE(r_tps.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_tps.Get().size());

    // Should have the last values for each RadioID
    CHECK_EQUAL(1, r_tps.Get()[0]->RadioID);
    CHECK_EQUAL(17, r_tps.Get()[0]->CurrentTxPower.Get());

    CHECK_EQUAL(2, r_tps.Get()[1]->RadioID);
    CHECK_EQUAL(25, r_tps.Get()[1]->CurrentTxPower.Get());
}
