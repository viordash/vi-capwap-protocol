#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/StationQoSProfile.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(StationQoSProfileTestsGroup){ //
                                         TEST_SETUP(){}

                                         TEST_TEARDOWN(){}
};

TEST(StationQoSProfileTestsGroup, Serialize_Deserialize_array) {
    uint8_t buffer[2048] = {};
    uint8_t mac1[6] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
    uint8_t mac2[6] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    uint8_t mac3[6] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 };

    WritableStationQoSProfileArray w_profiles;
    w_profiles.Add({ mac1, 0 });
    w_profiles.Add({ mac2, 3 });
    w_profiles.Add({ mac3, 7 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_profiles.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 36, raw_data.current); // 3 * 12 = 36

    ReadableStationQoSProfileArray r_profiles;
    raw_data = { buffer, buffer + 36 };

    CHECK_TRUE(r_profiles.Deserialize(&raw_data));
    CHECK_TRUE(r_profiles.Deserialize(&raw_data));
    CHECK_TRUE(r_profiles.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_profiles.Get().size());

    CHECK_EQUAL(0x11, r_profiles.Get()[0]->GetMACAddress()[0]);
    CHECK_EQUAL(0, r_profiles.Get()[0]->Get8021pPriority());

    CHECK_EQUAL(0xAA, r_profiles.Get()[1]->GetMACAddress()[0]);
    CHECK_EQUAL(3, r_profiles.Get()[1]->Get8021pPriority());

    CHECK_EQUAL(0x00, r_profiles.Get()[2]->GetMACAddress()[0]);
    CHECK_EQUAL(7, r_profiles.Get()[2]->Get8021pPriority());
}
