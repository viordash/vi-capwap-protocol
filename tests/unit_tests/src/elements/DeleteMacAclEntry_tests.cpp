#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/DeleteMacAclEntry.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(DeleteMacAclEntryTestsGroup){ //
                                         TEST_SETUP(){}

                                         TEST_TEARDOWN(){}
};

TEST(DeleteMacAclEntryTestsGroup, DeleteMacAclEntry_serialize) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    const uint8_t mac_6_0[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    const uint8_t mac_6_1[] = { 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD };
    const uint8_t mac_8_0[] = { 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD, 0xEE, 0xFF };
    const uint8_t mac_8_1[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0xEE, 0xFF };

    WritableDeleteMacAclEntry del_mac_acl_entry;
    del_mac_acl_entry.Add({ mac_6_0 });
    del_mac_acl_entry.Add({ mac_8_0 });
    del_mac_acl_entry.Add({ mac_8_1 });
    del_mac_acl_entry.Add({ mac_6_1 });

    del_mac_acl_entry.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 37, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x11, 0x00, 0x21, 0x04, 0x06, 0x00, 0x1A, 0x2B, 0x3C,
                                  0x4D, 0x5E, 0x08, 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD, 0xEE,
                                  0xFF, 0x08, 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0xEE, 0xFF,
                                  0x06, 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD };

    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(reference) };
    ReadableDeleteMacAclEntry read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(&buffer[0] + sizeof(reference), raw_data.current);
    CHECK_EQUAL(4, read_data.Get().size());
    CHECK_EQUAL(6, read_data.Get()[0]->Length);
    MEMCMP_EQUAL(mac_6_0, (char *)read_data.Get()[0]->MACAddresses, sizeof(mac_6_0));
    CHECK_EQUAL(8, read_data.Get()[1]->Length);
    MEMCMP_EQUAL(mac_8_0, (char *)read_data.Get()[1]->MACAddresses, sizeof(mac_8_0));
    CHECK_EQUAL(8, read_data.Get()[2]->Length);
    MEMCMP_EQUAL(mac_8_1, (char *)read_data.Get()[2]->MACAddresses, sizeof(mac_8_1));
    CHECK_EQUAL(6, read_data.Get()[3]->Length);
    MEMCMP_EQUAL(mac_6_1, (char *)read_data.Get()[3]->MACAddresses, sizeof(mac_6_1));
}

TEST(DeleteMacAclEntryTestsGroup, DeleteMacAclEntry_deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Заголовок элемента сообщения (4 байта) ----
        0x00, 0x11,     // Type: 17 (Delete MAC ACL Entry)
        0x00, 0x0F,     // Length: 15 байт (1 + 1 + 6 + 1 + 6 = 15) - это длина последующих данных

        // ---- Данные (Value) (15 байт) ----
        0x02,           // Num of Entries: 2 (1 байт) - указываем, что в списке два MAC-адреса

        // --- Запись №1 (7 байт) ---
        0x06,           // Length: 6 (длина стандартного MAC-адреса)
        0x00, 0x1A,     // MAC Address: 00:1A:2B:3C:4D:5E
        0x2B, 0x3C,
        0x4D, 0x5E,

        // --- Запись №2 (7 байт) ---
        0x06,           // Length: 6
        0xAA, 0xBB,     // MAC Address: AA:BB:CC:DD:EE:FF
        0xCC, 0xDD,
        0xEE, 0xFF
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };

    ReadableDeleteMacAclEntry read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    const uint8_t mac_6_0[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    const uint8_t mac_6_1[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, read_data.Get().size());
    CHECK_EQUAL(6, read_data.Get()[0]->Length);
    MEMCMP_EQUAL(mac_6_0, (char *)read_data.Get()[0]->MACAddresses, sizeof(mac_6_0));
    CHECK_EQUAL(6, read_data.Get()[1]->Length);
    MEMCMP_EQUAL(mac_6_1, (char *)read_data.Get()[1]->MACAddresses, sizeof(mac_6_1));
}

TEST(DeleteMacAclEntryTestsGroup, Add_array_of_items_is_unique) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    const uint8_t mac_6_0[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    const uint8_t mac_8_0[] = { 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD, 0xEE, 0xFF };

    WritableDeleteMacAclEntry del_mac_acl_entry;
    del_mac_acl_entry.Add({ mac_6_0 });
    del_mac_acl_entry.Add({ mac_6_0 });
    del_mac_acl_entry.Add({ mac_6_0 });
    del_mac_acl_entry.Add({ mac_8_0 });
    del_mac_acl_entry.Add({ mac_8_0 });
    del_mac_acl_entry.Add({ mac_8_0 });

    del_mac_acl_entry.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableDeleteMacAclEntry read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(2, read_data.Get().size());
    CHECK_EQUAL(6, read_data.Get()[0]->Length);
    MEMCMP_EQUAL(mac_6_0, (char *)read_data.Get()[0]->MACAddresses, sizeof(mac_6_0));
    CHECK_EQUAL(8, read_data.Get()[1]->Length);
    MEMCMP_EQUAL(mac_8_0, (char *)read_data.Get()[1]->MACAddresses, sizeof(mac_8_0));
}