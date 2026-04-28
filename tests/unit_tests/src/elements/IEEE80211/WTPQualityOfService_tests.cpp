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

TEST(WTPQualityOfServiceTestsGroup, Deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Message Element Header (4 bytes) ----
        0x04, 0x15,     // Type: 1045 (IEEE 802.11 WTP Quality of Service)
        0x00, 0x22,     // Length: 34 bytes

        // ---- Value (34 bytes) ----
        0x01,           // Radio ID: 1
        0x00,           // Tagging Policy: 802.1p (0)

        // Voice QoS Sub-Element (8 bytes)
        0x04,           // Queue Depth: 4
        0x00, 0x0F,     // CWMin: 15
        0x00, 0x3F,     // CWMax: 63
        0x02,           // AIFS: 2
        0x05, 0x2A,     // TagFields: 8021p=5, DSCP=42

        // Video QoS Sub-Element (8 bytes)
        0x08,           // Queue Depth: 8
        0x00, 0x1F,     // CWMin: 31
        0x00, 0x7F,     // CWMax: 127
        0x03,           // AIFS: 3
        0x03, 0x15,     // TagFields: 8021p=3, DSCP=21

        // Best Effort QoS Sub-Element (8 bytes)
        0x10,           // Queue Depth: 16
        0x00, 0x3F,     // CWMin: 63
        0x00, 0xFF,     // CWMax: 255
        0x04,           // AIFS: 4
        0x00, 0x00,     // TagFields: 8021p=0, DSCP=0

        // Background QoS Sub-Element (8 bytes)
        0x20,           // Queue Depth: 32
        0x00, 0x7F,     // CWMin: 127
        0x01, 0xFF,     // CWMax: 511
        0x07,           // AIFS: 7
        0x07, 0x3F      // TagFields: 8021p=7, DSCP=63
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = WTPQualityOfService::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::WTPQualityOfService, element->GetElementType());

    CHECK_EQUAL(1, element->RadioID);
    CHECK_EQUAL(0x00, element->TaggingPolicy);

    // Voice
    CHECK_EQUAL(4, element->Voice.QueueDepth);
    CHECK_EQUAL(15, element->Voice.CWMin.Get());
    CHECK_EQUAL(63, element->Voice.CWMax.Get());
    CHECK_EQUAL(2, element->Voice.AIFS);
    CHECK_EQUAL(5, element->Voice.Get8021pTag());
    CHECK_EQUAL(42, element->Voice.GetDscpTag());

    // Video
    CHECK_EQUAL(8, element->Video.QueueDepth);
    CHECK_EQUAL(31, element->Video.CWMin.Get());
    CHECK_EQUAL(127, element->Video.CWMax.Get());
    CHECK_EQUAL(3, element->Video.AIFS);
    CHECK_EQUAL(3, element->Video.Get8021pTag());
    CHECK_EQUAL(21, element->Video.GetDscpTag());

    // Best Effort
    CHECK_EQUAL(16, element->BestEffort.QueueDepth);
    CHECK_EQUAL(63, element->BestEffort.CWMin.Get());
    CHECK_EQUAL(255, element->BestEffort.CWMax.Get());
    CHECK_EQUAL(4, element->BestEffort.AIFS);

    // Background
    CHECK_EQUAL(32, element->Background.QueueDepth);
    CHECK_EQUAL(127, element->Background.CWMin.Get());
    CHECK_EQUAL(511, element->Background.CWMax.Get());
    CHECK_EQUAL(7, element->Background.AIFS);
    CHECK_EQUAL(7, element->Background.Get8021pTag());
    CHECK_EQUAL(63, element->Background.GetDscpTag());
}

TEST(WTPQualityOfServiceTestsGroup, Serialize) {
    uint8_t buffer[256] = {};
    // TaggingPolicy = 0x04 (D bit set - DSCP enabled)
    WTPQualityOfService element{ 5, 0x04 };

    element.Voice.QueueDepth = 4;
    element.Voice.CWMin = NetworkU16{ 15 };
    element.Voice.CWMax = NetworkU16{ 31 };
    element.Voice.AIFS = 2;
    element.Voice.Set8021pTag(5);
    element.Voice.SetDscpTag(42);

    element.Video.QueueDepth = 8;
    element.Video.CWMin = NetworkU16{ 31 };
    element.Video.CWMax = NetworkU16{ 63 };
    element.Video.AIFS = 3;

    element.BestEffort.QueueDepth = 16;
    element.BestEffort.CWMin = NetworkU16{ 63 };
    element.BestEffort.CWMax = NetworkU16{ 127 };
    element.BestEffort.AIFS = 4;

    element.Background.QueueDepth = 32;
    element.Background.CWMin = NetworkU16{ 127 };
    element.Background.CWMax = NetworkU16{ 255 };
    element.Background.AIFS = 7;

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 38, raw_data.current);

    // Verify header
    CHECK_EQUAL(0x04, buffer[0]);
    CHECK_EQUAL(0x15, buffer[1]);
    CHECK_EQUAL(0x00, buffer[2]);
    CHECK_EQUAL(0x22, buffer[3]); // Length: 34

    // Verify RadioID and TaggingPolicy
    CHECK_EQUAL(5, buffer[4]);
    CHECK_EQUAL(0x04, buffer[5]); // D bit set (DSCP enabled)

    // Verify Voice QoS
    CHECK_EQUAL(4, buffer[6]);     // Queue Depth
    CHECK_EQUAL(0x00, buffer[7]);  // CWMin high byte
    CHECK_EQUAL(0x0F, buffer[8]);  // CWMin low byte (15)
    CHECK_EQUAL(0x00, buffer[9]);  // CWMax high byte
    CHECK_EQUAL(0x1F, buffer[10]); // CWMax low byte (31)
    CHECK_EQUAL(2, buffer[11]);    // AIFS

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto deserialized = WTPQualityOfService::Deserialize(&raw_data);
    CHECK(deserialized != nullptr);
    CHECK_EQUAL(&buffer[0] + 38, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::WTPQualityOfService, deserialized->GetElementType());
    CHECK_EQUAL(5, deserialized->RadioID);
    CHECK_EQUAL(0x04, deserialized->TaggingPolicy);
    CHECK_TRUE(deserialized->GetD()); // DSCP enabled
    CHECK_EQUAL(15, deserialized->Voice.CWMin.Get());
    CHECK_EQUAL(31, deserialized->Voice.CWMax.Get());
    CHECK_EQUAL(5, deserialized->Voice.Get8021pTag());
    CHECK_EQUAL(42, deserialized->Voice.GetDscpTag());
}

TEST(WTPQualityOfServiceTestsGroup, Validate_RadioID_range) {
    // clang-format off
    // Valid RadioID = 0
    uint8_t data_valid_zero[38] = {
        0x04, 0x15, 0x00, 0x22, 0x00, 0x00
        // Rest zeros for QoS sub-elements
    };
    // Valid RadioID = 31
    uint8_t data_valid_max[38] = {
        0x04, 0x15, 0x00, 0x22, 0x1F, 0x00
    };
    // Invalid RadioID = 32
    uint8_t data_invalid_high[38] = {
        0x04, 0x15, 0x00, 0x22, 0x20, 0x00
    };
    // clang-format on

    RawData raw_data = { data_valid_zero, data_valid_zero + sizeof(data_valid_zero) };
    CHECK(WTPQualityOfService::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_max, data_valid_max + sizeof(data_valid_max) };
    CHECK(WTPQualityOfService::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid_high, data_invalid_high + sizeof(data_invalid_high) };
    CHECK(WTPQualityOfService::Deserialize(&raw_data) == nullptr);
}

TEST(WTPQualityOfServiceTestsGroup, Validate_TaggingPolicy_reserved_bits) {
    // clang-format off
    // Valid: all bits zero
    uint8_t data_all_zero[38] = {
        0x04, 0x15, 0x00, 0x22, 0x01, 0x00
    };
    // Valid: all policy bits set (P|Q|D|O|I = 0x1F)
    uint8_t data_all_policy_bits[38] = {
        0x04, 0x15, 0x00, 0x22, 0x01, 0x1F
    };
    // Invalid: reserved bit 5 set (0x20)
    uint8_t data_reserved_bit5[38] = {
        0x04, 0x15, 0x00, 0x22, 0x01, 0x20
    };
    // Invalid: reserved bit 6 set (0x40)
    uint8_t data_reserved_bit6[38] = {
        0x04, 0x15, 0x00, 0x22, 0x01, 0x40
    };
    // Invalid: reserved bit 7 set (0x80)
    uint8_t data_reserved_bit7[38] = {
        0x04, 0x15, 0x00, 0x22, 0x01, 0x80
    };
    // clang-format on

    RawData raw_data = { data_all_zero, data_all_zero + sizeof(data_all_zero) };
    CHECK(WTPQualityOfService::Deserialize(&raw_data) != nullptr);

    raw_data = { data_all_policy_bits, data_all_policy_bits + sizeof(data_all_policy_bits) };
    CHECK(WTPQualityOfService::Deserialize(&raw_data) != nullptr);

    raw_data = { data_reserved_bit5, data_reserved_bit5 + sizeof(data_reserved_bit5) };
    CHECK(WTPQualityOfService::Deserialize(&raw_data) == nullptr);

    raw_data = { data_reserved_bit6, data_reserved_bit6 + sizeof(data_reserved_bit6) };
    CHECK(WTPQualityOfService::Deserialize(&raw_data) == nullptr);

    raw_data = { data_reserved_bit7, data_reserved_bit7 + sizeof(data_reserved_bit7) };
    CHECK(WTPQualityOfService::Deserialize(&raw_data) == nullptr);
}

TEST(WTPQualityOfServiceTestsGroup, Deserialize_wrong_type) {
    // clang-format off
    uint8_t data[38] = {
        0x04, 0x14,     // Wrong Type: 1044 (not WTPQualityOfService)
        0x00, 0x22,
        0x01, 0x00
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    CHECK(WTPQualityOfService::Deserialize(&raw_data) == nullptr);
}

TEST(WTPQualityOfServiceTestsGroup, Deserialize_wrong_length) {
    // clang-format off
    uint8_t data[40] = {
        0x04, 0x15,
        0x00, 0x24,     // Wrong Length: 36 (should be 34)
        0x01, 0x00
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    CHECK(WTPQualityOfService::Deserialize(&raw_data) == nullptr);
}

TEST(WTPQualityOfServiceTestsGroup, Deserialize_insufficient_data) {
    // clang-format off
    uint8_t data[] = {
        0x04, 0x15, 0x00, 0x22, 0x01, 0x00, 0x04, 0x00  // Missing most of the data
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    CHECK(WTPQualityOfService::Deserialize(&raw_data) == nullptr);
}

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

    raw_data = { buffer, buffer + 114 };

    CHECK_TRUE(r_qos.Deserialize(&raw_data));
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
