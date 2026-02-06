#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/ACIPv4List.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(ACIPv4ListTestsGroup){ //
                                  TEST_SETUP(){}

                                  TEST_TEARDOWN(){}
};

TEST(ACIPv4ListTestsGroup, ACIPv4List_serialize) {
    uint8_t buffer[256] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    uint32_t addresses[] = {
        { inet_addr("192.168.1.110") }, { inet_addr("192.168.1.111") },
        { inet_addr("192.168.1.112") }, { inet_addr("192.168.1.114") },
        { inet_addr("192.168.1.115") }, { inet_addr("192.168.1.116") },
        { inet_addr("192.168.1.117") }, { inet_addr("192.168.1.118") },
        { inet_addr("192.168.1.119") }, { inet_addr("192.168.1.120") },
    };

    WritableACIPv4List write_data{
        addresses,
    };

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 4 + 40, raw_data.current);
    const uint8_t reference[] = {
        0x00, 0x02, 0x00, 0x28, 0xC0, 0xA8, 0x01, 0x6E, 0xC0, 0xA8, 0x01, 0x6F, 0xC0, 0xA8, 0x01,
        0x70, 0xC0, 0xA8, 0x01, 0x72, 0xC0, 0xA8, 0x01, 0x73, 0xC0, 0xA8, 0x01, 0x74, 0xC0, 0xA8,
        0x01, 0x75, 0xC0, 0xA8, 0x01, 0x76, 0xC0, 0xA8, 0x01, 0x77, 0xC0, 0xA8, 0x01, 0x78
    };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto read_data = ReadableACIPv4List::Deserialize(&raw_data);
    CHECK(read_data != nullptr);

    CHECK_EQUAL(&buffer[0] + sizeof(reference), raw_data.current);
    CHECK_EQUAL(40, read_data->GetLength());
    CHECK_EQUAL(ElementHeader::ElementType::ACIPv4List, read_data->GetElementType());
    CHECK_EQUAL(10, read_data->GetCount());
    CHECK_EQUAL(inet_addr("192.168.1.110"), read_data->addresses[0]);
    CHECK_EQUAL(inet_addr("192.168.1.111"), read_data->addresses[1]);
    CHECK_EQUAL(inet_addr("192.168.1.119"), read_data->addresses[8]);
    CHECK_EQUAL(inet_addr("192.168.1.120"), read_data->addresses[9]);
}

TEST(ACIPv4ListTestsGroup, ACIPv4List_deserialize) {
    // clang-format off
    uint8_t data[] = {
        0x00, 0x02,             // Type = 2 (AC IPv4 List)
        0x00, 0x0C,             // Length = 12 (3 адреса * 4 байта)
        // --- Value (12 байт) ---
        0x0A, 0x0A, 0x0A, 0x64, // 10.10.10.100
        0x0A, 0x0A, 0x0A, 0x65, // 10.10.10.101
        0x0A, 0x0A, 0x14, 0xFE, // 10.10.20.254
    };
    // clang-format on

    RawData raw_data{ data, data + sizeof(data) };

    auto read_data = ReadableACIPv4List::Deserialize(&raw_data);
    CHECK(read_data != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(12, read_data->GetLength());
    CHECK_EQUAL(3, read_data->GetCount());
    CHECK_EQUAL(inet_addr("10.10.10.100"), read_data->addresses[0]);
    CHECK_EQUAL(inet_addr("10.10.10.101"), read_data->addresses[1]);
    CHECK_EQUAL(inet_addr("10.10.20.254"), read_data->addresses[2]);
}
