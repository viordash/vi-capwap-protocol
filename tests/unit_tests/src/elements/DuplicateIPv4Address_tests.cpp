#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/DuplicateIPv4Address.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(DuplicateIPv4AddressTestsGroup){ //
                                            TEST_SETUP(){}

                                            TEST_TEARDOWN(){}
};

TEST(DuplicateIPv4AddressTestsGroup, DuplicateIPv4Address_serialize) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    const uint8_t mac_6_0[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    const uint8_t mac_8_0[] = { 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD, 0xEE, 0xFF };

    WritableDuplicateIPv4AdrArray duplicate_ipv4_address;
    duplicate_ipv4_address.Add(inet_addr("192.168.100.10"), DuplicateStatus::Detected, { mac_6_0 });
    duplicate_ipv4_address.Add(inet_addr("192.168.100.11"), DuplicateStatus::Cleared, { mac_8_0 });
    CHECK_FALSE(duplicate_ipv4_address.Empty());

    duplicate_ipv4_address.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 34, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x15, 0x00, 0x0C, 0xC0, 0xA8, 0x64, 0x0A, 0x01,
                                  0x06, 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x00, 0x15,
                                  0x00, 0x0E, 0xC0, 0xA8, 0x64, 0x0B, 0x00, 0x08, 0xAA,
                                  0xBB, 0xCC, 0xFF, 0xFE, 0xDD, 0xEE, 0xFF };

    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(reference) };
    ReadableDuplicateIPv4AdrArray read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(&buffer[0] + sizeof(reference), raw_data.current);

    CHECK_EQUAL(2, read_data.Get().size());

    CHECK_EQUAL(inet_addr("192.168.100.10"), read_data.Get()[0]->IPAddress);
    CHECK_EQUAL(DuplicateStatus::Detected, read_data.Get()[0]->Status);
    CHECK_EQUAL(6, read_data.Get()[0]->MACAddress.Length);
    MEMCMP_EQUAL(mac_6_0, (char *)read_data.Get()[0]->MACAddress.MACAddresses, sizeof(mac_6_0));

    CHECK_EQUAL(inet_addr("192.168.100.11"), read_data.Get()[1]->IPAddress);
    CHECK_EQUAL(DuplicateStatus::Cleared, read_data.Get()[1]->Status);
    CHECK_EQUAL(8, read_data.Get()[1]->MACAddress.Length);
    MEMCMP_EQUAL(mac_8_0, (char *)read_data.Get()[1]->MACAddress.MACAddresses, sizeof(mac_8_0));
}

TEST(DuplicateIPv4AddressTestsGroup, DuplicateIPv4Address_deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Заголовок элемента сообщения (4 байта) ----
        0x00, 0x15,     // Type: 21 (Duplicate IPv4 Address)
        0x00, 0x0C,     // Length: 12 байт (4 + 1 + 1 + 6 = 12) - это длина последующих данных

        // ---- Данные (Value) (12 байт) ----
        0xC0, 0xA8,     // IP Address: 192.168.1.100
        0x01, 0x64,     // (IP-адрес, который был продублирован)

        0x01,           // Status: 1 (Duplicate address is detected - дубликат обнаружен)

        0x06,           // Length: 6 (длина MAC-адреса "нарушителя")

        0xDE, 0xAD,     // MAC Address: DE:AD:BE:EF:CA:FE
        0xBE, 0xEF,     // (MAC-адрес устройства, вызвавшего конфликт)
        0xCA, 0xFE
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };

    ReadableDuplicateIPv4AdrArray read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(1, read_data.Get().size());

    CHECK_EQUAL(inet_addr("192.168.1.100"), read_data.Get()[0]->IPAddress);
    CHECK_EQUAL(DuplicateStatus::Detected, read_data.Get()[0]->Status);
    CHECK_EQUAL(6, read_data.Get()[0]->MACAddress.Length);
    const uint8_t mac_6_0[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE };
    MEMCMP_EQUAL(mac_6_0, (char *)read_data.Get()[0]->MACAddress.MACAddresses, sizeof(mac_6_0));
}

TEST(DuplicateIPv4AddressTestsGroup, Add_array_of_items_is_unique) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    const uint8_t mac_6_0[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    const uint8_t mac_8_0[] = { 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD, 0xEE, 0xFF };

    WritableDuplicateIPv4AdrArray duplicate_ipv4_address;
    duplicate_ipv4_address.Add(inet_addr("192.168.100.10"), DuplicateStatus::Detected, { mac_6_0 });
    duplicate_ipv4_address.Add(inet_addr("192.168.100.10"), DuplicateStatus::Detected, { mac_6_0 });
    duplicate_ipv4_address.Add(inet_addr("192.168.100.10"), DuplicateStatus::Detected, { mac_6_0 });
    duplicate_ipv4_address.Add(inet_addr("192.168.100.11"), DuplicateStatus::Cleared, { mac_8_0 });
    duplicate_ipv4_address.Add(inet_addr("192.168.100.11"), DuplicateStatus::Cleared, { mac_8_0 });
    duplicate_ipv4_address.Add(inet_addr("192.168.100.11"), DuplicateStatus::Cleared, { mac_8_0 });
    CHECK_FALSE(duplicate_ipv4_address.Empty());

    duplicate_ipv4_address.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };
    ReadableDuplicateIPv4AdrArray read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_FALSE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(2, read_data.Get().size());

    CHECK_EQUAL(inet_addr("192.168.100.10"), read_data.Get()[0]->IPAddress);
    CHECK_EQUAL(DuplicateStatus::Detected, read_data.Get()[0]->Status);
    CHECK_EQUAL(6, read_data.Get()[0]->MACAddress.Length);
    MEMCMP_EQUAL(mac_6_0, (char *)read_data.Get()[0]->MACAddress.MACAddresses, sizeof(mac_6_0));

    CHECK_EQUAL(inet_addr("192.168.100.11"), read_data.Get()[1]->IPAddress);
    CHECK_EQUAL(DuplicateStatus::Cleared, read_data.Get()[1]->Status);
    CHECK_EQUAL(8, read_data.Get()[1]->MACAddress.Length);
    MEMCMP_EQUAL(mac_8_0, (char *)read_data.Get()[1]->MACAddress.MACAddresses, sizeof(mac_8_0));
}