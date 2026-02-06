#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/ACNameWithPriority.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(ACNameWithPriorityTestsGroup){ //
                                          TEST_SETUP(){}

                                          TEST_TEARDOWN(){}
};

TEST(ACNameWithPriorityTestsGroup, ACNameWithPriority_serialize) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    WritableACNameWithPriorityArray write_data;
    write_data.Add(0, "0123");
    write_data.Add(1, "01234567");
    write_data.Add(2, "01234567890");

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 38, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x05, 0x00, 0x05, 0x00, 0x30, 0x31, 0x32, 0x33, 0x00,
                                  0x05, 0x00, 0x09, 0x01, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
                                  0x36, 0x37, 0x00, 0x05, 0x00, 0x0C, 0x02, 0x30, 0x31, 0x32,
                                  0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30 };

    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(reference) };
    ReadableACNameWithPriorityArray read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(&buffer[0] + sizeof(reference), raw_data.current);
    CHECK_EQUAL(3, read_data.Get().size());
    CHECK_EQUAL(5, read_data.Get()[0]->GetLength());
    CHECK_EQUAL(0, read_data.Get()[0]->GetPriority());
    STRNCMP_EQUAL("0123", (char *)read_data.Get()[0]->name, 4);
    CHECK_EQUAL(4, read_data.Get()[0]->GetNameLenght());

    CHECK_EQUAL(9, read_data.Get()[1]->GetLength());
    CHECK_EQUAL(1, read_data.Get()[1]->GetPriority());
    STRNCMP_EQUAL("01234567", (char *)read_data.Get()[1]->name, 8);
    CHECK_EQUAL(8, read_data.Get()[1]->GetNameLenght());

    CHECK_EQUAL(12, read_data.Get()[2]->GetLength());
    CHECK_EQUAL(2, read_data.Get()[2]->GetPriority());
    STRNCMP_EQUAL("01234567890", (char *)read_data.Get()[2]->name, 11);
    CHECK_EQUAL(11, read_data.Get()[2]->GetNameLenght());
}

TEST(ACNameWithPriorityTestsGroup, ACNameWithPriority_deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Message Element Header ----
        0x00, 0x05,
        0x00, 0x1B, // Length: 27 байт (1 байт Data Type + 26 байт Data)

        // ---- Message Element Value ----
        0x01,

        // Data: "This is a test data chunk." (26 байт)
        0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20, 0x74, 0x65,
        0x73, 0x74, 0x20, 0x64, 0x61, 0x74, 0x61, 0x20, 0x63, 0x68, 0x75, 0x6E,
        0x6B, 0x2E
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };

    ReadableACNameWithPriorityArray read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(1, read_data.Get().size());
    CHECK_EQUAL(27, read_data.Get()[0]->GetLength());
    CHECK_EQUAL(1, read_data.Get()[0]->GetPriority());
    STRNCMP_EQUAL("This is a test data chunk.", read_data.Get()[0]->name, 26);
    CHECK_EQUAL(26, read_data.Get()[0]->GetNameLenght());
}

TEST(ACNameWithPriorityTestsGroup, Add_array_of_items_is_unique) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    WritableACNameWithPriorityArray write_data;
    write_data.Add(0, "0123");
    write_data.Add(0, "0123");
    write_data.Add(0, "0123");
    write_data.Add(1, "01234567");
    write_data.Add(1, "01234567");
    write_data.Add(1, "01234567");

    write_data.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };
    ReadableACNameWithPriorityArray read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_FALSE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(2, read_data.Get().size());
    CHECK_EQUAL(5, read_data.Get()[0]->GetLength());
    CHECK_EQUAL(0, read_data.Get()[0]->GetPriority());
    STRNCMP_EQUAL("0123", (char *)read_data.Get()[0]->name, 4);
    CHECK_EQUAL(4, read_data.Get()[0]->GetNameLenght());

    CHECK_EQUAL(9, read_data.Get()[1]->GetLength());
    CHECK_EQUAL(1, read_data.Get()[1]->GetPriority());
    STRNCMP_EQUAL("01234567", (char *)read_data.Get()[1]->name, 8);
    CHECK_EQUAL(8, read_data.Get()[1]->GetNameLenght());
}