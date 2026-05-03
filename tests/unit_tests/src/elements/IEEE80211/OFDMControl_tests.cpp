#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/OFDMControl.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(OFDMControlTestsGroup){ //
                                   TEST_SETUP(){}

                                   TEST_TEARDOWN(){}
};

TEST(OFDMControlTestsGroup, Serialize_Deserialize_array) {
    uint8_t buffer[2048] = {};

    WritableOFDMControlArray w_controls;
    w_controls.Add({ 1, 36, 0x01, 100 });
    w_controls.Add({ 2, 40, 0x02, 200 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_controls.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 24, raw_data.current); // 2 × 12 = 24

    ReadableOFDMControlArray r_controls;
    raw_data = { buffer, buffer + 24 };

    CHECK_TRUE(r_controls.Deserialize(&raw_data));
    CHECK_TRUE(r_controls.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_controls.Get().size());

    CHECK_EQUAL(1, r_controls.Get()[0]->GetRadioID());
    CHECK_EQUAL(36, r_controls.Get()[0]->GetCurrentChannel());
    CHECK_EQUAL(0x01, r_controls.Get()[0]->GetBandSupport());
    CHECK_EQUAL(100, r_controls.Get()[0]->GetTIThreshold());

    CHECK_EQUAL(2, r_controls.Get()[1]->GetRadioID());
    CHECK_EQUAL(40, r_controls.Get()[1]->GetCurrentChannel());
    CHECK_EQUAL(0x02, r_controls.Get()[1]->GetBandSupport());
    CHECK_EQUAL(200, r_controls.Get()[1]->GetTIThreshold());
}
