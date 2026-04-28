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

TEST(WTPRadioInformationTestsGroup, Validate_RadioID_range) {
    // clang-format off
    // Valid RadioID = 0
    uint8_t data_valid_zero[] = {
        0x04, 0x18, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x01
    };
    // Valid RadioID = 31
    uint8_t data_valid_max[] = {
        0x04, 0x18, 0x00, 0x05, 0x1F, 0x00, 0x00, 0x00, 0x01
    };
    // Invalid RadioID = 32
    uint8_t data_invalid_high[] = {
        0x04, 0x18, 0x00, 0x05, 0x20, 0x00, 0x00, 0x00, 0x01
    };
    // clang-format on

    RawData raw_data = { data_valid_zero, data_valid_zero + sizeof(data_valid_zero) };
    CHECK(WTPRadioInformation::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_max, data_valid_max + sizeof(data_valid_max) };
    CHECK(WTPRadioInformation::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid_high, data_invalid_high + sizeof(data_invalid_high) };
    CHECK(WTPRadioInformation::Deserialize(&raw_data) == nullptr);
}

TEST(WTPRadioInformationTestsGroup, Deserialize_insufficient_data) {
    // clang-format off
    uint8_t data[] = {
        0x04, 0x18, 0x00, 0x05, 0x01, 0x00, 0x00, 0x00  // Missing last byte
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    CHECK(WTPRadioInformation::Deserialize(&raw_data) == nullptr);
}

TEST(WTPRadioInformationTestsGroup, WiFi_standards_combinations) {
    uint8_t buffer[256] = {};

    // Test 802.11b only (2.4 GHz)
    WTPRadioInformation bgn_radio{ 1, true, false, false, false, false, false, false };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    bgn_radio.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    auto elem = WTPRadioInformation::Deserialize(&raw_data);
    CHECK(elem != nullptr);
    CHECK_TRUE(elem->B);
    CHECK_FALSE(elem->A);
    STRCMP_EQUAL(" B", elem->ToString().c_str());

    // Test 802.11a only (5 GHz)
    WTPRadioInformation a_radio{ 2, false, true, false, false, false, false, false };
    raw_data = { buffer, buffer + sizeof(buffer) };
    a_radio.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    elem = WTPRadioInformation::Deserialize(&raw_data);
    CHECK(elem != nullptr);
    CHECK_FALSE(elem->B);
    CHECK_TRUE(elem->A);
    STRCMP_EQUAL(" A", elem->ToString().c_str());

    // Test 802.11b/g/n (2.4 GHz)
    WTPRadioInformation bgn_24ghz{ 3, true, false, true, true, false, false, false };
    raw_data = { buffer, buffer + sizeof(buffer) };
    bgn_24ghz.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    elem = WTPRadioInformation::Deserialize(&raw_data);
    CHECK(elem != nullptr);
    CHECK_TRUE(elem->B);
    CHECK_FALSE(elem->A);
    CHECK_TRUE(elem->G);
    CHECK_TRUE(elem->N);
    STRCMP_EQUAL(" B G N", elem->ToString().c_str());

    // Test 802.11a/n/ac (5 GHz)
    WTPRadioInformation anac_5ghz{ 4, false, true, false, true, true, false, false };
    raw_data = { buffer, buffer + sizeof(buffer) };
    anac_5ghz.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    elem = WTPRadioInformation::Deserialize(&raw_data);
    CHECK(elem != nullptr);
    CHECK_FALSE(elem->B);
    CHECK_TRUE(elem->A);
    CHECK_FALSE(elem->G);
    CHECK_TRUE(elem->N);
    CHECK_TRUE(elem->AC);
    STRCMP_EQUAL(" A N AC", elem->ToString().c_str());

    // Test 802.11ax (WiFi 6)
    WTPRadioInformation ax_radio{ 5, false, true, false, true, false, true, false };
    raw_data = { buffer, buffer + sizeof(buffer) };
    ax_radio.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    elem = WTPRadioInformation::Deserialize(&raw_data);
    CHECK(elem != nullptr);
    CHECK_TRUE(elem->AX);
    STRCMP_EQUAL(" A N AX", elem->ToString().c_str());

    // Test 802.11be (WiFi 7)
    WTPRadioInformation be_radio{ 6, false, true, false, true, true, true, true };
    raw_data = { buffer, buffer + sizeof(buffer) };
    be_radio.Serialize(&raw_data);
    raw_data = { buffer, buffer + sizeof(buffer) };
    elem = WTPRadioInformation::Deserialize(&raw_data);
    CHECK(elem != nullptr);
    CHECK_TRUE(elem->BE);
    STRCMP_EQUAL(" A N AC AX BE", elem->ToString().c_str());
}

TEST(WTPRadioInformationTestsGroup, Serialize_Deserialize_array) {
    uint8_t buffer[2048] = {};

    WritableWTPRadioInformationArray w_infos;
    w_infos.Add({ 1, true, false, true, true, false, false, false });  // b/g/n
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

TEST(WTPRadioInformationTestsGroup, No_standards_enabled) {
    uint8_t buffer[256] = {};

    // Radio with no WiFi standards enabled (all false)
    WTPRadioInformation no_std_radio{ 10, false, false, false, false, false, false, false };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    no_std_radio.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 9, raw_data.current);

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto elem = WTPRadioInformation::Deserialize(&raw_data);
    CHECK(elem != nullptr);
    CHECK_EQUAL(10, elem->RadioID);
    CHECK_FALSE(elem->B);
    CHECK_FALSE(elem->A);
    CHECK_FALSE(elem->G);
    CHECK_FALSE(elem->N);
    CHECK_FALSE(elem->AC);
    CHECK_FALSE(elem->AX);
    CHECK_FALSE(elem->BE);
    STRCMP_EQUAL("", elem->ToString().c_str());
}

TEST(WTPRadioInformationTestsGroup, All_standards_enabled) {
    uint8_t buffer[256] = {};

    // Radio with all WiFi standards enabled
    WTPRadioInformation all_std_radio{ 15, true, true, true, true, true, true, true };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    all_std_radio.Serialize(&raw_data);

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto elem = WTPRadioInformation::Deserialize(&raw_data);
    CHECK(elem != nullptr);
    CHECK_EQUAL(15, elem->RadioID);
    CHECK_TRUE(elem->B);
    CHECK_TRUE(elem->A);
    CHECK_TRUE(elem->G);
    CHECK_TRUE(elem->N);
    CHECK_TRUE(elem->AC);
    CHECK_TRUE(elem->AX);
    CHECK_TRUE(elem->BE);
    STRCMP_EQUAL(" B A G N AC AX BE", elem->ToString().c_str());
}