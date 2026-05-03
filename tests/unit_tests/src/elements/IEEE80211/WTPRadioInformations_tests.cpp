#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/WTPRadioInformation.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(WTPRadioInformationTestsGroup){ //
                                           TEST_SETUP(){}

                                           TEST_TEARDOWN(){}
};

TEST(WTPRadioInformationTestsGroup, ToString) {
    WTPRadioInformation element{ 7, true, true, false, true, false, true, false };

    STRCMP_EQUAL(" B A N AX", element.ToString().c_str());
}

TEST(WTPRadioInformationTestsGroup, Serialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableWTPRadioInformationArray w_infos;

    w_infos.Add({ 0, false, false, false, false, false, false, false });
    w_infos.Add({ 1, true, true, false, false, false, false, false });
    w_infos.Add({ 2, false, false, false, false, false, false, false });
    w_infos.Add({ 3, true, true, false, true, false, false, false });
    w_infos.Add({ 4, false, true, false, false, false, false, false });
    w_infos.Add({ 5, true, true, false, false, false, false, false });
    w_infos.Add({ 6, false, false, false, false, false, false, false });
    w_infos.Add({ 7, true, true, false, false, false, false, false });
    w_infos.Add({ 8, false, true, false, false, false, false, false });
    w_infos.Add({ 9, false, true, true, false, false, false, false });
    w_infos.Add({ 10, false, false, false, false, false, false, false });
    w_infos.Add({ 11, false, true, false, false, false, false, false });
    w_infos.Add({ 12, false, true, false, false, false, false, false });
    w_infos.Add({ 13, true, true, false, false, false, false, false });
    w_infos.Add({ 14, false, false, false, false, false, false, false });
    w_infos.Add({ 15, true, true, false, false, false, false, false });
    w_infos.Add({ 16, false, true, false, true, false, false, false });
    w_infos.Add({ 17, true, false, false, true, false, false, false });
    w_infos.Add({ 18, false, false, false, false, false, false, false });
    w_infos.Add({ 19, true, true, false, false, false, false, false });
    w_infos.Add({ 20, false, true, true, true, false, false, false });
    w_infos.Add({ 21, true, true, false, false, false, false, false });
    w_infos.Add({ 22, false, false, false, false, false, false, false });
    w_infos.Add({ 23, true, true, false, false, false, false, false });
    w_infos.Add({ 24, false, true, true, false, false, false, false });
    w_infos.Add({ 25, true, true, false, false, false, false, false });
    w_infos.Add({ 26, false, false, false, true, false, false, false });
    w_infos.Add({ 127, true, true, false, false, false, false, false });
    w_infos.Add({ 28, false, true, true, false, false, false, false });
    w_infos.Add({ 29, true, true, true, false, false, false, false });
    w_infos.Add({ 30, false, false, false, false, false, false, false });
    w_infos.Add({ 31, true, true, false, true, false, false, false });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_infos.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 288, raw_data.current);
    const uint8_t reference[] = {
        0x04, 0x18, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x18, 0x00, 0x05, 0x01, 0x00,
        0x00, 0x00, 0x03, 0x04, 0x18, 0x00, 0x05, 0x02, 0x00, 0x00, 0x00, 0x00, 0x04, 0x18, 0x00,
        0x05, 0x03, 0x00, 0x00, 0x00, 0x0B, 0x04, 0x18, 0x00, 0x05, 0x04, 0x00, 0x00, 0x00, 0x02,
        0x04, 0x18, 0x00, 0x05, 0x05, 0x00, 0x00, 0x00, 0x03, 0x04, 0x18, 0x00, 0x05, 0x06, 0x00,
        0x00, 0x00, 0x00, 0x04, 0x18, 0x00, 0x05, 0x07, 0x00, 0x00, 0x00, 0x03, 0x04, 0x18, 0x00,
        0x05, 0x08, 0x00, 0x00, 0x00, 0x02, 0x04, 0x18, 0x00, 0x05, 0x09, 0x00, 0x00, 0x00, 0x06,
        0x04, 0x18, 0x00, 0x05, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x04, 0x18, 0x00, 0x05, 0x0B, 0x00,
        0x00, 0x00, 0x02, 0x04, 0x18, 0x00, 0x05, 0x0C, 0x00, 0x00, 0x00, 0x02, 0x04, 0x18, 0x00,
        0x05, 0x0D, 0x00, 0x00, 0x00, 0x03, 0x04, 0x18, 0x00, 0x05, 0x0E, 0x00, 0x00, 0x00, 0x00,
        0x04, 0x18, 0x00, 0x05, 0x0F, 0x00, 0x00, 0x00, 0x03, 0x04, 0x18, 0x00, 0x05, 0x10, 0x00,
        0x00, 0x00, 0x0A, 0x04, 0x18, 0x00, 0x05, 0x11, 0x00, 0x00, 0x00, 0x09, 0x04, 0x18, 0x00,
        0x05, 0x12, 0x00, 0x00, 0x00, 0x00, 0x04, 0x18, 0x00, 0x05, 0x13, 0x00, 0x00, 0x00, 0x03,
        0x04, 0x18, 0x00, 0x05, 0x14, 0x00, 0x00, 0x00, 0x0E, 0x04, 0x18, 0x00, 0x05, 0x15, 0x00,
        0x00, 0x00, 0x03, 0x04, 0x18, 0x00, 0x05, 0x16, 0x00, 0x00, 0x00, 0x00, 0x04, 0x18, 0x00,
        0x05, 0x17, 0x00, 0x00, 0x00, 0x03, 0x04, 0x18, 0x00, 0x05, 0x18, 0x00, 0x00, 0x00, 0x06,
        0x04, 0x18, 0x00, 0x05, 0x19, 0x00, 0x00, 0x00, 0x03, 0x04, 0x18, 0x00, 0x05, 0x1A, 0x00,
        0x00, 0x00, 0x08, 0x04, 0x18, 0x00, 0x05, 0x7F, 0x00, 0x00, 0x00, 0x03, 0x04, 0x18, 0x00,
        0x05, 0x1C, 0x00, 0x00, 0x00, 0x06, 0x04, 0x18, 0x00, 0x05, 0x1D, 0x00, 0x00, 0x00, 0x07,
        0x04, 0x18, 0x00, 0x05, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x04, 0x18, 0x00, 0x05, 0x1F, 0x00,
        0x00, 0x00, 0x0B
    };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));
}

TEST(WTPRadioInformationTestsGroup, Serialize_Deserialize_array) {
    uint8_t buffer[2048] = {};

    WritableWTPRadioInformationArray w_infos;
    w_infos.Add({ 1, true, false, true, true, false, false, false }); // b/g/n
    w_infos.Add({ 2, false, true, false, true, true, false, false }); // a/n/ac
    w_infos.Add({ 3, false, true, false, true, true, true, false });  // a/n/ac/ax

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    w_infos.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 27, raw_data.current); // 3 × 9 = 27

    ReadableWTPRadioInformationArray r_infos;
    raw_data = { buffer, buffer + 27 };

    CHECK_TRUE(r_infos.Deserialize(&raw_data));
    CHECK_TRUE(r_infos.Deserialize(&raw_data));
    CHECK_TRUE(r_infos.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_infos.Get().size());

    // Check first element (b/g/n)
    CHECK_EQUAL(1, r_infos.Get()[0]->RadioID);
    CHECK_TRUE(r_infos.Get()[0]->B);
    CHECK_FALSE(r_infos.Get()[0]->A);
    CHECK_TRUE(r_infos.Get()[0]->G);
    CHECK_TRUE(r_infos.Get()[0]->N);
    CHECK_FALSE(r_infos.Get()[0]->AC);

    // Check second element (a/n/ac)
    CHECK_EQUAL(2, r_infos.Get()[1]->RadioID);
    CHECK_FALSE(r_infos.Get()[1]->B);
    CHECK_TRUE(r_infos.Get()[1]->A);
    CHECK_FALSE(r_infos.Get()[1]->G);
    CHECK_TRUE(r_infos.Get()[1]->N);
    CHECK_TRUE(r_infos.Get()[1]->AC);

    // Check third element (a/n/ac/ax)
    CHECK_EQUAL(3, r_infos.Get()[2]->RadioID);
    CHECK_FALSE(r_infos.Get()[2]->B);
    CHECK_TRUE(r_infos.Get()[2]->A);
    CHECK_TRUE(r_infos.Get()[2]->N);
    CHECK_TRUE(r_infos.Get()[2]->AC);
    CHECK_TRUE(r_infos.Get()[2]->AX);
    CHECK_FALSE(r_infos.Get()[2]->BE);
}