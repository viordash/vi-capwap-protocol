#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/WTPName.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(WTPNameTestsGroup){ //
                               TEST_SETUP(){}

                               TEST_TEARDOWN(){}
};

TEST(WTPNameTestsGroup, WTPName_serialize) {
    uint8_t buffer[256] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    WritableWTPName write_data{ "abcdefабвгд" };

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 4 + 16, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x2D, 0x00, 0x10, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
                                  0xD0, 0xB0, 0xD0, 0xB1, 0xD0, 0xB2, 0xD0, 0xB3, 0xD0, 0xB4 };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto read_data = ReadableWTPName::Deserialize(&raw_data);
    CHECK(read_data != nullptr);

    CHECK_EQUAL(&buffer[0] + sizeof(reference), raw_data.current);
    CHECK_EQUAL(16, read_data->GetLength());
    CHECK_EQUAL(ElementHeader::ElementType::WTPName, read_data->GetElementType());
    STRNCMP_EQUAL("abcdefабвгд", (char *)read_data->name, 16);
}

TEST(WTPNameTestsGroup, WTPName_take_ownership) {
    uint8_t buffer[256] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    std::string str("abcdefабвгд");

    WritableWTPName write_data{ str };
    str.clear();

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 4 + 16, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x2D, 0x00, 0x10, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
                                  0xD0, 0xB0, 0xD0, 0xB1, 0xD0, 0xB2, 0xD0, 0xB3, 0xD0, 0xB4 };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto read_data = ReadableWTPName::Deserialize(&raw_data);
    CHECK(read_data != nullptr);

    CHECK_EQUAL(&buffer[0] + sizeof(reference), raw_data.current);
    CHECK_EQUAL(16, read_data->GetLength());
    CHECK_EQUAL(ElementHeader::ElementType::WTPName, read_data->GetElementType());
    STRNCMP_EQUAL("abcdefабвгд", (char *)read_data->name, 16);
}

TEST(WTPNameTestsGroup, WTPName_deserialize_ascii) {
    // clang-format off
// Юнит-тест: WTP Name - Пример 1 (простое ASCII-имя)
// Тип элемента = 4, Длина = 14
 uint8_t data[] = {
    // --- TLV Header (4 байта) ---
    0x00, 0x2D,       // Type = 45 (в Big Endian)
    0x00, 0x0E,       // Length = 14 (в Big Endian)

    // --- Value (14 байт): "Corporate-WTP-1" ---
    0x43, 0x6F, 0x72, 0x70, 0x6F, 0x72, 0x61, 0x74, // Corporat
    0x65, 0x2D, 0x41, 0x43, 0x2D, 0x31              // e-AC-1
};
    // clang-format on

    RawData raw_data{ data, data + sizeof(data) };

    auto read_data = ReadableWTPName::Deserialize(&raw_data);
    CHECK(read_data != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(14, read_data->GetLength());
    STRNCMP_EQUAL("Corporate-AC-1", (char *)read_data->name, 14);
}

TEST(WTPNameTestsGroup, WTPName_deserialize_utf8) {
    // clang-format off
// Юнит-тест: WTP Name - Пример 2 (имя с UTF-8)
// Тип элемента = 4, Длина = 16
uint8_t data[] = {
    // --- TLV Header (4 байта) ---
    0x00, 0x2D,       // Type = 45
    0x00, 0x10,       // Length = 16

    // --- Value (16 байт): "München-WTP-Main" ---
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

    auto read_data = ReadableWTPName::Deserialize(&raw_data);
    CHECK(read_data != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(16, read_data->GetLength());
    STRNCMP_EQUAL("München-AC-Main", (char *)read_data->name, 16);
}
