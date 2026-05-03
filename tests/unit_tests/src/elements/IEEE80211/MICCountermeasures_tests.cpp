#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/MICCountermeasures.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(MICCountermeasuresTestsGroup){ //
                                          TEST_SETUP(){}

                                          TEST_TEARDOWN(){}
};

TEST(MICCountermeasuresTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableMICCountermeasuresArray w_cms;

    const uint8_t mac_0[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 };
    const uint8_t mac_1[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    const uint8_t mac_2[] = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC };

    w_cms.Add({ 1, 1, mac_0 });
    w_cms.Add({ 1, 2, mac_1 });
    w_cms.Add({ 2, 1, mac_2 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_cms.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 36, raw_data.current);

    // clang-format off
    uint8_t reference[] = {
        // Element 0: RadioID=1, WlanID=1
        0x04, 0x07, 0x00, 0x08, 0x01, 0x01, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
        // Element 1: RadioID=1, WlanID=2
        0x04, 0x07, 0x00, 0x08, 0x01, 0x02, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
        // Element 2: RadioID=2, WlanID=1
        0x04, 0x07, 0x00, 0x08, 0x02, 0x01, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    ReadableMICCountermeasuresArray r_cms;

    raw_data = { reference, reference + sizeof(reference) };

    CHECK_TRUE(r_cms.Deserialize(&raw_data));
    CHECK_TRUE(r_cms.Deserialize(&raw_data));
    CHECK_TRUE(r_cms.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_cms.Get().size());

    CHECK_EQUAL(1, r_cms.Get()[0]->RadioID);
    CHECK_EQUAL(1, r_cms.Get()[0]->WlanID);
    MEMCMP_EQUAL(mac_0, r_cms.Get()[0]->MACAddress, sizeof(mac_0));

    CHECK_EQUAL(1, r_cms.Get()[1]->RadioID);
    CHECK_EQUAL(2, r_cms.Get()[1]->WlanID);
    MEMCMP_EQUAL(mac_1, r_cms.Get()[1]->MACAddress, sizeof(mac_1));

    CHECK_EQUAL(2, r_cms.Get()[2]->RadioID);
    CHECK_EQUAL(1, r_cms.Get()[2]->WlanID);
    MEMCMP_EQUAL(mac_2, r_cms.Get()[2]->MACAddress, sizeof(mac_2));
}
