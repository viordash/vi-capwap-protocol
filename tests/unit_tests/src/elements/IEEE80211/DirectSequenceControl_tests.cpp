#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/DirectSequenceControl.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(DirectSequenceControlTestsGroup){ //
                                             TEST_SETUP(){}

                                             TEST_TEARDOWN(){}
};

TEST(DirectSequenceControlTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableDirectSequenceControlArray w_ctrls;

    w_ctrls.Add({ 1, 1, DirectSequenceControl::CCAMode::EdOnly, 100 });
    w_ctrls.Add({ 2, 6, DirectSequenceControl::CCAMode::CsOnly, 200 });
    w_ctrls.Add({ 3, 11, DirectSequenceControl::CCAMode::EdAndCs, 300 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_ctrls.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 36, raw_data.current);

    // clang-format off
    uint8_t reference[] = {
        // Element 0: RadioID=1, Channel=1, CCA=EdOnly, Threshold=100
        0x04, 0x04, 0x00, 0x08, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x64,
        // Element 1: RadioID=2, Channel=6, CCA=CsOnly, Threshold=200
        0x04, 0x04, 0x00, 0x08, 0x02, 0x00, 0x06, 0x02, 0x00, 0x00, 0x00, 0xC8,
        // Element 2: RadioID=3, Channel=11, CCA=EdAndCs, Threshold=300
        0x04, 0x04, 0x00, 0x08, 0x03, 0x00, 0x0B, 0x04, 0x00, 0x00, 0x01, 0x2C
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    ReadableDirectSequenceControlArray r_ctrls;

    raw_data = { reference, reference + sizeof(reference) };

    CHECK_TRUE(r_ctrls.Deserialize(&raw_data));
    CHECK_TRUE(r_ctrls.Deserialize(&raw_data));
    CHECK_TRUE(r_ctrls.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_ctrls.Get().size());

    CHECK_EQUAL(1, r_ctrls.Get()[0]->GetRadioID());
    CHECK_EQUAL(1, r_ctrls.Get()[0]->GetCurrentChannel());
    CHECK_EQUAL(DirectSequenceControl::CCAMode::EdOnly, r_ctrls.Get()[0]->GetCurrentCCA());
    CHECK_EQUAL(100, r_ctrls.Get()[0]->GetEnergyDetectThreshold());

    CHECK_EQUAL(2, r_ctrls.Get()[1]->GetRadioID());
    CHECK_EQUAL(6, r_ctrls.Get()[1]->GetCurrentChannel());
    CHECK_EQUAL(DirectSequenceControl::CCAMode::CsOnly, r_ctrls.Get()[1]->GetCurrentCCA());
    CHECK_EQUAL(200, r_ctrls.Get()[1]->GetEnergyDetectThreshold());

    CHECK_EQUAL(3, r_ctrls.Get()[2]->GetRadioID());
    CHECK_EQUAL(11, r_ctrls.Get()[2]->GetCurrentChannel());
    CHECK_EQUAL(DirectSequenceControl::CCAMode::EdAndCs, r_ctrls.Get()[2]->GetCurrentCCA());
    CHECK_EQUAL(300, r_ctrls.Get()[2]->GetEnergyDetectThreshold());
}

TEST(DirectSequenceControlTestsGroup, Add_array_of_items_is_unique_by_RadioID) {
    uint8_t buffer[2048] = {};

    WritableDirectSequenceControlArray w_ctrls;

    // Add same RadioID multiple times - should replace
    w_ctrls.Add({ 1, 1, DirectSequenceControl::CCAMode::EdOnly, 100 });
    w_ctrls.Add({ 1, 6, DirectSequenceControl::CCAMode::CsOnly, 200 });
    w_ctrls.Add({ 1, 11, DirectSequenceControl::CCAMode::EdAndCs, 300 });
    w_ctrls.Add({ 2, 1, DirectSequenceControl::CCAMode::CsWithTimer, 400 });
    w_ctrls.Add({ 2, 6, DirectSequenceControl::CCAMode::HrCsAndEd, 500 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_ctrls.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableDirectSequenceControlArray r_ctrls;

    CHECK_TRUE(r_ctrls.Deserialize(&raw_data));
    CHECK_TRUE(r_ctrls.Deserialize(&raw_data));
    CHECK_FALSE(r_ctrls.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_ctrls.Get().size());

    // Should have last values for each RadioID
    CHECK_EQUAL(1, r_ctrls.Get()[0]->GetRadioID());
    CHECK_EQUAL(11, r_ctrls.Get()[0]->GetCurrentChannel());
    CHECK_EQUAL(DirectSequenceControl::CCAMode::EdAndCs, r_ctrls.Get()[0]->GetCurrentCCA());
    CHECK_EQUAL(300, r_ctrls.Get()[0]->GetEnergyDetectThreshold());

    CHECK_EQUAL(2, r_ctrls.Get()[1]->GetRadioID());
    CHECK_EQUAL(6, r_ctrls.Get()[1]->GetCurrentChannel());
    CHECK_EQUAL(DirectSequenceControl::CCAMode::HrCsAndEd, r_ctrls.Get()[1]->GetCurrentCCA());
    CHECK_EQUAL(500, r_ctrls.Get()[1]->GetEnergyDetectThreshold());
}
