#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/UpdateStationQoS.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(UpdateStationQoSTestsGroup){ //
                                        TEST_SETUP(){}

                                        TEST_TEARDOWN(){}
};

TEST(UpdateStationQoSTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableUpdateStationQoSArray w_qos;

    const uint8_t mac_0[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 };
    const uint8_t mac_1[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    const uint8_t mac_2[] = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC };

    w_qos.Add({ 1, mac_0, 1, 10 });
    w_qos.Add({ 1, mac_1, 2, 20 });
    w_qos.Add({ 2, mac_2, 7, 63 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_qos.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 39, raw_data.current); // 3 * 13 = 39

    ReadableUpdateStationQoSArray r_qos;
    CHECK_FALSE(r_qos.IsPresent());

    raw_data = { buffer, buffer + 39 };

    CHECK_TRUE(r_qos.Deserialize(&raw_data));
    CHECK_TRUE(r_qos.IsPresent());
    CHECK_TRUE(r_qos.Deserialize(&raw_data));
    CHECK_TRUE(r_qos.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_qos.Get().size());

    CHECK_EQUAL(1, r_qos.Get()[0]->RadioID);
    MEMCMP_EQUAL(mac_0, r_qos.Get()[0]->MACAddress, sizeof(mac_0));
    CHECK_EQUAL(1, r_qos.Get()[0]->Get8021pTag());
    CHECK_EQUAL(10, r_qos.Get()[0]->GetDscpTag());

    CHECK_EQUAL(1, r_qos.Get()[1]->RadioID);
    MEMCMP_EQUAL(mac_1, r_qos.Get()[1]->MACAddress, sizeof(mac_1));
    CHECK_EQUAL(2, r_qos.Get()[1]->Get8021pTag());
    CHECK_EQUAL(20, r_qos.Get()[1]->GetDscpTag());

    CHECK_EQUAL(2, r_qos.Get()[2]->RadioID);
    MEMCMP_EQUAL(mac_2, r_qos.Get()[2]->MACAddress, sizeof(mac_2));
    CHECK_EQUAL(7, r_qos.Get()[2]->Get8021pTag());
    CHECK_EQUAL(63, r_qos.Get()[2]->GetDscpTag());
}

TEST(UpdateStationQoSTestsGroup, QoS_sub_element_parsing) {
    const uint8_t mac[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    // 802.1p = 0, DSCP = 0
    UpdateStationQoS elem0{ 1, mac, 0, 0 };
    CHECK_EQUAL(0, elem0.Get8021pTag());
    CHECK_EQUAL(0, elem0.GetDscpTag());

    // 802.1p = 0, DSCP = 63 (max)
    UpdateStationQoS elem1{ 1, mac, 0, 63 };
    CHECK_EQUAL(0, elem1.Get8021pTag());
    CHECK_EQUAL(63, elem1.GetDscpTag());

    // 802.1p = 7 (max), DSCP = 0
    UpdateStationQoS elem2{ 1, mac, 7, 0 };
    CHECK_EQUAL(7, elem2.Get8021pTag());
    CHECK_EQUAL(0, elem2.GetDscpTag());

    // 802.1p = 7, DSCP = 63
    UpdateStationQoS elem3{ 1, mac, 7, 63 };
    CHECK_EQUAL(7, elem3.Get8021pTag());
    CHECK_EQUAL(63, elem3.GetDscpTag());

    // 802.1p = 5, DSCP = 42
    UpdateStationQoS elem4{ 1, mac, 5, 42 };
    CHECK_EQUAL(5, elem4.Get8021pTag());
    CHECK_EQUAL(42, elem4.GetDscpTag());
}

TEST(UpdateStationQoSTestsGroup, QoS_sub_element_setters) {
    const uint8_t mac[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    UpdateStationQoS elem{ 1, mac, 0, 0 };
    CHECK_EQUAL(0, elem.Get8021pTag());
    CHECK_EQUAL(0, elem.GetDscpTag());

    elem.Set8021pTag(5);
    CHECK_EQUAL(5, elem.Get8021pTag());
    CHECK_EQUAL(0, elem.GetDscpTag());

    elem.SetDscpTag(30);
    CHECK_EQUAL(5, elem.Get8021pTag());
    CHECK_EQUAL(30, elem.GetDscpTag());

    elem.Set8021pTag(7);
    elem.SetDscpTag(63);
    CHECK_EQUAL(7, elem.Get8021pTag());
    CHECK_EQUAL(63, elem.GetDscpTag());
}
