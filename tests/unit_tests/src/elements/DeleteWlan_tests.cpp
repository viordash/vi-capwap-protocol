#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/DeleteWlan.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(DeleteWlanTestsGroup){ //
                                  TEST_SETUP(){}

                                  TEST_TEARDOWN(){}
};

TEST(DeleteWlanTestsGroup, Deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Message Element Header (4 bytes) ----
        0x04, 0x03,     // Type: 1027 (IEEE 802.11 Delete WLAN)
        0x00, 0x02,     // Length: 2 bytes

        // ---- Value (2 bytes) ----
        0x01,           // Radio ID: 1
        0x02            // WLAN ID: 2
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = DeleteWlan::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::DeleteWlan, element->GetElementType());

    CHECK_EQUAL(1, element->RadioID);
    CHECK_EQUAL(2, element->WlanID);
}

TEST(DeleteWlanTestsGroup, Serialize) {
    uint8_t buffer[256] = {};
    DeleteWlan element_0{ 5, 3 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 6, raw_data.current);
    const uint8_t reference[] = { 0x04, 0x03, 0x00, 0x02, 0x05, 0x03 };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = DeleteWlan::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(&buffer[0] + 6, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::DeleteWlan, element->GetElementType());
    CHECK_EQUAL(5, element->RadioID);
    CHECK_EQUAL(3, element->WlanID);
}

TEST(DeleteWlanTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableDeleteWlanArray w_wlans;

    w_wlans.Add({ 1, 1 });
    w_wlans.Add({ 1, 2 });
    w_wlans.Add({ 2, 1 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_wlans.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 18, raw_data.current);

    // clang-format off
    uint8_t reference[] = {
        // Element 0: RadioID=1, WlanID=1
        0x04, 0x03, 0x00, 0x02, 0x01, 0x01,
        // Element 1: RadioID=1, WlanID=2
        0x04, 0x03, 0x00, 0x02, 0x01, 0x02,
        // Element 2: RadioID=2, WlanID=1
        0x04, 0x03, 0x00, 0x02, 0x02, 0x01
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    ReadableDeleteWlanArray r_wlans;

    raw_data = { reference, reference + sizeof(reference) };

    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_wlans.Get().size());

    CHECK_EQUAL(1, r_wlans.Get()[0]->RadioID);
    CHECK_EQUAL(1, r_wlans.Get()[0]->WlanID);

    CHECK_EQUAL(1, r_wlans.Get()[1]->RadioID);
    CHECK_EQUAL(2, r_wlans.Get()[1]->WlanID);

    CHECK_EQUAL(2, r_wlans.Get()[2]->RadioID);
    CHECK_EQUAL(1, r_wlans.Get()[2]->WlanID);
}

TEST(DeleteWlanTestsGroup, Add_array_of_items_is_unique) {
    uint8_t buffer[2048] = {};

    WritableDeleteWlanArray w_wlans;

    // Add same RadioID+WlanID multiple times
    w_wlans.Add({ 1, 1 });
    w_wlans.Add({ 1, 1 });
    w_wlans.Add({ 1, 1 });
    w_wlans.Add({ 2, 2 });
    w_wlans.Add({ 2, 2 });
    w_wlans.Add({ 2, 2 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_wlans.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableDeleteWlanArray r_wlans;

    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_TRUE(r_wlans.Deserialize(&raw_data));
    CHECK_FALSE(r_wlans.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_wlans.Get().size());

    CHECK_EQUAL(1, r_wlans.Get()[0]->RadioID);
    CHECK_EQUAL(1, r_wlans.Get()[0]->WlanID);

    CHECK_EQUAL(2, r_wlans.Get()[1]->RadioID);
    CHECK_EQUAL(2, r_wlans.Get()[1]->WlanID);
}

TEST(DeleteWlanTestsGroup, Validate_RadioID_range) {
    // clang-format off
    // Valid RadioID = 0
    uint8_t data_valid_zero[] = {
        0x04, 0x03, 0x00, 0x02, 0x00, 0x01
    };
    // Valid RadioID = 1
    uint8_t data_valid_min[] = {
        0x04, 0x03, 0x00, 0x02, 0x01, 0x01
    };
    // Valid RadioID = 31
    uint8_t data_valid_max[] = {
        0x04, 0x03, 0x00, 0x02, 0x1F, 0x01
    };
    // Invalid RadioID = 32
    uint8_t data_invalid_high[] = {
        0x04, 0x03, 0x00, 0x02, 0x20, 0x01
    };
    // clang-format on

    RawData raw_data = { data_valid_zero, data_valid_zero + sizeof(data_valid_zero) };
    CHECK(DeleteWlan::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_min, data_valid_min + sizeof(data_valid_min) };
    CHECK(DeleteWlan::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_max, data_valid_max + sizeof(data_valid_max) };
    CHECK(DeleteWlan::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid_high, data_invalid_high + sizeof(data_invalid_high) };
    CHECK(DeleteWlan::Deserialize(&raw_data) == nullptr);
}

TEST(DeleteWlanTestsGroup, Validate_WlanID_range) {
    // clang-format off
    // Valid WlanID = 1
    uint8_t data_valid_min[] = {
        0x04, 0x03, 0x00, 0x02, 0x01, 0x01
    };
    // Valid WlanID = 16
    uint8_t data_valid_max[] = {
        0x04, 0x03, 0x00, 0x02, 0x01, 0x10
    };
    // Invalid WlanID = 0
    uint8_t data_invalid_zero[] = {
        0x04, 0x03, 0x00, 0x02, 0x01, 0x00
    };
    // Invalid WlanID = 17
    uint8_t data_invalid_high[] = {
        0x04, 0x03, 0x00, 0x02, 0x01, 0x11
    };
    // clang-format on

    RawData raw_data = { data_valid_min, data_valid_min + sizeof(data_valid_min) };
    CHECK(DeleteWlan::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_max, data_valid_max + sizeof(data_valid_max) };
    CHECK(DeleteWlan::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid_zero, data_invalid_zero + sizeof(data_invalid_zero) };
    CHECK(DeleteWlan::Deserialize(&raw_data) == nullptr);

    raw_data = { data_invalid_high, data_invalid_high + sizeof(data_invalid_high) };
    CHECK(DeleteWlan::Deserialize(&raw_data) == nullptr);
}
