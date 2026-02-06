#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/ReturnedMessageElement.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(ReturnedMessageElementTestsGroup){ //
                                              TEST_SETUP(){}

                                              TEST_TEARDOWN(){}
};

TEST(ReturnedMessageElementTestsGroup, ReturnedMessageElementArray) {
    uint8_t buffer[4096] = {};

    const uint8_t message_element_0[] = { 0, 1, 2, 3, 4 };
    const uint8_t message_element_1[] = { 100, 101, 102, 103, 104, 105, 106, 107 };
    const uint8_t message_element_2[] = {
        200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211
    };

    WritableReturnedMessageElementArray write_data;

    write_data.Add(ReturnedMessageElement::Reasons::Reserved,
                   { message_element_0, message_element_0 + sizeof(message_element_0) });
    write_data.Add(ReturnedMessageElement::Reasons::UnknownMessageElement,
                   { message_element_1, message_element_1 + sizeof(message_element_1) });
    write_data.Add(ReturnedMessageElement::Reasons::UnsupportedMessageElement,
                   { message_element_2, message_element_2 + sizeof(message_element_2) });

    CHECK_TRUE(write_data.Validate());

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 43, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x22, 0x00, 0x07, 0x00, 0x05, 0x00, 0x01, 0x02, 0x03, 0x04,
                                  0x00, 0x22, 0x00, 0x0A, 0x01, 0x08, 0x64, 0x65, 0x66, 0x67, 0x68,
                                  0x69, 0x6A, 0x6B, 0x00, 0x22, 0x00, 0x0E, 0x02, 0x0C, 0xC8, 0xC9,
                                  0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3 };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    ReadableReturnedMessageElementArray read_data;
    raw_data = { buffer, buffer + 43 };
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(3, read_data.Get().size());
    CHECK_EQUAL(ReturnedMessageElement::Reasons::Reserved, read_data.Get()[0]->GetReason());
    CHECK_EQUAL(5, read_data.Get()[0]->GetDataLength());
    MEMCMP_EQUAL(message_element_0, read_data.Get()[0]->data, sizeof(message_element_0));

    CHECK_EQUAL(ReturnedMessageElement::Reasons::UnknownMessageElement,
                read_data.Get()[1]->GetReason());
    CHECK_EQUAL(8, read_data.Get()[1]->GetDataLength());
    MEMCMP_EQUAL(message_element_1, read_data.Get()[1]->data, sizeof(message_element_1));

    CHECK_EQUAL(ReturnedMessageElement::Reasons::UnsupportedMessageElement,
                read_data.Get()[2]->GetReason());
    CHECK_EQUAL(12, read_data.Get()[2]->GetDataLength());
    MEMCMP_EQUAL(message_element_2, read_data.Get()[2]->data, sizeof(message_element_2));
}

TEST(ReturnedMessageElementTestsGroup,
     ReturnedMessageElementArray_valid_message_element_with_length_less_or_equal_255_bytes) {
    uint8_t buffer[4096] = {};

    const uint8_t message_element[255] = {};

    WritableReturnedMessageElementArray write_data;
    write_data.Add(ReturnedMessageElement::Reasons::Reserved,
                   { message_element, message_element + sizeof(message_element) });

    CHECK_TRUE(write_data.Validate());

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 261, raw_data.current);
    raw_data = { buffer, buffer + 261 };

    ReadableReturnedMessageElementArray read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(1, read_data.Get().size());
    CHECK_EQUAL(255, read_data.Get()[0]->GetDataLength());
}

TEST(ReturnedMessageElementTestsGroup,
     ReturnedMessageElementArray_invalid_message_elements_with_length_greater_than_255_bytes) {
    uint8_t buffer[4096] = {};

    const uint8_t message_element[256] = {};

    WritableReturnedMessageElementArray write_data;
    write_data.Add(ReturnedMessageElement::Reasons::Reserved,
                   { message_element, message_element + sizeof(message_element) });

    CHECK_FALSE(write_data.Validate());

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 262, raw_data.current);
    raw_data = { buffer, buffer + 262 };

    ReadableReturnedMessageElementArray read_data;
    CHECK_FALSE(read_data.Deserialize(&raw_data));
}

TEST(ReturnedMessageElementTestsGroup, ReturnedMessageElementArray_clear) {
    uint8_t buffer[4096] = {};

    const uint8_t message_element[255] = {};

    WritableReturnedMessageElementArray write_data;
    write_data.Add(ReturnedMessageElement::Reasons::Reserved,
                   { message_element, message_element + sizeof(message_element) });
    write_data.Add(ReturnedMessageElement::Reasons::UnknownMessageElementValue,
                   { message_element, message_element + sizeof(message_element) });

    write_data.Clear();
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 0, raw_data.current);
}

TEST(ReturnedMessageElementTestsGroup, Add_array_of_items_is_unique) {
    uint8_t buffer[4096] = {};

    const uint8_t message_element_0[] = { 0, 1, 2, 3, 4 };
    const uint8_t message_element_1[] = { 100, 101, 102, 103, 104, 105, 106, 107 };

    WritableReturnedMessageElementArray write_data;

    write_data.Add(ReturnedMessageElement::Reasons::Reserved,
                   { message_element_0, message_element_0 + sizeof(message_element_0) });
    write_data.Add(ReturnedMessageElement::Reasons::Reserved,
                   { message_element_0, message_element_0 + sizeof(message_element_0) });
    write_data.Add(ReturnedMessageElement::Reasons::Reserved,
                   { message_element_0, message_element_0 + sizeof(message_element_0) });
    write_data.Add(ReturnedMessageElement::Reasons::UnknownMessageElement,
                   { message_element_1, message_element_1 + sizeof(message_element_1) });
    write_data.Add(ReturnedMessageElement::Reasons::UnknownMessageElement,
                   { message_element_1, message_element_1 + sizeof(message_element_1) });
    write_data.Add(ReturnedMessageElement::Reasons::UnknownMessageElement,
                   { message_element_1, message_element_1 + sizeof(message_element_1) });
    CHECK_TRUE(write_data.Validate());

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    write_data.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableReturnedMessageElementArray read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_FALSE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(2, read_data.Get().size());
    CHECK_EQUAL(ReturnedMessageElement::Reasons::Reserved, read_data.Get()[0]->GetReason());
    CHECK_EQUAL(5, read_data.Get()[0]->GetDataLength());
    MEMCMP_EQUAL(message_element_0, read_data.Get()[0]->data, sizeof(message_element_0));

    CHECK_EQUAL(ReturnedMessageElement::Reasons::UnknownMessageElement,
                read_data.Get()[1]->GetReason());
    CHECK_EQUAL(8, read_data.Get()[1]->GetDataLength());
    MEMCMP_EQUAL(message_element_1, read_data.Get()[1]->data, sizeof(message_element_1));
}