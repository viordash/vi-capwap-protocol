#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/DecryptionErrorReport.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(DecryptionErrorReportTestsGroup){ //
                                             TEST_SETUP(){}

                                             TEST_TEARDOWN(){}
};

TEST(DecryptionErrorReportTestsGroup, DecryptionErrorReport_serialize) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    const uint8_t mac_6_0[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    const uint8_t mac_6_1[] = { 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD };
    const uint8_t mac_8_0[] = { 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD, 0xEE, 0xFF };
    const uint8_t mac_8_1[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0xEE, 0xFF };

    WritableDecryptionErrorReportArray decryption_error_report;
    decryption_error_report.Add(12, { { mac_6_0 }, { mac_8_0 }, { mac_8_1 } });
    decryption_error_report.Add(19, { { mac_6_1 } });
    CHECK_FALSE(decryption_error_report.Empty());

    decryption_error_report.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 44, raw_data.current);
    const uint8_t reference[] = {
        0x00, 0x0F, 0x00, 0x1B, 0x0C, 0x03, 0x06, 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x08, 0xAA,
        0xBB, 0xCC, 0xFF, 0xFE, 0xDD, 0xEE, 0xFF, 0x08, 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0xEE,
        0xFF, 0x00, 0x0F, 0x00, 0x09, 0x13, 0x01, 0x06, 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD
    };

    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(reference) };
    ReadableDecryptionErrorReportArray read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(&buffer[0] + sizeof(reference), raw_data.current);
    CHECK_EQUAL(2, read_data.Get().size());

    const auto &item0 = read_data.Get()[0];
    CHECK_EQUAL(12, item0.GetRadioID());
    CHECK_EQUAL(3, item0.Get().size());
    CHECK_EQUAL(6, item0.Get()[0]->Length);
    MEMCMP_EQUAL(mac_6_0, (char *)item0.Get()[0]->MACAddresses, sizeof(mac_6_0));
    CHECK_EQUAL(8, item0.Get()[1]->Length);
    MEMCMP_EQUAL(mac_8_0, (char *)item0.Get()[1]->MACAddresses, sizeof(mac_8_0));
    CHECK_EQUAL(8, item0.Get()[2]->Length);
    MEMCMP_EQUAL(mac_8_1, (char *)item0.Get()[2]->MACAddresses, sizeof(mac_8_1));

    const auto &item1 = read_data.Get()[1];
    CHECK_EQUAL(19, item1.GetRadioID());
    CHECK_EQUAL(1, item1.Get().size());
    CHECK_EQUAL(6, item1.Get()[0]->Length);
    MEMCMP_EQUAL(mac_6_1, (char *)item1.Get()[0]->MACAddresses, sizeof(mac_6_1));
}

TEST(DecryptionErrorReportTestsGroup, DecryptionErrorReport_deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Заголовок элемента сообщения (4 байта) ----
        0x00, 0x0F,     // Type: 15 (Decryption Error Report)
        0x00, 0x10,     // Length: 16 байт (1 + 1 + 7 + 7 = 16) - это длина последующих данных

        // ---- Данные (Value) (16 байт) ----
        0x02,           // Radio ID: 2 (например, радиомодуль 5 ГГц)
        0x02,           // Num of Entries: 2 (сообщаем о двух станциях)

        // --- Запись №1 (7 байт) ---
        0x06,           // Length: 6 (длина стандартного MAC-адреса EUI-48)
        0x11, 0x22,     // MAC Address: 11:22:33:44:55:66
        0x33, 0x44,
        0x55, 0x66,

        // --- Запись №2 (7 байт) ---
        0x06,           // Length: 6
        0xAA, 0xBB,     // MAC Address: AA:BB:CC:DD:EE:FF
        0xCC, 0xDD,
        0xEE, 0xFF
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };

    ReadableDecryptionErrorReportArray read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    const uint8_t mac_6_0[] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
    const uint8_t mac_6_1[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(1, read_data.Get().size());

    const auto &item0 = read_data.Get()[0];
    CHECK_EQUAL(2, item0.GetRadioID());
    CHECK_EQUAL(2, item0.Get().size());
    CHECK_EQUAL(6, item0.Get()[0]->Length);
    MEMCMP_EQUAL(mac_6_0, (char *)item0.Get()[0]->MACAddresses, sizeof(mac_6_0));
    CHECK_EQUAL(6, item0.Get()[1]->Length);
    MEMCMP_EQUAL(mac_6_1, (char *)item0.Get()[1]->MACAddresses, sizeof(mac_6_1));
}

TEST(DecryptionErrorReportTestsGroup, Add_array_of_items_is_unique) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    const uint8_t mac_6_0[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    const uint8_t mac_6_1[] = { 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD };
    const uint8_t mac_8_0[] = { 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD, 0xEE, 0xFF };
    const uint8_t mac_8_1[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0xEE, 0xFF };

    WritableDecryptionErrorReportArray decryption_error_report;
    decryption_error_report.Add(12, { { mac_6_0 }, { mac_8_0 }, { mac_8_1 } });
    decryption_error_report.Add(12, { { mac_6_0 }, { mac_8_0 }, { mac_8_1 } });
    decryption_error_report.Add(12, { { mac_6_0 }, { mac_8_0 }, { mac_8_1 } });
    decryption_error_report.Add(19, { { mac_6_1 } });
    decryption_error_report.Add(19, { { mac_6_1 } });
    decryption_error_report.Add(19, { { mac_6_1 } });

    decryption_error_report.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableDecryptionErrorReportArray read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_FALSE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(2, read_data.Get().size());

    const auto &item0 = read_data.Get()[0];
    CHECK_EQUAL(12, item0.GetRadioID());
    CHECK_EQUAL(3, item0.Get().size());
    CHECK_EQUAL(6, item0.Get()[0]->Length);
    MEMCMP_EQUAL(mac_6_0, (char *)item0.Get()[0]->MACAddresses, sizeof(mac_6_0));
    CHECK_EQUAL(8, item0.Get()[1]->Length);
    MEMCMP_EQUAL(mac_8_0, (char *)item0.Get()[1]->MACAddresses, sizeof(mac_8_0));
    CHECK_EQUAL(8, item0.Get()[2]->Length);
    MEMCMP_EQUAL(mac_8_1, (char *)item0.Get()[2]->MACAddresses, sizeof(mac_8_1));

    const auto &item1 = read_data.Get()[1];
    CHECK_EQUAL(19, item1.GetRadioID());
    CHECK_EQUAL(1, item1.Get().size());
    CHECK_EQUAL(6, item1.Get()[0]->Length);
    MEMCMP_EQUAL(mac_6_1, (char *)item1.Get()[0]->MACAddresses, sizeof(mac_6_1));
}