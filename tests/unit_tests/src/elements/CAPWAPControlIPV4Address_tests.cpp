
#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ClearHeader.h"
#include "ControlHeader.h"
#include "elements/CAPWAPControlIPv4Address.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(CAPWAPControlIPv4AddressTestsGroup){ //
                                                TEST_SETUP(){}

                                                TEST_TEARDOWN(){}
};

TEST(CAPWAPControlIPv4AddressTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};
    CAPWAPControlIPv4Address addresses[] = {
        { inet_addr("192.168.100.10"), 10 }, { inet_addr("192.168.100.11"), 11 },
        { inet_addr("192.168.100.12"), 12 }, { inet_addr("192.168.100.14"), 14 },
        { inet_addr("192.168.100.15"), 15 }, { inet_addr("192.168.100.16"), 16 },
        { inet_addr("192.168.100.17"), 17 }, { inet_addr("192.168.100.18"), 18 },
        { inet_addr("192.168.100.19"), 19 }, { inet_addr("192.168.100.20"), 20 },
        { inet_addr("192.168.100.21"), 21 }, { inet_addr("192.168.100.22"), 22 },
        { inet_addr("192.168.100.23"), 23 }, { inet_addr("192.168.100.24"), 24 },
    };

    WritableCAPWAPControlIPV4AdrArray w_addresses{ addresses };

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_addresses.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 140, raw_data.current);
    uint8_t reference[] = { 0x00, 0x0A, 0x00, 0x06, 0xC0, 0xA8, 0x64, 0x0A, 0x00, 0x0A, 0x00, 0x0A,
                            0x00, 0x06, 0xC0, 0xA8, 0x64, 0x0B, 0x00, 0x0B, 0x00, 0x0A, 0x00, 0x06,
                            0xC0, 0xA8, 0x64, 0x0C, 0x00, 0x0C, 0x00, 0x0A, 0x00, 0x06, 0xC0, 0xA8,
                            0x64, 0x0E, 0x00, 0x0E, 0x00, 0x0A, 0x00, 0x06, 0xC0, 0xA8, 0x64, 0x0F,
                            0x00, 0x0F, 0x00, 0x0A, 0x00, 0x06, 0xC0, 0xA8, 0x64, 0x10, 0x00, 0x10,
                            0x00, 0x0A, 0x00, 0x06, 0xC0, 0xA8, 0x64, 0x11, 0x00, 0x11, 0x00, 0x0A,
                            0x00, 0x06, 0xC0, 0xA8, 0x64, 0x12, 0x00, 0x12, 0x00, 0x0A, 0x00, 0x06,
                            0xC0, 0xA8, 0x64, 0x13, 0x00, 0x13, 0x00, 0x0A, 0x00, 0x06, 0xC0, 0xA8,
                            0x64, 0x14, 0x00, 0x14, 0x00, 0x0A, 0x00, 0x06, 0xC0, 0xA8, 0x64, 0x15,
                            0x00, 0x15, 0x00, 0x0A, 0x00, 0x06, 0xC0, 0xA8, 0x64, 0x16, 0x00, 0x16,
                            0x00, 0x0A, 0x00, 0x06, 0xC0, 0xA8, 0x64, 0x17, 0x00, 0x17, 0x00, 0x0A,
                            0x00, 0x06, 0xC0, 0xA8, 0x64, 0x18, 0x00, 0x18 };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    ReadableCAPWAPControlIPV4AdrArray r_addresses;

    raw_data = { reference, reference + sizeof(reference) };

    CHECK_TRUE(r_addresses.Deserialize(&raw_data));
    CHECK_TRUE(r_addresses.Deserialize(&raw_data));
    CHECK_TRUE(r_addresses.Deserialize(&raw_data));
    CHECK_TRUE(r_addresses.Deserialize(&raw_data));
    CHECK_TRUE(r_addresses.Deserialize(&raw_data));
    CHECK_TRUE(r_addresses.Deserialize(&raw_data));
    CHECK_TRUE(r_addresses.Deserialize(&raw_data));
    CHECK_EQUAL(7, r_addresses.Get().size());
    CHECK_EQUAL(inet_addr("192.168.100.10"), r_addresses.Get()[0]->GetIPAddress());
    CHECK_EQUAL(10, r_addresses.Get()[0]->GetWTPCount());
    CHECK_EQUAL(inet_addr("192.168.100.11"), r_addresses.Get()[1]->GetIPAddress());
    CHECK_EQUAL(11, r_addresses.Get()[1]->GetWTPCount());
    CHECK_EQUAL(inet_addr("192.168.100.12"), r_addresses.Get()[2]->GetIPAddress());
    CHECK_EQUAL(12, r_addresses.Get()[2]->GetWTPCount());
    CHECK_EQUAL(inet_addr("192.168.100.14"), r_addresses.Get()[3]->GetIPAddress());
    CHECK_EQUAL(14, r_addresses.Get()[3]->GetWTPCount());
    CHECK_EQUAL(inet_addr("192.168.100.15"), r_addresses.Get()[4]->GetIPAddress());
    CHECK_EQUAL(15, r_addresses.Get()[4]->GetWTPCount());
    CHECK_EQUAL(inet_addr("192.168.100.16"), r_addresses.Get()[5]->GetIPAddress());
    CHECK_EQUAL(16, r_addresses.Get()[5]->GetWTPCount());
    CHECK_EQUAL(inet_addr("192.168.100.17"), r_addresses.Get()[6]->GetIPAddress());
    CHECK_EQUAL(17, r_addresses.Get()[6]->GetWTPCount());
}
