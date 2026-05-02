#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ClearHeader.h"
#include "ControlHeader.h"
#include "elements/WTPStaticIPAddressInformation.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(WTPStaticIPAddressInformationTestsGroup){ //
                                                     TEST_SETUP(){}

                                                     TEST_TEARDOWN(){}
};

TEST(WTPStaticIPAddressInformationTestsGroup, Deserialize) {
    // clang-format off
    uint8_t data[] = {
        0x00, 0x31, // Type: 49 (0x0031)
        0x00, 0x0D, // Length: 13 (0x000D)

        // --- Поле Value (13 байт) ---
        // IP Address: 192.168.1.100
        0xC0, 0xA8, 0x01, 0x64,
        // Netmask: 255.255.255.0
        0xFF, 0xFF, 0xFF, 0x00,
        // Gateway: 192.168.1.1
        0xC0, 0xA8, 0x01, 0x01,
        // Static: 1 (Enabled)
        0x01
    };
    // clang-format on

    RawData raw_data{ data, data + sizeof(data) };
    ReadableWTPStaticIPAddressInformation read_data;
    CHECK_FALSE(read_data.IsPresent());
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::WTPStaticIPAddressInformation,
                read_data.GetElementType());

    CHECK_EQUAL(inet_addr("192.168.1.100"), read_data.Get()->IpAddress);
    CHECK_EQUAL(inet_addr("255.255.255.0"), read_data.Get()->Netmask);
    CHECK_EQUAL(inet_addr("192.168.1.1"), read_data.Get()->Gateway);
    CHECK_EQUAL(1, read_data.Get()->Static);
    CHECK_TRUE(read_data.IsPresent());
}

TEST(WTPStaticIPAddressInformationTestsGroup, Serialize) {
    uint8_t buffer[256] = {};
    WritableWTPStaticIPAddressInformation element_0{ inet_addr("192.168.100.10"),
                                                     inet_addr("255.255.255.0"),
                                                     inet_addr("192.168.1.1"),
                                                     true };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 17, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x31, 0x00, 0x0D, 0xC0, 0xA8, 0x64, 0x0A, 0xFF,
                                  0xFF, 0xFF, 0x00, 0xC0, 0xA8, 0x01, 0x01, 0x01 };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    ReadableWTPStaticIPAddressInformation read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(&buffer[0] + 17, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::WTPStaticIPAddressInformation,
                read_data.GetElementType());
    CHECK_EQUAL(inet_addr("192.168.100.10"), read_data.Get()->IpAddress);
    CHECK_EQUAL(inet_addr("255.255.255.0"), read_data.Get()->Netmask);
    CHECK_EQUAL(inet_addr("192.168.1.1"), read_data.Get()->Gateway);
    CHECK_EQUAL(1, read_data.Get()->Static);
}
