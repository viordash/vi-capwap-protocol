#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/DeleteStation.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(DeleteStationTestsGroup){ //
                                     TEST_SETUP(){}

                                     TEST_TEARDOWN(){}
};

TEST(DeleteStationTestsGroup, DeleteStation_serialize) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    const uint8_t mac_6_0[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    const uint8_t mac_8_0[] = { 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD, 0xEE, 0xFF };

    WritableDeleteStationArray delete_station_arr;
    delete_station_arr.Add(7, { mac_6_0 });
    delete_station_arr.Add(8, { mac_8_0 });

    delete_station_arr.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 26, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x12, 0x00, 0x08, 0x07, 0x06, 0x00, 0x1A, 0x2B,
                                  0x3C, 0x4D, 0x5E, 0x00, 0x12, 0x00, 0x0A, 0x08, 0x08,
                                  0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD, 0xEE, 0xFF };

    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(reference) };
    ReadableDeleteStationArray read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(&buffer[0] + sizeof(reference), raw_data.current);

    CHECK_EQUAL(2, read_data.Get().size());

    CHECK_EQUAL(7, read_data.Get()[0]->RadioID);
    CHECK_EQUAL(6, read_data.Get()[0]->MACAddress.Length);
    MEMCMP_EQUAL(mac_6_0, (char *)read_data.Get()[0]->MACAddress.MACAddresses, sizeof(mac_6_0));

    CHECK_EQUAL(8, read_data.Get()[1]->RadioID);
    CHECK_EQUAL(8, read_data.Get()[1]->MACAddress.Length);
    MEMCMP_EQUAL(mac_8_0, (char *)read_data.Get()[1]->MACAddress.MACAddresses, sizeof(mac_8_0));
}

TEST(DeleteStationTestsGroup, DeleteStation_deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Заголовок элемента сообщения (4 байта) ----
        0x00, 0x12,     // Type: 18 (Delete Station)
        0x00, 0x08,     // Length: 8 байт (1 + 1 + 6 = 8) - это длина последующих данных

        // ---- Данные (Value) (8 байт) ----
        0x02,           // Radio ID: 2 (радиомодуль, к которому был подключен клиент)

        0x06,           // Length: 6 (длина стандартного MAC-адреса EUI-48)

        0xAA, 0xBB,     // MAC Address: AA:BB:CC:DD:EE:FF
        0xCC, 0xDD,     // (MAC-адрес клиента, которого нужно удалить)
        0xEE, 0xFF
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };

    ReadableDeleteStationArray read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(1, read_data.Get().size());

    CHECK_EQUAL(2, read_data.Get()[0]->RadioID);
    CHECK_EQUAL(6, read_data.Get()[0]->MACAddress.Length);
    const uint8_t mac_6_0[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    MEMCMP_EQUAL(mac_6_0, (char *)read_data.Get()[0]->MACAddress.MACAddresses, sizeof(mac_6_0));
}

TEST(DeleteStationTestsGroup, Add_array_of_items_is_unique) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    const uint8_t mac_6_0[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    const uint8_t mac_8_0[] = { 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD, 0xEE, 0xFF };

    WritableDeleteStationArray delete_station_arr;
    delete_station_arr.Add(7, { mac_6_0 });
    delete_station_arr.Add(7, { mac_6_0 });
    delete_station_arr.Add(7, { mac_6_0 });
    delete_station_arr.Add(8, { mac_8_0 });
    delete_station_arr.Add(8, { mac_8_0 });
    delete_station_arr.Add(8, { mac_8_0 });

    delete_station_arr.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };
    ReadableDeleteStationArray read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_FALSE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(2, read_data.Get().size());

    CHECK_EQUAL(7, read_data.Get()[0]->RadioID);
    CHECK_EQUAL(6, read_data.Get()[0]->MACAddress.Length);
    MEMCMP_EQUAL(mac_6_0, (char *)read_data.Get()[0]->MACAddress.MACAddresses, sizeof(mac_6_0));

    CHECK_EQUAL(8, read_data.Get()[1]->RadioID);
    CHECK_EQUAL(8, read_data.Get()[1]->MACAddress.Length);
    MEMCMP_EQUAL(mac_8_0, (char *)read_data.Get()[1]->MACAddress.MACAddresses, sizeof(mac_8_0));
}