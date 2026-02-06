#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/VendorSpecificPayload.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(VendorSpecificPayloadTestsGroup){ //
                                             TEST_SETUP(){}

                                             TEST_TEARDOWN(){}
};

TEST(VendorSpecificPayloadTestsGroup, VendorSpecificPayload_serialize) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    VendorSpecificPayload payload{ 123456, 789, 0 };

    payload.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 10, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x25, 0x00, 0x06, 0x00, 0x01, 0xE2, 0x40, 0x03, 0x15 };

    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = VendorSpecificPayload::Deserialize(&raw_data);
    CHECK(element != nullptr);

    CHECK_EQUAL(&buffer[0] + sizeof(reference), raw_data.current);
    CHECK_EQUAL(6, element->GetLength());
    CHECK_EQUAL(123456, element->GetVendorIdentifier());
    CHECK_EQUAL(789, element->GetElementId());
}

TEST(VendorSpecificPayloadTestsGroup, VendorSpecificPayload_deserialize) {
    uint8_t data[] = {
        // 7. [ОПЦИОНАЛЬНО] Vendor Specific Payload (Тип: 37, Длина: 7)
        0x00,
        0x25,
        0x00,
        0x07,
        // Value:
        0x00,
        0x00,
        0x00,
        0x09, // Vendor ID: 9 (Cisco)
        0xDE,
        0xAD, // Element ID: 0xDEAD
        0x01  // Data: 1 байт (например, флаг отладки)
    };
    RawData raw_data{ data, data + sizeof(data) };

    auto element = VendorSpecificPayload::Deserialize(&raw_data);
    CHECK(element != nullptr);

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(1 + 6, element->GetLength());
    CHECK_EQUAL(9, element->GetVendorIdentifier());
    CHECK_EQUAL(0xDEAD, element->GetElementId());
    CHECK_EQUAL(1, element->value[0]);
}

TEST(VendorSpecificPayloadTestsGroup, VendorSpecificPayloadArray_serialize) {
    uint8_t buffer[4096] = {};

    WritableVendorSpecificPayloadArray write_data;
    write_data.Add(12300, 700, { '0', '1', '2', '3' });
    write_data.Add(12301, 701, { '0', '1', '2', '3', '4', '5', '6', '7' });
    write_data.Add(12302, 702, { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' });
    write_data.Add(12303, 703, "012345678900123");
    write_data.Add(12304, 704, "0123456789001234567");
    write_data.Add(12305, 705, "0123456789001234567890");

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 139, raw_data.current);
    const uint8_t reference[] = {
        0x00, 0x25, 0x00, 0x0A, 0x00, 0x00, 0x30, 0x0C, 0x02, 0xBC, 0x30, 0x31, 0x32, 0x33,
        0x00, 0x25, 0x00, 0x0E, 0x00, 0x00, 0x30, 0x0D, 0x02, 0xBD, 0x30, 0x31, 0x32, 0x33,
        0x34, 0x35, 0x36, 0x37, 0x00, 0x25, 0x00, 0x11, 0x00, 0x00, 0x30, 0x0E, 0x02, 0xBE,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x00, 0x25, 0x00,
        0x15, 0x00, 0x00, 0x30, 0x0F, 0x02, 0xBF, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
        0x37, 0x38, 0x39, 0x30, 0x30, 0x31, 0x32, 0x33, 0x00, 0x25, 0x00, 0x19, 0x00, 0x00,
        0x30, 0x10, 0x02, 0xC0, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
        0x30, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x00, 0x25, 0x00, 0x1C, 0x00,
        0x00, 0x30, 0x11, 0x02, 0xC1, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
        0x39, 0x30, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30
    };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));
}

TEST(VendorSpecificPayloadTestsGroup, Add_array_of_items_is_unique) {
    uint8_t buffer[4096] = {};

    WritableVendorSpecificPayloadArray write_data;
    write_data.Add(12300, 700, { '0', '1', '2', '3' });
    write_data.Add(12300, 700, { '0', '1', '2', '3' });
    write_data.Add(12300, 700, { '0', '1', '2', '3' });
    write_data.Add(12301, 701, { '0', '1', '2', '3', '4', '5', '6', '7' });
    write_data.Add(12301, 701, { '0', '1', '2', '3', '4', '5', '6', '7' });
    write_data.Add(12301, 701, { '0', '1', '2', '3', '4', '5', '6', '7' });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    write_data.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableVendorSpecificPayloadArray read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_FALSE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(2, read_data.Get().size());
}