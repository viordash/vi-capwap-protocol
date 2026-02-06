
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ChangeStateEventRequest.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(ChangeStateEventRequestTestsGroup){ //
                                               TEST_SETUP(){}

                                               TEST_TEARDOWN(){}
};

TEST(ChangeStateEventRequestTestsGroup, ChangeStateEventRequest_serialize) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    WritableRadioOperationalStateArray radio_operational_states;
    radio_operational_states.Add({ 0,
                                   RadioOperationalState::States::Enabled,
                                   RadioOperationalState::Causes::AdministrativelySet });
    radio_operational_states.Add(
        { 1, RadioOperationalState::States::Disabled, RadioOperationalState::Causes::Normal });
    radio_operational_states.Add({ 2,
                                   RadioOperationalState::States::Reserved,
                                   RadioOperationalState::Causes::RadioFailure });

    ResultCode result_code = ResultCode::Type::Success;

    const uint8_t UnknownMessageElement[25] = {};
    const uint8_t UnsupportedMessageElement[35] = {};
    WritableReturnedMessageElementArray returned_elements;
    returned_elements.Add(
        ReturnedMessageElement::Reasons::UnknownMessageElement,
        { UnknownMessageElement, UnknownMessageElement + sizeof(UnknownMessageElement) });
    returned_elements.Add(ReturnedMessageElement::Reasons::UnsupportedMessageElement,
                          { UnsupportedMessageElement,
                            UnsupportedMessageElement + sizeof(UnsupportedMessageElement) });

    WritableChangeStateEventRequest write_data(radio_operational_states,
                                               result_code,
                                               returned_elements,
                                               VendorSpecificPayload::Dummy);

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 117 - (sizeof(ClearHeader) + sizeof(ControlHeader)), raw_data.current);
    const uint8_t reference[] = {
        0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x2A, 0x00, 0x65,
        0x00, 0x00, 0x20, 0x00, 0x03, 0x00, 0x01, 0x03, 0x00, 0x20, 0x00, 0x03, 0x01, 0x02, 0x00,
        0x00, 0x20, 0x00, 0x03, 0x02, 0x00, 0x01, 0x00, 0x21, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x22, 0x00, 0x1B, 0x01, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x22, 0x00, 0x25, 0x02, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    MEMCMP_EQUAL(buffer,
                 reference + (sizeof(ClearHeader) + sizeof(ControlHeader)),
                 sizeof(reference) - (sizeof(ClearHeader) + sizeof(ControlHeader)));

    raw_data = { buffer, buffer + 117 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };
    ReadableChangeStateEventRequest read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(3, read_data.radio_operational_states.Get().size());
    CHECK_EQUAL(0, read_data.radio_operational_states.Get()[0]->RadioID);
    CHECK_EQUAL(RadioOperationalState::States::Enabled,
                read_data.radio_operational_states.Get()[0]->State);
    CHECK_EQUAL(RadioOperationalState::Causes::AdministrativelySet,
                read_data.radio_operational_states.Get()[0]->Cause);
    CHECK_EQUAL(1, read_data.radio_operational_states.Get()[1]->RadioID);
    CHECK_EQUAL(RadioOperationalState::States::Disabled,
                read_data.radio_operational_states.Get()[1]->State);
    CHECK_EQUAL(RadioOperationalState::Causes::Normal,
                read_data.radio_operational_states.Get()[1]->Cause);
    CHECK_EQUAL(2, read_data.radio_operational_states.Get()[2]->RadioID);
    CHECK_EQUAL(RadioOperationalState::States::Reserved,
                read_data.radio_operational_states.Get()[2]->State);
    CHECK_EQUAL(RadioOperationalState::Causes::RadioFailure,
                read_data.radio_operational_states.Get()[2]->Cause);

    CHECK_EQUAL(ResultCode::Type::Success, read_data.result_code->type);

    CHECK_EQUAL(2, read_data.returned_message_elements.Get().size());
    CHECK_EQUAL(ReturnedMessageElement::Reasons::UnknownMessageElement,
                read_data.returned_message_elements.Get()[0]->GetReason());
    CHECK_EQUAL(25, read_data.returned_message_elements.Get()[0]->GetDataLength());
    MEMCMP_EQUAL(UnknownMessageElement,
                 read_data.returned_message_elements.Get()[0]->data,
                 sizeof(UnknownMessageElement));

    CHECK_EQUAL(ReturnedMessageElement::Reasons::UnsupportedMessageElement,
                read_data.returned_message_elements.Get()[1]->GetReason());
    CHECK_EQUAL(35, read_data.returned_message_elements.Get()[1]->GetDataLength());
    MEMCMP_EQUAL(UnsupportedMessageElement,
                 read_data.returned_message_elements.Get()[1]->data,
                 sizeof(UnsupportedMessageElement));
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(ChangeStateEventRequestTestsGroup,
     ChangeStateEventRequest_serialize_with_VendorSpecificPayload) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    WritableRadioOperationalStateArray radio_operational_states;
    radio_operational_states.Add({ 0,
                                   RadioOperationalState::States::Enabled,
                                   RadioOperationalState::Causes::AdministrativelySet });
    radio_operational_states.Add(
        { 1, RadioOperationalState::States::Disabled, RadioOperationalState::Causes::Normal });
    radio_operational_states.Add({ 2,
                                   RadioOperationalState::States::Reserved,
                                   RadioOperationalState::Causes::RadioFailure });

    ResultCode result_code = ResultCode::Type::Success;
    WritableReturnedMessageElementArray returned_elements;

    WritableVendorSpecificPayloadArray vendor_specific_payloads;
    vendor_specific_payloads.Add(123456, 789, "01234567890ABCDEF0123");
    vendor_specific_payloads.Add(1, 2, "01234567890A");

    WritableChangeStateEventRequest write_data(radio_operational_states,
                                               result_code,
                                               returned_elements,
                                               vendor_specific_payloads);

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 82, raw_data.current);

    raw_data = { buffer, buffer + 82 };
    ReadableChangeStateEventRequest read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(2, read_data.vendor_specific_payloads.Get().size());
    CHECK_EQUAL(123456, read_data.vendor_specific_payloads.Get()[0]->GetVendorIdentifier());
    CHECK_EQUAL(789, read_data.vendor_specific_payloads.Get()[0]->GetElementId());
    STRNCMP_EQUAL("01234567890ABCDEF0123",
                  (char *)read_data.vendor_specific_payloads.Get()[0]->value,
                  21);
    CHECK_EQUAL(1, read_data.vendor_specific_payloads.Get()[1]->GetVendorIdentifier());
    CHECK_EQUAL(2, read_data.vendor_specific_payloads.Get()[1]->GetElementId());
    STRNCMP_EQUAL("01234567890A", (char *)read_data.vendor_specific_payloads.Get()[1]->value, 12);
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(ChangeStateEventRequestTestsGroup,
     if_returned_message_elements_is_not_valid_change_resultcode_to_error_and_clear_list) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    WritableRadioOperationalStateArray radio_operational_states;
    radio_operational_states.Add({ 0,
                                   RadioOperationalState::States::Enabled,
                                   RadioOperationalState::Causes::AdministrativelySet });

    ResultCode result_code = ResultCode::Type::Success;

    const uint8_t message_element[256] = {};

    WritableReturnedMessageElementArray returned_elements;
    returned_elements.Add(ReturnedMessageElement::Reasons::UnknownMessageElement,
                          { message_element, message_element + sizeof(message_element) });

    WritableChangeStateEventRequest write_data(radio_operational_states,
                                               result_code,
                                               returned_elements,
                                               VendorSpecificPayload::Dummy);

    CHECK_EQUAL(ResultCode::Type::Failure_UnrecognizedMessageElement, write_data.GetResultCode());

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 31 - (sizeof(ClearHeader) + sizeof(ControlHeader)), raw_data.current);

    raw_data = { buffer, buffer + 31 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };
    ReadableChangeStateEventRequest read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(0, read_data.returned_message_elements.Get().size());
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(ChangeStateEventRequestTestsGroup, ChangeStateEventRequest_deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ======== CAPWAP Header (8 байт) ========
        // Спецификация: RFC 5415, Section 4.3
        0x00,              // Preamble: Version 0, Type 0 (CAPWAP Header follows)
        0x10,              // HLEN (5 bits) = 2 (8 bytes), RID (top 3 bits) = 0
        0x42,              // RID (low 2 bits) = 1, WBID (5 bits) = 1 (802.11), T = 0
        0x00,              // F=0, L=0, W=0, M=0, K=0, Flags=0
        0x00, 0x00,        // Fragment ID = 0 (not fragmented)
        0x00, 0x00,        // Fragment Offset = 0, Rsvd = 0 (not fragmented)

        // ======== CAPWAP Control Header (8 байт) ========
        // Спецификация: RFC 5415, Section 4.5.1
        0x00, 0x00, 0x00, 0x0B, // Message Type = 11 (Change State Event Request)
        0x05,                   // Sequence Number = 5 (пример)
        0x00, 0x1C,             // Message Element Length = 28 байт
        0x00,                   // Flags = 0

        // ======== Message Elements (всего 28 байт) ========

        // --- 1. Radio Operational State (Обязательный элемент) ---
        // Спецификация: RFC 5415, Section 4.6.34
        // Длина элемента: 7 байт
        0x00, 0x20,        // Element Type = 32
        0x00, 0x03,        // Length = 3
        0x01,              //   Radio ID = 1
        0x01,              //   State = 1 (Enabled)
        0x00,              //   Cause = 0 (Normal)

        // --- 2. Result Code (Обязательный элемент) ---
        // Спецификация: RFC 5415, Section 4.6.35
        // Длина элемента: 8 байт
        0x00, 0x21,        // Element Type = 33
        0x00, 0x04,        // Length = 4
        0x00, 0x00, 0x00, 0x0C, //   Result Code = 12 (Config Failure, Service Provided Anyhow)

        // --- 3. Returned Message Element (Опциональный, для демонстрации ошибки) ---
        // Спецификация: RFC 5415, Section 4.6.36
        // Длина элемента: 13 байт
        0x00, 0x22,        // Element Type = 34
        0x00, 0x09,        // Length = 9 (ИСПРАВЛЕНО с 10)
        //   Value (9 байт):
        0x03,              //     Reason = 3 (Unknown Message Element Value)
        0x07,              //     Внутренняя Length = 7
        //     Инкапсулированный (неудачный) элемент (7 байт):
        //     (Это гипотетический элемент с Type=4000, который WTP не поняла)
        0x0F, 0xA0,        //       - Original Type = 4000
        0x00, 0x03,        //       - Original Length = 3
        0xFF, 0x00, 0x00   //       - Original Value (например, "красный цвет LED")
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableChangeStateEventRequest read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);

    CHECK_EQUAL(1, read_data.radio_operational_states.Get().size());
    CHECK_EQUAL(1, read_data.radio_operational_states.Get()[0]->RadioID);
    CHECK_EQUAL(RadioOperationalState::States::Enabled,
                read_data.radio_operational_states.Get()[0]->State);
    CHECK_EQUAL(RadioOperationalState::Causes::Normal,
                read_data.radio_operational_states.Get()[0]->Cause);

    CHECK_EQUAL(ResultCode::Type::ConfigurationFailure_ServiceProvided,
                read_data.result_code->type);

    CHECK_EQUAL(1, read_data.returned_message_elements.Get().size());
    CHECK_EQUAL(ReturnedMessageElement::Reasons::UnknownMessageElementValue,
                read_data.returned_message_elements.Get()[0]->GetReason());
    CHECK_EQUAL(7, read_data.returned_message_elements.Get()[0]->GetDataLength());
    CHECK_EQUAL(0x0F, read_data.returned_message_elements.Get()[0]->data[0]);
    CHECK_EQUAL(0xA0, read_data.returned_message_elements.Get()[0]->data[1]);
    CHECK_EQUAL(0x00, read_data.returned_message_elements.Get()[0]->data[2]);
    CHECK_EQUAL(0x03, read_data.returned_message_elements.Get()[0]->data[3]);
    CHECK_EQUAL(0xFF, read_data.returned_message_elements.Get()[0]->data[4]);
    CHECK_EQUAL(0x00, read_data.returned_message_elements.Get()[0]->data[5]);
    CHECK_EQUAL(0x00, read_data.returned_message_elements.Get()[0]->data[6]);
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(ChangeStateEventRequestTestsGroup,
     ChangeStateEventRequest_deserialize_handle_unknown_element) {
    // clang-format off
    uint8_t data[] = {
      // ======== CAPWAP Header (8 байт) ========
        // Спецификация: RFC 5415, Section 4.3
        0x00,              // Preamble: Version 0, Type 0 (CAPWAP Header follows)
        0x10,              // HLEN (5 bits) = 2 (8 bytes), RID (top 3 bits) = 0
        0x42,              // RID (low 2 bits) = 1, WBID (5 bits) = 1 (802.11), T = 0
        0x00,              // F=0, L=0, W=0, M=0, K=0, Flags=0
        0x00, 0x00,        // Fragment ID = 0 (not fragmented)
        0x00, 0x00,        // Fragment Offset = 0, Rsvd = 0 (not fragmented)

        // ======== CAPWAP Control Header (8 байт) ========
        // Спецификация: RFC 5415, Section 4.5.1
        0x00, 0x00, 0x00, 0x0B, // Message Type = 11 (Change State Event Request)
        0x05,                   // Sequence Number = 5 (пример)
        0x00, 0x1C - 3,           
        0x00,                   // Flags = 0
       
        // --- 1. Radio Operational State (Обязательный элемент) ---
        // Спецификация: RFC 5415, Section 4.6.34
        // Длина элемента: 7 байт
        0x00, 0x20,        // Element Type = 32
        0x00, 0x03,        // Length = 3
        0x01,              //   Radio ID = 1
        0x01,              //   State = 1 (Enabled)
        0x00,              //   Cause = 0 (Normal)

        // --- 2. Result Code (Обязательный элемент) ---
        // Спецификация: RFC 5415, Section 4.6.35
        // Длина элемента: 8 байт
        0x00, 0x21,        // Element Type = 33
        0x00, 0x04,        // Length = 4
        0x00, 0x00, 0x00, 0x0C, //   Result Code = 12 (Config Failure, Service Provided Anyhow)

        // 7. Unknown (5 байт)
        0xFF, 0xFF, 0x00, 0x01, 0x00,
        // 8. Unknown (5 байт)
        0xFF, 0xFE, 0x00, 0x01, 0x00,
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableChangeStateEventRequest read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, read_data.unknown_elements);
}