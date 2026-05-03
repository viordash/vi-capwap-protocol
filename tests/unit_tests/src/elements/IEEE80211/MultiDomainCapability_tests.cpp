#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/MultiDomainCapability.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(MultiDomainCapabilityTestsGroup){ //
                                             TEST_SETUP(){}

                                             TEST_TEARDOWN(){}
};

TEST(MultiDomainCapabilityTestsGroup, Serialize_Deserialize_array) {
    uint8_t buffer[2048] = {};

    WritableMultiDomainCapabilityArray w_capabilities;
    w_capabilities.Add({ 1, 1, 13, 20 });
    w_capabilities.Add({ 2, 36, 4, 17 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_capabilities.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 24, raw_data.current); // 2 × 12 = 24

    ReadableMultiDomainCapabilityArray r_capabilities;
    CHECK_FALSE(r_capabilities.IsPresent());
    raw_data = { buffer, buffer + 24 };

    CHECK_TRUE(r_capabilities.Deserialize(&raw_data));
    CHECK_TRUE(r_capabilities.IsPresent());
    CHECK_TRUE(r_capabilities.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_capabilities.Get().size());

    CHECK_EQUAL(1, r_capabilities.Get()[0]->GetRadioID());
    CHECK_EQUAL(1, r_capabilities.Get()[0]->GetFirstChannel());
    CHECK_EQUAL(13, r_capabilities.Get()[0]->GetNumberOfChannels());
    CHECK_EQUAL(20, r_capabilities.Get()[0]->GetMaxTxPowerLevel());

    CHECK_EQUAL(2, r_capabilities.Get()[1]->GetRadioID());
    CHECK_EQUAL(36, r_capabilities.Get()[1]->GetFirstChannel());
    CHECK_EQUAL(4, r_capabilities.Get()[1]->GetNumberOfChannels());
    CHECK_EQUAL(17, r_capabilities.Get()[1]->GetMaxTxPowerLevel());
}

TEST(MultiDomainCapabilityTestsGroup, Add_array_of_items_is_unique_by_RadioID) {
    uint8_t buffer[2048] = {};

    WritableMultiDomainCapabilityArray w_capabilities;

    // Add same RadioID multiple times - should replace
    w_capabilities.Add({ 1, 1, 13, 20 });
    w_capabilities.Add({ 1, 36, 8, 30 });    // Replaces first
    w_capabilities.Add({ 2, 36, 4, 17 });
    w_capabilities.Add({ 2, 100, 11, 25 });  // Replaces second

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_capabilities.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableMultiDomainCapabilityArray r_capabilities;
    CHECK_FALSE(r_capabilities.IsPresent());

    CHECK_TRUE(r_capabilities.Deserialize(&raw_data));
    CHECK_TRUE(r_capabilities.IsPresent());
    CHECK_TRUE(r_capabilities.Deserialize(&raw_data));
    CHECK_FALSE(r_capabilities.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_capabilities.Get().size());

    // Should have the last values for each RadioID
    CHECK_EQUAL(1, r_capabilities.Get()[0]->GetRadioID());
    CHECK_EQUAL(36, r_capabilities.Get()[0]->GetFirstChannel());
    CHECK_EQUAL(8, r_capabilities.Get()[0]->GetNumberOfChannels());
    CHECK_EQUAL(30, r_capabilities.Get()[0]->GetMaxTxPowerLevel());

    CHECK_EQUAL(2, r_capabilities.Get()[1]->GetRadioID());
    CHECK_EQUAL(100, r_capabilities.Get()[1]->GetFirstChannel());
    CHECK_EQUAL(11, r_capabilities.Get()[1]->GetNumberOfChannels());
    CHECK_EQUAL(25, r_capabilities.Get()[1]->GetMaxTxPowerLevel());
}