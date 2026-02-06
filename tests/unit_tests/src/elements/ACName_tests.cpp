#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/ACName.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(ACNameTestsGroup){ //
                              TEST_SETUP(){}

                              TEST_TEARDOWN(){}
};

TEST(ACNameTestsGroup, ACName_serialize) {
    uint8_t buffer[256] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    const char *name = "abcdefабвгд";

    WritableACName write_data{ name };

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 4 + 16, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x04, 0x00, 0x10, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
                                  0xD0, 0xB0, 0xD0, 0xB1, 0xD0, 0xB2, 0xD0, 0xB3, 0xD0, 0xB4 };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto read_data = ReadableACName::Deserialize(&raw_data);
    CHECK(read_data != nullptr);

    CHECK_EQUAL(&buffer[0] + sizeof(reference), raw_data.current);
    CHECK_EQUAL(16, read_data->GetLength());
    STRNCMP_EQUAL("abcdefабвгд", (char *)read_data->name, 16);
}

TEST(ACNameTestsGroup, ACName_empty) {
    uint8_t buffer[256] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    WritableACName write_data{ "" };

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 4 + 0, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x04, 0x00, 0x00 };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto read_data = ReadableACName::Deserialize(&raw_data);
    CHECK(read_data != nullptr);

    CHECK_EQUAL(&buffer[0] + sizeof(reference), raw_data.current);
    CHECK_EQUAL(0, read_data->GetLength());
}

TEST(ACNameTestsGroup, ACName_deserialize_ascii) {
    // clang-format off
// Юнит-тест: AC Name - Пример 1 (простое ASCII-имя)
// Тип элемента = 4, Длина = 14
 uint8_t data[] = {
    // --- TLV Header (4 байта) ---
    0x00, 0x04,       // Type = 4 (в Big Endian)
    0x00, 0x0E,       // Length = 14 (в Big Endian)

    // --- Value (14 байт): "Corporate-AC-1" ---
    0x43, 0x6F, 0x72, 0x70, 0x6F, 0x72, 0x61, 0x74, // Corporat
    0x65, 0x2D, 0x41, 0x43, 0x2D, 0x31              // e-AC-1
};
    // clang-format on

    RawData raw_data{ data, data + sizeof(data) };

    auto read_data = ReadableACName::Deserialize(&raw_data);
    CHECK(read_data != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(14, read_data->GetLength());
    STRNCMP_EQUAL("Corporate-AC-1", (char *)read_data->name, 14);
}

TEST(ACNameTestsGroup, ACName_deserialize_utf8) {
    // clang-format off
// Юнит-тест: AC Name - Пример 2 (имя с UTF-8)
// Тип элемента = 4, Длина = 16
uint8_t data[] = {
    // --- TLV Header (4 байта) ---
    0x00, 0x04,       // Type = 4
    0x00, 0x10,       // Length = 16

    // --- Value (16 байт): "München-AC-Main" ---
    0x4D,             // M
    0xC3, 0xBC,       // ü (2 байта в UTF-8)
    0x6E, 0x63, 0x68, 0x65, 0x6E, // nchen
    0x2D,             // -
    0x41, 0x43,       // AC
    0x2D,             // -
    0x4D, 0x61, 0x69, 0x6E  // Main
};
    // clang-format on

    RawData raw_data{ data, data + sizeof(data) };

    auto read_data = ReadableACName::Deserialize(&raw_data);
    CHECK(read_data != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(16, read_data->GetLength());
    STRNCMP_EQUAL("München-AC-Main", (char *)read_data->name, 16);
}
