#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ClearHeader.h"
#include "ControlHeader.h"
#include "elements/CAPWAPLocalIPv4Address.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(CAPWAPLocalIPv4AddressTestsGroup){ //
                                              TEST_SETUP(){}

                                              TEST_TEARDOWN(){}
};

TEST(CAPWAPLocalIPv4AddressTestsGroup, Deserialize) {
    // Юнит-тест: CAPWAP Local IPv4 Address - Пример 1 (WTP за NAT)
    // Тип элемента = 30, Длина = 4
    uint8_t data[] = {
        // --- TLV Header (4 байта) ---
        0x00,
        0x1E, // Type = 30 (в Big Endian)
        0x00,
        0x04, // Length = 4 (в Big Endian)

        // --- Value (4 байта) ---
        0xC0,
        0xA8,
        0x01,
        0x64 // IP Address: 192.168.1.100
    };
    RawData raw_data{ data, data + sizeof(data) };
    auto element = CAPWAPLocalIPv4Address::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::CAPWAPLocalIPv4Address, element->GetElementType());

    CHECK_EQUAL(inet_addr("192.168.1.100"), element->GetIPAddress());
}

TEST(CAPWAPLocalIPv4AddressTestsGroup, Serialize) {
    uint8_t buffer[256] = {};
    CAPWAPLocalIPv4Address element_0{ inet_addr("192.168.1.100") };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 8, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x1E, 0x00, 0x04, 0xC0, 0xA8, 0x01, 0x64 };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = CAPWAPLocalIPv4Address::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(&buffer[0] + 8, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::CAPWAPLocalIPv4Address, element->GetElementType());
    CHECK_EQUAL(inet_addr("192.168.1.100"), element->GetIPAddress());
}

TEST(CAPWAPLocalIPv4AddressTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};
    CAPWAPLocalIPv4Address addresses[] = {
        { inet_addr("192.168.1.110") }, { inet_addr("192.168.1.111") },
        { inet_addr("192.168.1.112") }, { inet_addr("192.168.1.114") },
        { inet_addr("192.168.1.115") }, { inet_addr("192.168.1.116") },
        { inet_addr("192.168.1.117") }, { inet_addr("192.168.1.118") },
        { inet_addr("192.168.1.119") }, { inet_addr("192.168.1.120") },
        { inet_addr("192.168.1.121") }, { inet_addr("192.168.1.122") },
        { inet_addr("192.168.1.123") }, { inet_addr("192.168.1.124") },
    };

    WritableCAPWAPLocalIPV4AdrArray w_addresses{ addresses };

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_addresses.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 112, raw_data.current);
    uint8_t reference[] = { 0x00, 0x1E, 0x00, 0x04, 0xC0, 0xA8, 0x01, 0x6E, 0x00, 0x1E, 0x00, 0x04,
                            0xC0, 0xA8, 0x01, 0x6F, 0x00, 0x1E, 0x00, 0x04, 0xC0, 0xA8, 0x01, 0x70,
                            0x00, 0x1E, 0x00, 0x04, 0xC0, 0xA8, 0x01, 0x72, 0x00, 0x1E, 0x00, 0x04,
                            0xC0, 0xA8, 0x01, 0x73, 0x00, 0x1E, 0x00, 0x04, 0xC0, 0xA8, 0x01, 0x74,
                            0x00, 0x1E, 0x00, 0x04, 0xC0, 0xA8, 0x01, 0x75, 0x00, 0x1E, 0x00, 0x04,
                            0xC0, 0xA8, 0x01, 0x76, 0x00, 0x1E, 0x00, 0x04, 0xC0, 0xA8, 0x01, 0x77,
                            0x00, 0x1E, 0x00, 0x04, 0xC0, 0xA8, 0x01, 0x78, 0x00, 0x1E, 0x00, 0x04,
                            0xC0, 0xA8, 0x01, 0x79, 0x00, 0x1E, 0x00, 0x04, 0xC0, 0xA8, 0x01, 0x7A,
                            0x00, 0x1E, 0x00, 0x04, 0xC0, 0xA8, 0x01, 0x7B, 0x00, 0x1E, 0x00, 0x04,
                            0xC0, 0xA8, 0x01, 0x7C };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    ReadableCAPWAPLocalIPV4AdrArray r_addresses;

    raw_data = { reference, reference + sizeof(reference) };

    CHECK_TRUE(r_addresses.Deserialize(&raw_data));
    CHECK_TRUE(r_addresses.Deserialize(&raw_data));
    CHECK_TRUE(r_addresses.Deserialize(&raw_data));
    CHECK_TRUE(r_addresses.Deserialize(&raw_data));
    CHECK_TRUE(r_addresses.Deserialize(&raw_data));
    CHECK_TRUE(r_addresses.Deserialize(&raw_data));
    CHECK_TRUE(r_addresses.Deserialize(&raw_data));
    CHECK_EQUAL(7, r_addresses.Get().size());
    CHECK_EQUAL(inet_addr("192.168.1.110"), r_addresses.Get()[0]->GetIPAddress());
    CHECK_EQUAL(inet_addr("192.168.1.111"), r_addresses.Get()[1]->GetIPAddress());
    CHECK_EQUAL(inet_addr("192.168.1.112"), r_addresses.Get()[2]->GetIPAddress());
    CHECK_EQUAL(inet_addr("192.168.1.114"), r_addresses.Get()[3]->GetIPAddress());
    CHECK_EQUAL(inet_addr("192.168.1.115"), r_addresses.Get()[4]->GetIPAddress());
    CHECK_EQUAL(inet_addr("192.168.1.116"), r_addresses.Get()[5]->GetIPAddress());
    CHECK_EQUAL(inet_addr("192.168.1.117"), r_addresses.Get()[6]->GetIPAddress());
}
