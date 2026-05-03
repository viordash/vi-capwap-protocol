#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/DeleteWlan.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(DeleteWlanTestsGroup){ //
                                  TEST_SETUP(){}

                                  TEST_TEARDOWN(){}
};

TEST(DeleteWlanTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableDeleteWlanArray w_wlans;

    w_wlans.Add({ 1, 1 });
    w_wlans.Add({ 1, 2 });
    w_wlans.Add({ 2, 1 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_wlans.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 18, raw_data.current);

    // clang-format off
    uint8_t reference[] = {
        // Element 0: RadioID=1, WlanID=1
        0x04, 0x03, 0x00, 0x02, 0x01, 0x01,
        // Element 1: RadioID=1, WlanID=2
        0x04, 0x03, 0x00, 0x02, 0x01, 0x02,
        // Element 2: RadioID=2, WlanID=1
        0x04, 0x03, 0x00, 0x02, 0x02, 0x01
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    ReadableDeleteWlanArray r_wlans;

    raw_data = { reference, reference + sizeof(reference) };

    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_wlans.Get().size());

    CHECK_EQUAL(1, r_wlans.Get()[0]->RadioID);
    CHECK_EQUAL(1, r_wlans.Get()[0]->WlanID);

    CHECK_EQUAL(1, r_wlans.Get()[1]->RadioID);
    CHECK_EQUAL(2, r_wlans.Get()[1]->WlanID);

    CHECK_EQUAL(2, r_wlans.Get()[2]->RadioID);
    CHECK_EQUAL(1, r_wlans.Get()[2]->WlanID);
}

TEST(DeleteWlanTestsGroup, Add_array_of_items_is_unique) {
    uint8_t buffer[2048] = {};

    WritableDeleteWlanArray w_wlans;

    // Add same RadioID+WlanID multiple times
    w_wlans.Add({ 1, 1 });
    w_wlans.Add({ 1, 1 });
    w_wlans.Add({ 1, 1 });
    w_wlans.Add({ 2, 2 });
    w_wlans.Add({ 2, 2 });
    w_wlans.Add({ 2, 2 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_wlans.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableDeleteWlanArray r_wlans;

    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_FALSE(r_wlans.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_wlans.Get().size());

    CHECK_EQUAL(1, r_wlans.Get()[0]->RadioID);
    CHECK_EQUAL(1, r_wlans.Get()[0]->WlanID);

    CHECK_EQUAL(2, r_wlans.Get()[1]->RadioID);
    CHECK_EQUAL(2, r_wlans.Get()[1]->WlanID);
}
