#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/WTPRadioFailAlarmIndication.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(WTPRadioFailAlarmIndicationTestsGroup){ //
                                                   TEST_SETUP(){}

                                                   TEST_TEARDOWN(){}
};

TEST(WTPRadioFailAlarmIndicationTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableWTPRadioFailAlarmIndicationArray w_alarms;

    w_alarms.Add({ 1, WTPRadioFailAlarmIndication::Receiver, WTPRadioFailAlarmIndication::Minor });
    w_alarms.Add(
        { 1, WTPRadioFailAlarmIndication::Transmitter, WTPRadioFailAlarmIndication::Major });
    w_alarms.Add(
        { 2, WTPRadioFailAlarmIndication::Receiver, WTPRadioFailAlarmIndication::Critical });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_alarms.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 24, raw_data.current); // 3 * 8 = 24

    // clang-format off
    uint8_t reference[] = {
        // Element 0: RadioID=1, Type=Receiver, Status=Minor
        0x04, 0x17, 0x00, 0x04, 0x01, 0x01, 0x02, 0x00,
        // Element 1: RadioID=1, Type=Transmitter, Status=Major
        0x04, 0x17, 0x00, 0x04, 0x01, 0x02, 0x03, 0x00,
        // Element 2: RadioID=2, Type=Receiver, Status=Critical
        0x04, 0x17, 0x00, 0x04, 0x02, 0x01, 0x04, 0x00
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    ReadableWTPRadioFailAlarmIndicationArray r_alarms;
    CHECK_FALSE(r_alarms.IsPresent());

    raw_data = { reference, reference + sizeof(reference) };

    CHECK_TRUE(r_alarms.Deserialize(&raw_data));
    CHECK_TRUE(r_alarms.IsPresent());
    CHECK_TRUE(r_alarms.Deserialize(&raw_data));
    CHECK_TRUE(r_alarms.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_alarms.Get().size());

    CHECK_EQUAL(1, r_alarms.Get()[0]->RadioID);
    CHECK_EQUAL(WTPRadioFailAlarmIndication::Receiver, r_alarms.Get()[0]->Type);
    CHECK_EQUAL(WTPRadioFailAlarmIndication::Minor, r_alarms.Get()[0]->Status);

    CHECK_EQUAL(1, r_alarms.Get()[1]->RadioID);
    CHECK_EQUAL(WTPRadioFailAlarmIndication::Transmitter, r_alarms.Get()[1]->Type);
    CHECK_EQUAL(WTPRadioFailAlarmIndication::Major, r_alarms.Get()[1]->Status);

    CHECK_EQUAL(2, r_alarms.Get()[2]->RadioID);
    CHECK_EQUAL(WTPRadioFailAlarmIndication::Receiver, r_alarms.Get()[2]->Type);
    CHECK_EQUAL(WTPRadioFailAlarmIndication::Critical, r_alarms.Get()[2]->Status);
}
