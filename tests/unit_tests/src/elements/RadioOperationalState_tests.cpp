#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ClearHeader.h"
#include "ControlHeader.h"
#include "elements/RadioOperationalState.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(RadioOperationalStateTestsGroup){ //
                                             TEST_SETUP(){}

                                             TEST_TEARDOWN(){}
};

TEST(RadioOperationalStateTestsGroup, Deserialize) {
    uint8_t data[] = {
        0x00, 0x20, // Type: 32
        0x00, 0x03, // Length: 2 байта
        0x01,       // Value -> Radio ID: 1
        0x01,       // Value -> State: 1 (Enabled)
        0x03        // Value -> Cause: 3 (AdministrativelySet)
    };
    RawData raw_data{ data, data + sizeof(data) };
    auto element = RadioOperationalState::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::RadioOperationalState, element->GetElementType());

    CHECK_EQUAL(1, element->RadioID);
    CHECK_EQUAL(RadioOperationalState::States::Enabled, element->State);
    CHECK_EQUAL(RadioOperationalState::Causes::AdministrativelySet, element->Cause);
}

TEST(RadioOperationalStateTestsGroup, Serialize) {
    uint8_t buffer[256] = {};
    RadioOperationalState element_0{ 3,
                                     RadioOperationalState::States::Disabled,
                                     RadioOperationalState::Causes::RadioFailure };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 7, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x20, 0x00, 0x03, 0x03, 0x02, 0x01 };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = RadioOperationalState::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(&buffer[0] + 7, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::RadioOperationalState, element->GetElementType());
    CHECK_EQUAL(3, element->RadioID);
    CHECK_EQUAL(RadioOperationalState::States::Disabled, element->State);
    CHECK_EQUAL(RadioOperationalState::Causes::RadioFailure, element->Cause);
}

TEST(RadioOperationalStateTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableRadioOperationalStateArray w_states;
    w_states.Add({ 0,
                   RadioOperationalState::States::Enabled,
                   RadioOperationalState::Causes::AdministrativelySet });
    w_states.Add(
        { 1, RadioOperationalState::States::Disabled, RadioOperationalState::Causes::Normal });
    w_states.Add({ 2,
                   RadioOperationalState::States::Reserved,
                   RadioOperationalState::Causes::RadioFailure });
    w_states.Add({ 3,
                   RadioOperationalState::States::Enabled,
                   RadioOperationalState::Causes::SoftwareFailure });
    w_states.Add({ 4,
                   RadioOperationalState::States::Disabled,
                   RadioOperationalState::Causes::AdministrativelySet });
    w_states.Add(
        { 5, RadioOperationalState::States::Reserved, RadioOperationalState::Causes::Normal });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_states.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 42, raw_data.current);
    uint8_t reference[] = { 0x00, 0x20, 0x00, 0x03, 0x00, 0x01, 0x03, 0x00, 0x20, 0x00, 0x03,
                            0x01, 0x02, 0x00, 0x00, 0x20, 0x00, 0x03, 0x02, 0x00, 0x01, 0x00,
                            0x20, 0x00, 0x03, 0x03, 0x01, 0x02, 0x00, 0x20, 0x00, 0x03, 0x04,
                            0x02, 0x03, 0x00, 0x20, 0x00, 0x03, 0x05, 0x00, 0x00 };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    ReadableRadioOperationalStateArray r_states;

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
    CHECK_EQUAL(RadioOperationalState::States::Enabled, r_states.Get()[0]->State);
    CHECK_EQUAL(RadioOperationalState::Causes::AdministrativelySet, r_states.Get()[0]->Cause);
    CHECK_EQUAL(1, r_states.Get()[1]->RadioID);
    CHECK_EQUAL(RadioOperationalState::States::Disabled, r_states.Get()[1]->State);
    CHECK_EQUAL(RadioOperationalState::Causes::Normal, r_states.Get()[1]->Cause);
    CHECK_EQUAL(2, r_states.Get()[2]->RadioID);
    CHECK_EQUAL(RadioOperationalState::States::Reserved, r_states.Get()[2]->State);
    CHECK_EQUAL(RadioOperationalState::Causes::RadioFailure, r_states.Get()[2]->Cause);
    CHECK_EQUAL(3, r_states.Get()[3]->RadioID);
    CHECK_EQUAL(RadioOperationalState::States::Enabled, r_states.Get()[3]->State);
    CHECK_EQUAL(RadioOperationalState::Causes::SoftwareFailure, r_states.Get()[3]->Cause);
    CHECK_EQUAL(4, r_states.Get()[4]->RadioID);
    CHECK_EQUAL(RadioOperationalState::States::Disabled, r_states.Get()[4]->State);
    CHECK_EQUAL(RadioOperationalState::Causes::AdministrativelySet, r_states.Get()[4]->Cause);
    CHECK_EQUAL(5, r_states.Get()[5]->RadioID);
    CHECK_EQUAL(RadioOperationalState::States::Reserved, r_states.Get()[5]->State);
    CHECK_EQUAL(RadioOperationalState::Causes::Normal, r_states.Get()[5]->Cause);
}

TEST(RadioOperationalStateTestsGroup, Add_array_of_items_is_unique) {
    uint8_t buffer[2048] = {};

    WritableRadioOperationalStateArray w_states;
    w_states.Add({ 0,
                   RadioOperationalState::States::Enabled,
                   RadioOperationalState::Causes::AdministrativelySet });
    w_states.Add({ 0,
                   RadioOperationalState::States::Enabled,
                   RadioOperationalState::Causes::AdministrativelySet });
    w_states.Add({ 0,
                   RadioOperationalState::States::Enabled,
                   RadioOperationalState::Causes::AdministrativelySet });
    w_states.Add(
        { 1, RadioOperationalState::States::Disabled, RadioOperationalState::Causes::Normal });
    w_states.Add(
        { 1, RadioOperationalState::States::Disabled, RadioOperationalState::Causes::Normal });
    w_states.Add(
        { 1, RadioOperationalState::States::Disabled, RadioOperationalState::Causes::Normal });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_states.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableRadioOperationalStateArray r_states;

    CHECK_TRUE(r_states.Deserialize(&raw_data));
    CHECK_TRUE(r_states.Deserialize(&raw_data));
    CHECK_FALSE(r_states.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_states.Get().size());
    CHECK_EQUAL(0, r_states.Get()[0]->RadioID);
    CHECK_EQUAL(RadioOperationalState::States::Enabled, r_states.Get()[0]->State);
    CHECK_EQUAL(RadioOperationalState::Causes::AdministrativelySet, r_states.Get()[0]->Cause);
    CHECK_EQUAL(1, r_states.Get()[1]->RadioID);
    CHECK_EQUAL(RadioOperationalState::States::Disabled, r_states.Get()[1]->State);
    CHECK_EQUAL(RadioOperationalState::Causes::Normal, r_states.Get()[1]->Cause);
}