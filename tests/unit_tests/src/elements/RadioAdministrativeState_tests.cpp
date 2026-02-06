#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ClearHeader.h"
#include "ControlHeader.h"
#include "elements/RadioAdministrativeState.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(RadioAdministrativeStateTestsGroup){ //
                                                TEST_SETUP(){}

                                                TEST_TEARDOWN(){}
};

TEST(RadioAdministrativeStateTestsGroup, Deserialize) {
    // Юнит-тест: CAPWAP Control IPv4 Address - Пример 1 (один интерфейс)
    // Тип элемента = 10, Длина = 6
    uint8_t data[] = {
        0x00, 0x1F, // Type: 31 (Radio Administrative State)
        0x00, 0x02, // Length: 2 байта
        0x01,       // Value -> Radio ID: 1
        0x01        // Value -> Admin State: 1 (Enabled)
    };
    RawData raw_data{ data, data + sizeof(data) };
    auto element = RadioAdministrativeState::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::RadioAdministrativeState, element->GetElementType());

    CHECK_EQUAL(1, element->RadioID);
    CHECK_EQUAL(RadioAdministrativeState::States::Enabled, element->AdminState);
}

TEST(RadioAdministrativeStateTestsGroup, Serialize) {
    uint8_t buffer[256] = {};
    RadioAdministrativeState element_0{ 3, RadioAdministrativeState::States::Disabled };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 6, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x1F, 0x00, 0x02, 0x03, 0x02 };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = RadioAdministrativeState::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(&buffer[0] + 6, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::RadioAdministrativeState, element->GetElementType());
    CHECK_EQUAL(3, element->RadioID);
    CHECK_EQUAL(RadioAdministrativeState::States::Disabled, element->AdminState);
}

TEST(RadioAdministrativeStateTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableRadioAdministrativeStateArray w_states;

    w_states.Add({ 0, RadioAdministrativeState::States::Enabled });
    w_states.Add({ 1, RadioAdministrativeState::States::Disabled });
    w_states.Add({ 2, RadioAdministrativeState::States::Reserved });
    w_states.Add({ 3, RadioAdministrativeState::States::Enabled });
    w_states.Add({ 4, RadioAdministrativeState::States::Disabled });
    w_states.Add({ 5, RadioAdministrativeState::States::Reserved });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_states.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 36, raw_data.current);
    uint8_t reference[] = {
        0x00, 0x1F, 0x00, 0x02, 0x00, 0x01, 0x00, 0x1F, 0x00, 0x02, 0x01, 0x02,
        0x00, 0x1F, 0x00, 0x02, 0x02, 0x00, 0x00, 0x1F, 0x00, 0x02, 0x03, 0x01,
        0x00, 0x1F, 0x00, 0x02, 0x04, 0x02, 0x00, 0x1F, 0x00, 0x02, 0x05, 0x00
    };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    ReadableRadioAdministrativeStateArray r_states;

    raw_data = { reference, reference + sizeof(reference) };

    CHECK_TRUE(r_states.Deserialize(&raw_data));
    CHECK_TRUE(r_states.Deserialize(&raw_data));
    CHECK_TRUE(r_states.Deserialize(&raw_data));
    CHECK_TRUE(r_states.Deserialize(&raw_data));
    CHECK_TRUE(r_states.Deserialize(&raw_data));
    CHECK_TRUE(r_states.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(6, r_states.Get().size());
    CHECK_EQUAL(0, r_states.Get()[0]->RadioID);
    CHECK_EQUAL(RadioAdministrativeState::States::Enabled, r_states.Get()[0]->AdminState);
    CHECK_EQUAL(1, r_states.Get()[1]->RadioID);
    CHECK_EQUAL(RadioAdministrativeState::States::Disabled, r_states.Get()[1]->AdminState);
    CHECK_EQUAL(2, r_states.Get()[2]->RadioID);
    CHECK_EQUAL(RadioAdministrativeState::States::Reserved, r_states.Get()[2]->AdminState);
    CHECK_EQUAL(3, r_states.Get()[3]->RadioID);
    CHECK_EQUAL(RadioAdministrativeState::States::Enabled, r_states.Get()[3]->AdminState);
    CHECK_EQUAL(4, r_states.Get()[4]->RadioID);
    CHECK_EQUAL(RadioAdministrativeState::States::Disabled, r_states.Get()[4]->AdminState);
    CHECK_EQUAL(5, r_states.Get()[5]->RadioID);
    CHECK_EQUAL(RadioAdministrativeState::States::Reserved, r_states.Get()[5]->AdminState);
}

TEST(RadioAdministrativeStateTestsGroup, Add_array_of_items_is_unique) {
    uint8_t buffer[2048] = {};

    WritableRadioAdministrativeStateArray w_states;

    w_states.Add({ 0, RadioAdministrativeState::States::Enabled });
    w_states.Add({ 0, RadioAdministrativeState::States::Enabled });
    w_states.Add({ 0, RadioAdministrativeState::States::Enabled });
    w_states.Add({ 1, RadioAdministrativeState::States::Disabled });
    w_states.Add({ 1, RadioAdministrativeState::States::Disabled });
    w_states.Add({ 1, RadioAdministrativeState::States::Disabled });
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_states.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableRadioAdministrativeStateArray r_states;

    CHECK_TRUE(r_states.Deserialize(&raw_data));
    CHECK_TRUE(r_states.Deserialize(&raw_data));
    CHECK_FALSE(r_states.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_states.Get().size());
    CHECK_EQUAL(0, r_states.Get()[0]->RadioID);
    CHECK_EQUAL(RadioAdministrativeState::States::Enabled, r_states.Get()[0]->AdminState);
    CHECK_EQUAL(1, r_states.Get()[1]->RadioID);
    CHECK_EQUAL(RadioAdministrativeState::States::Disabled, r_states.Get()[1]->AdminState);
}