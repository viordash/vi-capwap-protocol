#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/WTPQualityOfService.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(WTPQualityOfServiceTestsGroup){ //
                                           TEST_SETUP(){}

                                           TEST_TEARDOWN(){}
};

TEST(WTPQualityOfServiceTestsGroup, QoSSubElement_TagFields) {
    QoSSubElement qos{};

    // Test 8021p tag
    qos.Set8021pTag(0);
    CHECK_EQUAL(0, qos.Get8021pTag());

    qos.Set8021pTag(7);
    CHECK_EQUAL(7, qos.Get8021pTag());

    qos.Set8021pTag(5);
    CHECK_EQUAL(5, qos.Get8021pTag());

    // Test overflow handling (8021p is 3 bits max = 7)
    qos.Set8021pTag(0xFF);
    CHECK_EQUAL(7, qos.Get8021pTag());

    // Test DSCP tag
    qos.SetDscpTag(0);
    CHECK_EQUAL(0, qos.GetDscpTag());

    qos.SetDscpTag(63);
    CHECK_EQUAL(63, qos.GetDscpTag());

    qos.SetDscpTag(42);
    CHECK_EQUAL(42, qos.GetDscpTag());

    // Test overflow handling (DSCP is 6 bits max = 63)
    qos.SetDscpTag(0xFF);
    CHECK_EQUAL(63, qos.GetDscpTag());

    // Test that setting one doesn't affect the other
    qos.Set8021pTag(5);
    qos.SetDscpTag(30);
    CHECK_EQUAL(5, qos.Get8021pTag());
    CHECK_EQUAL(30, qos.GetDscpTag());

    qos.Set8021pTag(3);
    CHECK_EQUAL(3, qos.Get8021pTag());
    CHECK_EQUAL(30, qos.GetDscpTag());

    qos.SetDscpTag(15);
    CHECK_EQUAL(3, qos.Get8021pTag());
    CHECK_EQUAL(15, qos.GetDscpTag());
}

TEST(WTPQualityOfServiceTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableWTPQualityOfServiceArray w_qos;

    // P bit set (802.1p enabled) = 0x10
    WTPQualityOfService elem1{ 1, 0x10 };
    elem1.Voice.QueueDepth = 4;
    elem1.Voice.CWMin = NetworkU16{ 15 };
    elem1.Voice.CWMax = NetworkU16{ 31 };
    w_qos.Add(elem1);

    // D bit set (DSCP enabled) = 0x04
    WTPQualityOfService elem2{ 2, 0x04 };
    elem2.Voice.QueueDepth = 8;
    elem2.Voice.CWMin = NetworkU16{ 31 };
    elem2.Voice.CWMax = NetworkU16{ 63 };
    w_qos.Add(elem2);

    // P and D bits set (both enabled) = 0x14
    WTPQualityOfService elem3{ 3, 0x14 };
    elem3.Voice.QueueDepth = 2;
    elem3.Voice.CWMin = NetworkU16{ 7 };
    elem3.Voice.CWMax = NetworkU16{ 15 };
    w_qos.Add(elem3);

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_qos.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 114, raw_data.current); // 3 * 38 = 114

    ReadableWTPQualityOfServiceArray r_qos;
    CHECK_FALSE(r_qos.IsPresent());

    raw_data = { buffer, buffer + 114 };

    CHECK_TRUE(r_qos.Deserialize(&raw_data));
    CHECK_TRUE(r_qos.IsPresent());
    CHECK_TRUE(r_qos.Deserialize(&raw_data));
    CHECK_TRUE(r_qos.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_qos.Get().size());

    CHECK_EQUAL(1, r_qos.Get()[0]->RadioID);
    CHECK_EQUAL(0x10, r_qos.Get()[0]->TaggingPolicy);
    CHECK_TRUE(r_qos.Get()[0]->GetP());
    CHECK_FALSE(r_qos.Get()[0]->GetD());
    CHECK_EQUAL(4, r_qos.Get()[0]->Voice.QueueDepth);
    CHECK_EQUAL(15, r_qos.Get()[0]->Voice.CWMin.Get());
    CHECK_EQUAL(31, r_qos.Get()[0]->Voice.CWMax.Get());

    CHECK_EQUAL(2, r_qos.Get()[1]->RadioID);
    CHECK_EQUAL(0x04, r_qos.Get()[1]->TaggingPolicy);
    CHECK_FALSE(r_qos.Get()[1]->GetP());
    CHECK_TRUE(r_qos.Get()[1]->GetD());
    CHECK_EQUAL(8, r_qos.Get()[1]->Voice.QueueDepth);
    CHECK_EQUAL(31, r_qos.Get()[1]->Voice.CWMin.Get());
    CHECK_EQUAL(63, r_qos.Get()[1]->Voice.CWMax.Get());

    CHECK_EQUAL(3, r_qos.Get()[2]->RadioID);
    CHECK_EQUAL(0x14, r_qos.Get()[2]->TaggingPolicy);
    CHECK_TRUE(r_qos.Get()[2]->GetP());
    CHECK_TRUE(r_qos.Get()[2]->GetD());
    CHECK_EQUAL(2, r_qos.Get()[2]->Voice.QueueDepth);
    CHECK_EQUAL(7, r_qos.Get()[2]->Voice.CWMin.Get());
    CHECK_EQUAL(15, r_qos.Get()[2]->Voice.CWMax.Get());
}

TEST(WTPQualityOfServiceTestsGroup, TaggingPolicy_bit_field_accessors) {
    // Test all bit field accessors for TaggingPolicy:
    //      0 1 2 3 4 5 6 7
    //     +-+-+-+-+-+-+-+-+
    //     |Rsvd |P|Q|D|O|I|
    //     +-+-+-+-+-+-+-+-+

    WTPQualityOfService element{ 1, 0x00 };

    // Test initial state (all zeros)
    CHECK_FALSE(element.GetP());
    CHECK_FALSE(element.GetQ());
    CHECK_FALSE(element.GetD());
    CHECK_FALSE(element.GetO());
    CHECK_FALSE(element.GetI());
    CHECK_EQUAL(0x00, element.TaggingPolicy);

    // Test setting P bit (bit 4) - 802.1p priority tag enable
    element.SetP(true);
    CHECK_TRUE(element.GetP());
    CHECK_EQUAL(0x10, element.TaggingPolicy);
    element.SetP(false);
    CHECK_FALSE(element.GetP());
    CHECK_EQUAL(0x00, element.TaggingPolicy);

    // Test setting Q bit (bit 3) - 802.1p behavior
    element.SetQ(true);
    CHECK_TRUE(element.GetQ());
    CHECK_EQUAL(0x08, element.TaggingPolicy);
    element.SetQ(false);
    CHECK_FALSE(element.GetQ());
    CHECK_EQUAL(0x00, element.TaggingPolicy);

    // Test setting D bit (bit 2) - DSCP enable
    element.SetD(true);
    CHECK_TRUE(element.GetD());
    CHECK_EQUAL(0x04, element.TaggingPolicy);
    element.SetD(false);
    CHECK_FALSE(element.GetD());
    CHECK_EQUAL(0x00, element.TaggingPolicy);

    // Test setting O bit (bit 1) - DSCP outer
    element.SetO(true);
    CHECK_TRUE(element.GetO());
    CHECK_EQUAL(0x02, element.TaggingPolicy);
    element.SetO(false);
    CHECK_FALSE(element.GetO());
    CHECK_EQUAL(0x00, element.TaggingPolicy);

    // Test setting I bit (bit 0) - DSCP inner
    element.SetI(true);
    CHECK_TRUE(element.GetI());
    CHECK_EQUAL(0x01, element.TaggingPolicy);
    element.SetI(false);
    CHECK_FALSE(element.GetI());
    CHECK_EQUAL(0x00, element.TaggingPolicy);

    // Test setting multiple bits doesn't affect others
    element.SetP(true);
    element.SetD(true);
    element.SetI(true);
    CHECK_TRUE(element.GetP());
    CHECK_FALSE(element.GetQ());
    CHECK_TRUE(element.GetD());
    CHECK_FALSE(element.GetO());
    CHECK_TRUE(element.GetI());
    CHECK_EQUAL(0x15, element.TaggingPolicy); // P(0x10) + D(0x04) + I(0x01) = 0x15

    // Test clearing one bit doesn't affect others
    element.SetD(false);
    CHECK_TRUE(element.GetP());
    CHECK_FALSE(element.GetQ());
    CHECK_FALSE(element.GetD());
    CHECK_FALSE(element.GetO());
    CHECK_TRUE(element.GetI());
    CHECK_EQUAL(0x11, element.TaggingPolicy); // P(0x10) + I(0x01) = 0x11

    // Test all bits set
    element.SetP(true);
    element.SetQ(true);
    element.SetD(true);
    element.SetO(true);
    element.SetI(true);
    CHECK_EQUAL(0x1F, element.TaggingPolicy);
}

TEST(WTPQualityOfServiceTestsGroup, Serialize_Deserialize_array) {
    uint8_t buffer[2048] = {};

    WritableWTPQualityOfServiceArray w_qos;

    WTPQualityOfService elem1{ 1, 0x10 };
    elem1.Voice.QueueDepth = 4;
    elem1.Voice.CWMin = NetworkU16{ 15 };
    elem1.Voice.CWMax = NetworkU16{ 31 };
    w_qos.Add(elem1);

    WTPQualityOfService elem2{ 2, 0x04 };
    elem2.Video.QueueDepth = 8;
    elem2.Video.CWMin = NetworkU16{ 31 };
    elem2.Video.CWMax = NetworkU16{ 63 };
    w_qos.Add(elem2);

    WTPQualityOfService elem3{ 3, 0x14 };
    elem3.BestEffort.QueueDepth = 6;
    elem3.BestEffort.CWMin = NetworkU16{ 7 };
    elem3.BestEffort.CWMax = NetworkU16{ 15 };
    w_qos.Add(elem3);

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_qos.Serialize(&raw_data);
    // Each element: 4 (header) + 1 (radio_id) + 1 (tagging_policy) + 32 (4 * QoS sub-elements) = 38 bytes
    // Total: 3 * 38 = 114
    CHECK_EQUAL(&buffer[0] + 114, raw_data.current);

    ReadableWTPQualityOfServiceArray r_qos;
    CHECK_FALSE(r_qos.IsPresent());

    raw_data = { buffer, buffer + 114 };

    CHECK_TRUE(r_qos.Deserialize(&raw_data));
    CHECK_TRUE(r_qos.Deserialize(&raw_data));
    CHECK_TRUE(r_qos.Deserialize(&raw_data));
    CHECK_FALSE(r_qos.Deserialize(&raw_data));
    CHECK_TRUE(r_qos.IsPresent());
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_qos.Get().size());

    CHECK_EQUAL(1, r_qos.Get()[0]->RadioID);
    CHECK_EQUAL(0x10, r_qos.Get()[0]->TaggingPolicy);
    CHECK_EQUAL(4, r_qos.Get()[0]->Voice.QueueDepth);
    CHECK_EQUAL(15, r_qos.Get()[0]->Voice.CWMin.Get());
    CHECK_EQUAL(31, r_qos.Get()[0]->Voice.CWMax.Get());

    CHECK_EQUAL(2, r_qos.Get()[1]->RadioID);
    CHECK_EQUAL(0x04, r_qos.Get()[1]->TaggingPolicy);
    CHECK_EQUAL(8, r_qos.Get()[1]->Video.QueueDepth);
    CHECK_EQUAL(31, r_qos.Get()[1]->Video.CWMin.Get());
    CHECK_EQUAL(63, r_qos.Get()[1]->Video.CWMax.Get());

    CHECK_EQUAL(3, r_qos.Get()[2]->RadioID);
    CHECK_EQUAL(0x14, r_qos.Get()[2]->TaggingPolicy);
    CHECK_EQUAL(6, r_qos.Get()[2]->BestEffort.QueueDepth);
    CHECK_EQUAL(7, r_qos.Get()[2]->BestEffort.CWMin.Get());
    CHECK_EQUAL(15, r_qos.Get()[2]->BestEffort.CWMax.Get());
}

TEST(WTPQualityOfServiceTestsGroup, Add_array_of_items_is_unique_by_radio_id) {
    uint8_t buffer[2048] = {};

    WritableWTPQualityOfServiceArray w_qos;

    // Add same RadioID multiple times - should replace
    w_qos.Add({ 1, 0x00 });
    w_qos.Add({ 1, 0x1F }); // All flags set

    w_qos.Add({ 2, 0x10 }); // P flag
    w_qos.Add({ 2, 0x08 }); // Q flag

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_qos.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableWTPQualityOfServiceArray r_qos;
    CHECK_FALSE(r_qos.IsPresent());

    CHECK_TRUE(r_qos.Deserialize(&raw_data));
    CHECK_TRUE(r_qos.IsPresent());
    CHECK_TRUE(r_qos.Deserialize(&raw_data));
    CHECK_FALSE(r_qos.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_qos.Get().size());

    // Should have the last values for each RadioID
    CHECK_EQUAL(1, r_qos.Get()[0]->RadioID);
    CHECK_EQUAL(0x1F, r_qos.Get()[0]->TaggingPolicy);

    CHECK_EQUAL(2, r_qos.Get()[1]->RadioID);
    CHECK_EQUAL(0x08, r_qos.Get()[1]->TaggingPolicy);
}
