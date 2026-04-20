#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/WTPRadioInformation.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(WTPRadioInformationTestsGroup){ //
                                           TEST_SETUP(){}

                                           TEST_TEARDOWN(){}
};

TEST(WTPRadioInformationTestsGroup, Deserialize) {
    uint8_t data[] = {
        // WTP Radio Information (802.11) - Пример 1 (2.4 GHz b/g/n)
        0x04,
        0x18, // Type = 1048 (в Big Endian)
        0x00,
        0x05, // Length = 5 (в Big Endian)

        // --- Value (5 байт) ---
        0x01, // Radio ID: 1
        0x00,
        0x00,
        0x00, // Radio Type (первые 3 байта зарезервированы/не используются)
        0x0D  // Radio Type (последний байт, битовая маска для b, g, n)
    };
    RawData raw_data{ data, data + sizeof(data) };
    auto element = WTPRadioInformation::Deserialize(&raw_data);

    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::WTPRadioInformation, element->GetElementType());
    CHECK_EQUAL(1, element->RadioID);
    CHECK_TRUE(element->B);
    CHECK_FALSE(element->A);
    CHECK_TRUE(element->G);
    CHECK_TRUE(element->N);
}

TEST(WTPRadioInformationTestsGroup, Serialize) {
    uint8_t buffer[256] = {};
    WTPRadioInformation element_0{ 31, true, true, false, false, false, true, false };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 9, raw_data.current);
    const uint8_t reference[] = {
        0x04, 0x18, 0x00, 0x05, 0x1F, 0x00, 0x00, 0x00, 0x23,
    };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = WTPRadioInformation::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(&buffer[0] + 9, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::WTPRadioInformation, element->GetElementType());
    CHECK_EQUAL(31, element->RadioID);
    CHECK_TRUE(element->B);
    CHECK_TRUE(element->A);
    CHECK_FALSE(element->G);
    CHECK_FALSE(element->N);
    CHECK_FALSE(element->AC);
    CHECK_TRUE(element->AX);
    CHECK_FALSE(element->BE);
}

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