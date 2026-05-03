#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/InformationElement.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(InformationElementTestsGroup){ //
                                          TEST_SETUP(){}

                                          TEST_TEARDOWN(){}
};

TEST(InformationElementTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableInformationElementArray w_ies;

    std::vector<uint8_t> ie1 = { 0x00, 0x02, 0x41, 0x42 };             // SSID: "AB"
    std::vector<uint8_t> ie2 = { 0x01, 0x04, 0x82, 0x84, 0x8B, 0x96 }; // Supported Rates
    std::vector<uint8_t> ie3 = { 0x03, 0x01, 0x06 }; // DS Parameter Set: Channel 6

    w_ies.Add({ 1, 1, 0xC0, ie1 }); // B=1, P=1
    w_ies.Add({ 1, 2, 0x80, ie2 }); // B=1, P=0
    w_ies.Add({ 2, 1, 0x40, ie3 }); // B=0, P=1

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_ies.Serialize(&raw_data);
    // Element sizes: (7+4)=11, (7+6)=13, (7+3)=10 = 34 total
    CHECK_EQUAL(&buffer[0] + 34, raw_data.current);

    ReadableInformationElementArray r_ies;
    CHECK_FALSE(r_ies.IsPresent());

    raw_data = { buffer, buffer + 34 };

    CHECK_TRUE(r_ies.Deserialize(&raw_data));
    CHECK_TRUE(r_ies.IsPresent());
    CHECK_TRUE(r_ies.Deserialize(&raw_data));
    CHECK_TRUE(r_ies.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_ies.Get().size());

    CHECK_EQUAL(1, r_ies.Get()[0]->GetRadioID());
    CHECK_EQUAL(1, r_ies.Get()[0]->GetWlanID());
    CHECK_EQUAL(0xC0, r_ies.Get()[0]->GetFlags());
    CHECK_EQUAL(4, r_ies.Get()[0]->GetIELength());

    CHECK_EQUAL(1, r_ies.Get()[1]->GetRadioID());
    CHECK_EQUAL(2, r_ies.Get()[1]->GetWlanID());
    CHECK_EQUAL(0x80, r_ies.Get()[1]->GetFlags());
    CHECK_EQUAL(6, r_ies.Get()[1]->GetIELength());

    CHECK_EQUAL(2, r_ies.Get()[2]->GetRadioID());
    CHECK_EQUAL(1, r_ies.Get()[2]->GetWlanID());
    CHECK_EQUAL(0x40, r_ies.Get()[2]->GetFlags());
    CHECK_EQUAL(3, r_ies.Get()[2]->GetIELength());
}
