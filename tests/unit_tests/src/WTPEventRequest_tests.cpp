#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "WTPEventRequest.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(WTPEventRequestTestsGroup){ //
                                       TEST_SETUP(){}

                                       TEST_TEARDOWN(){}
};

TEST(WTPEventRequestTestsGroup, WTPEventRequest_serialize) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    const uint8_t mac_6_0[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    const uint8_t mac_6_1[] = { 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD };
    const uint8_t mac_6_2[] = { 0xAB, 0xBC, 0xCD, 0xDE, 0xEF, 0xFF };
    const uint8_t mac_8_0[] = { 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD, 0xEE, 0xFF };
    const uint8_t mac_8_1[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0xEE, 0xFF };

    WritableDecryptionErrorReportArray decryption_error_report;
    decryption_error_report.Add(3, { { mac_6_0 }, { mac_8_0 } });

    WritableDuplicateIPv4AdrArray duplicate_ipv4_address;
    duplicate_ipv4_address.Add(inet_addr("192.168.100.10"), DuplicateStatus::Detected, { mac_6_1 });
    duplicate_ipv4_address.Add(inet_addr("192.168.100.11"), DuplicateStatus::Cleared, { mac_8_1 });

    WritableWTPRadioStatisticsArray wtp_radio_statistics;
    wtp_radio_statistics.Add({ 1,
                               WTPRadioStatistics::LastFailureType::HardwareFailure,
                               40,
                               41,
                               42,
                               43,
                               44,
                               45,
                               46,
                               47,
                               -48 });

    std::optional<WTPRebootStatistics> wtp_reboot_statistics{
        std::in_place, 12340, 12341,
        12342,         12343, 12344,
        12345,         12346, WTPRebootStatistics::LastFailureType::LinkFailure
    };

    WritableDeleteStationArray delete_station;
    delete_station.Add(7, { mac_6_2 });
    delete_station.Add(8, { mac_6_0 });

    WritableVendorSpecificPayloadArray vendor_specific_payloads;
    vendor_specific_payloads.Add(123456, 789, "01234567890ABCDEF0123");

    WritableWTPEventRequest write_data(decryption_error_report,
                                       duplicate_ipv4_address,
                                       wtp_radio_statistics,
                                       wtp_reboot_statistics,
                                       delete_station,
                                       vendor_specific_payloads);

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 170 - (sizeof(ClearHeader) + sizeof(ControlHeader)), raw_data.current);
    const uint8_t reference[] = {
        0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x2A, 0x00, 0x9A,
        0x00, 0x00, 0x0F, 0x00, 0x12, 0x03, 0x02, 0x06, 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x08,
        0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD, 0xEE, 0xFF, 0x00, 0x15, 0x00, 0x0C, 0xC0, 0xA8, 0x64,
        0x0A, 0x01, 0x06, 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD, 0x00, 0x15, 0x00, 0x0E, 0xC0, 0xA8,
        0x64, 0x0B, 0x00, 0x08, 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0xEE, 0xFF, 0x00, 0x2F, 0x00,
        0x14, 0x01, 0x02, 0x00, 0x28, 0x00, 0x29, 0x00, 0x2A, 0x00, 0x2B, 0x00, 0x2C, 0x00, 0x2D,
        0x00, 0x2E, 0x00, 0x2F, 0xFF, 0xD0, 0x00, 0x30, 0x00, 0x0F, 0x30, 0x34, 0x30, 0x35, 0x30,
        0x36, 0x30, 0x37, 0x30, 0x38, 0x30, 0x39, 0x30, 0x3A, 0x02, 0x00, 0x12, 0x00, 0x08, 0x07,
        0x06, 0xAB, 0xBC, 0xCD, 0xDE, 0xEF, 0xFF, 0x00, 0x12, 0x00, 0x08, 0x08, 0x06, 0x00, 0x1A,
        0x2B, 0x3C, 0x4D, 0x5E, 0x00, 0x25, 0x00, 0x1B, 0x00, 0x01, 0xE2, 0x40, 0x03, 0x15, 0x30,
        0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45,
        0x46, 0x30, 0x31, 0x32, 0x33
    };

    MEMCMP_EQUAL(buffer,
                 reference + (sizeof(ClearHeader) + sizeof(ControlHeader)),
                 sizeof(reference) - (sizeof(ClearHeader) + sizeof(ControlHeader)));

    raw_data = { buffer, buffer + 170 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };
    ReadableWTPEventRequest read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(1, read_data.decryption_error_report.Get().size());
    CHECK_EQUAL(3, read_data.decryption_error_report.Get()[0].GetRadioID());
    CHECK_EQUAL(2, read_data.decryption_error_report.Get()[0].Get().size());
    CHECK_EQUAL(6, read_data.decryption_error_report.Get()[0].Get()[0]->Length);
    MEMCMP_EQUAL(mac_6_0,
                 (char *)read_data.decryption_error_report.Get()[0].Get()[0]->MACAddresses,
                 sizeof(mac_6_0));
    CHECK_EQUAL(8, read_data.decryption_error_report.Get()[0].Get()[1]->Length);
    MEMCMP_EQUAL(mac_8_0,
                 (char *)read_data.decryption_error_report.Get()[0].Get()[1]->MACAddresses,
                 sizeof(mac_8_0));

    CHECK_EQUAL(2, read_data.duplicate_ipv4_address.Get().size());

    CHECK_EQUAL(inet_addr("192.168.100.10"), read_data.duplicate_ipv4_address.Get()[0]->IPAddress);
    CHECK_EQUAL(DuplicateStatus::Detected, read_data.duplicate_ipv4_address.Get()[0]->Status);
    CHECK_EQUAL(6, read_data.duplicate_ipv4_address.Get()[0]->MACAddress.Length);
    MEMCMP_EQUAL(mac_6_1,
                 (char *)read_data.duplicate_ipv4_address.Get()[0]->MACAddress.MACAddresses,
                 sizeof(mac_6_1));

    CHECK_EQUAL(inet_addr("192.168.100.11"), read_data.duplicate_ipv4_address.Get()[1]->IPAddress);
    CHECK_EQUAL(DuplicateStatus::Cleared, read_data.duplicate_ipv4_address.Get()[1]->Status);
    CHECK_EQUAL(8, read_data.duplicate_ipv4_address.Get()[1]->MACAddress.Length);
    MEMCMP_EQUAL(mac_8_1,
                 (char *)read_data.duplicate_ipv4_address.Get()[1]->MACAddress.MACAddresses,
                 sizeof(mac_8_1));

    CHECK_EQUAL(1, read_data.wtp_radio_statistics.Get().size());
    CHECK_EQUAL(ElementHeader::ElementType::WTPRadioStatistics,
                read_data.wtp_radio_statistics.Get()[0]->GetElementType());
    CHECK_EQUAL(1, read_data.wtp_radio_statistics.Get()[0]->GetRadioID());
    CHECK_EQUAL(WTPRadioStatistics::LastFailureType::HardwareFailure,
                read_data.wtp_radio_statistics.Get()[0]->GetLastFailureType());
    CHECK_EQUAL(40, read_data.wtp_radio_statistics.Get()[0]->GetResetCount());
    CHECK_EQUAL(41, read_data.wtp_radio_statistics.Get()[0]->GetSWFailureCount());
    CHECK_EQUAL(42, read_data.wtp_radio_statistics.Get()[0]->GetHWFailureCount());
    CHECK_EQUAL(43, read_data.wtp_radio_statistics.Get()[0]->GetOtherFailureCount());
    CHECK_EQUAL(44, read_data.wtp_radio_statistics.Get()[0]->GetUnknownFailureCount());
    CHECK_EQUAL(45, read_data.wtp_radio_statistics.Get()[0]->GetConfigUpdateCount());
    CHECK_EQUAL(46, read_data.wtp_radio_statistics.Get()[0]->GetChannelChangeCount());
    CHECK_EQUAL(47, read_data.wtp_radio_statistics.Get()[0]->GetBandChangeCount());
    CHECK_EQUAL(-48, read_data.wtp_radio_statistics.Get()[0]->GetCurrentNoiseFloor());

    CHECK(read_data.wtp_reboot_statistics != nullptr);
    CHECK_EQUAL(12340, read_data.wtp_reboot_statistics->GetRebootCount());
    CHECK_EQUAL(12341, read_data.wtp_reboot_statistics->GetACInitiatedCount());
    CHECK_EQUAL(12342, read_data.wtp_reboot_statistics->GetLinkFailureCount());
    CHECK_EQUAL(12343, read_data.wtp_reboot_statistics->GetSWFailureCount());
    CHECK_EQUAL(12344, read_data.wtp_reboot_statistics->GetHWFailureCount());
    CHECK_EQUAL(12345, read_data.wtp_reboot_statistics->GetOtherFailureCount());
    CHECK_EQUAL(12346, read_data.wtp_reboot_statistics->GetUnknownFailureCount());
    CHECK_EQUAL(WTPRebootStatistics::LastFailureType::LinkFailure,
                read_data.wtp_reboot_statistics->GetLastFailureType());

    CHECK_EQUAL(2, read_data.delete_station.Get().size());
    CHECK_EQUAL(7, read_data.delete_station.Get()[0]->RadioID);
    CHECK_EQUAL(6, read_data.delete_station.Get()[0]->MACAddress.Length);
    MEMCMP_EQUAL(mac_6_2,
                 (char *)read_data.delete_station.Get()[0]->MACAddress.MACAddresses,
                 sizeof(mac_6_2));
    CHECK_EQUAL(8, read_data.delete_station.Get()[1]->RadioID);
    CHECK_EQUAL(6, read_data.delete_station.Get()[1]->MACAddress.Length);
    MEMCMP_EQUAL(mac_6_0,
                 (char *)read_data.delete_station.Get()[1]->MACAddress.MACAddresses,
                 sizeof(mac_6_0));

    CHECK_EQUAL(1, read_data.vendor_specific_payloads.Get().size());
    CHECK_EQUAL(123456, read_data.vendor_specific_payloads.Get()[0]->GetVendorIdentifier());
    CHECK_EQUAL(789, read_data.vendor_specific_payloads.Get()[0]->GetElementId());
    STRNCMP_EQUAL("01234567890ABCDEF0123",
                  (char *)read_data.vendor_specific_payloads.Get()[0]->value,
                  21);
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(WTPEventRequestTestsGroup, WTPEventRequest_deserialize_image_data) {
    // clang-format off
    uint8_t data[] = {
        // ---- CAPWAP Header (8 байт) ----
        0x00,       // Preamble: Version 0, Type 0 (CAPWAP Header follows)
        0x10,       // HLEN: 2 (8 bytes), RID high bits
        0x42,       // RID low bits, WBID: 1 (802.11), T=0
        0x00,       // F=0, L=0, W=0, M=0, K=0, Flags=0
        0x00, 0x00, // Fragment ID
        0x00, 0x00, // Fragment Offset

        // =================================================================
        // 2. Control Message Header (8 байт)
        // =================================================================
        0x00, 0x00,
        0x00, 0x09,     // Message Type: 9 (WTP Event Request)
        0x3C,           // Sequence Number: 0x3C (60)
        0x00, 0x3F,     // Message Element Length: 63 байта (13 + 24 + 12 + 14 = 63)
        0x00,           // Flags: 0

        // =================================================================
        // 3. Message Elements (63 байта)
        // =================================================================

        // --- Элемент 1: Decryption Error Report (Type 15) - 13 байт ---
        // Назначение: Сообщить об ошибке расшифровки для одного клиента на радио #1.
        0x00, 0x0F,     // Type: 15
        0x00, 0x09,     // Length: 9 байт (1+1+7=9)
        0x01,           // Value: Radio ID = 1 (например, 2.4 ГГц)
        0x01,           // Value: Num of Entries = 1
        0x06,           // Value: Длина MAC-адреса = 6
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66, // Value: MAC = 11:22:33:44:55:66

        // --- Элемент 2: WTP Radio Statistics (Type 47) - 24 байта ---
        // Назначение: Отправить периодическую статистику для радио #2.
        0x00, 0x2F,     // Type: 47
        0x00, 0x14,     // Length: 20 байт (фиксированная длина для этого элемента)
        0x02,           // Value: Radio ID = 2 (например, 5 ГГц)
        0x01,           // Value: Last Fail Type = 1 (Software Failure)
        0x00, 0x05,     // Value: Reset Count = 5
        0x00, 0x02,     // Value: SW Failure Count = 2
        0x00, 0x00,     // Value: HW Failure Count = 0
        0x00, 0x00,     // Value: Other Failure Count = 0
        0x00, 0x01,     // Value: Unknown Failure Count = 1
        0x00, 0x14,     // Value: Config Update Count = 20
        0x00, 0x08,     // Value: Channel Change Count = 8
        0x00, 0x00,     // Value: Band Change Count = 0
        0xFF, 0xA1,     // Value: Current Noise Floor = -95 dBm

        // --- Элемент 3: Delete Station (Type 18) - 12 байт ---
        // Назначение: Сообщить об удалении клиента по тайм-ауту с радио #1.
        0x00, 0x12,     // Type: 18
        0x00, 0x08,     // Length: 8 байт (1+1+6=8)
        0x01,           // Value: Radio ID = 1 (2.4 ГГц)
        0x06,           // Value: Длина MAC-адреса = 6
        0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE, // Value: MAC = DE:AD:BE:EF:CA:FE

        // --- Элемент 4: Vendor Specific Payload (Type 37) - 14 байт ---
        // Назначение: Отправить кастомное событие (например, "обнаружен радар DFS").
        0x00, 0x25,     // Type: 37
        0x00, 0x0A,     // Length: 10 байт (4+2+4=10)
        0x00, 0x00, 0x00, 0x09, // Value: Vendor Identifier = 9 (Cisco)
        0x10, 0x01,     // Value: Element ID = 0x1001 (условный ID для DFS события)
        0x01, 0x02, 0x03, 0x04  // Value: Данные события (например, канал и ширина)
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableWTPEventRequest read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);

    CHECK_EQUAL(1, read_data.decryption_error_report.Get().size());
    CHECK_EQUAL(1, read_data.decryption_error_report.Get()[0].GetRadioID());
    CHECK_EQUAL(1, read_data.decryption_error_report.Get()[0].Get().size());
    const uint8_t mac_decryption_error_report[] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
    CHECK_EQUAL(6, read_data.decryption_error_report.Get()[0].Get()[0]->Length);
    MEMCMP_EQUAL(mac_decryption_error_report,
                 (char *)read_data.decryption_error_report.Get()[0].Get()[0]->MACAddresses,
                 sizeof(mac_decryption_error_report));

    CHECK_EQUAL(1, read_data.wtp_radio_statistics.Get().size());
    CHECK_EQUAL(ElementHeader::ElementType::WTPRadioStatistics,
                read_data.wtp_radio_statistics.Get()[0]->GetElementType());
    CHECK_EQUAL(2, read_data.wtp_radio_statistics.Get()[0]->GetRadioID());
    CHECK_EQUAL(WTPRadioStatistics::LastFailureType::SoftwareFailure,
                read_data.wtp_radio_statistics.Get()[0]->GetLastFailureType());
    CHECK_EQUAL(5, read_data.wtp_radio_statistics.Get()[0]->GetResetCount());
    CHECK_EQUAL(2, read_data.wtp_radio_statistics.Get()[0]->GetSWFailureCount());
    CHECK_EQUAL(0, read_data.wtp_radio_statistics.Get()[0]->GetHWFailureCount());
    CHECK_EQUAL(0, read_data.wtp_radio_statistics.Get()[0]->GetOtherFailureCount());
    CHECK_EQUAL(1, read_data.wtp_radio_statistics.Get()[0]->GetUnknownFailureCount());
    CHECK_EQUAL(20, read_data.wtp_radio_statistics.Get()[0]->GetConfigUpdateCount());
    CHECK_EQUAL(8, read_data.wtp_radio_statistics.Get()[0]->GetChannelChangeCount());
    CHECK_EQUAL(0, read_data.wtp_radio_statistics.Get()[0]->GetBandChangeCount());
    CHECK_EQUAL(-95, read_data.wtp_radio_statistics.Get()[0]->GetCurrentNoiseFloor());

    CHECK(read_data.wtp_reboot_statistics == nullptr);

    CHECK_EQUAL(1, read_data.delete_station.Get().size());
    CHECK_EQUAL(1, read_data.delete_station.Get()[0]->RadioID);
    const uint8_t mac_delete_station[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE };
    CHECK_EQUAL(6, read_data.delete_station.Get()[0]->MACAddress.Length);
    MEMCMP_EQUAL(mac_delete_station,
                 (char *)read_data.delete_station.Get()[0]->MACAddress.MACAddresses,
                 sizeof(mac_delete_station));

    CHECK_EQUAL(1, read_data.vendor_specific_payloads.Get().size());
    CHECK_EQUAL(9, read_data.vendor_specific_payloads.Get()[0]->GetVendorIdentifier());
    CHECK_EQUAL(0x1001, read_data.vendor_specific_payloads.Get()[0]->GetElementId());
    const uint8_t vendor_payload[] = { 0x01, 0x02, 0x03, 0x04 };
    MEMCMP_EQUAL(vendor_payload,
                 read_data.vendor_specific_payloads.Get()[0]->value,
                 sizeof(vendor_payload));
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(WTPEventRequestTestsGroup, WTPEventRequest_deserialize_handle_unknown_element) {
    // clang-format off
    uint8_t data[] = {
        // ---- CAPWAP Header (8 байт) ----
        0x00,       // Preamble: Version 0, Type 0 (CAPWAP Header follows)
        0x10,       // HLEN: 2 (8 bytes), RID high bits
        0x42,       // RID low bits, WBID: 1 (802.11), T=0
        0x00,       // F=0, L=0, W=0, M=0, K=0, Flags=0
        0x00, 0x00, // Fragment ID
        0x00, 0x00, // Fragment Offset

        // =================================================================
        // 2. Control Message Header (8 байт)
        // =================================================================
        0x00, 0x00,
        0x00, 0x09,     // Message Type: 9 (WTP Event Request)
        0x3C,           // Sequence Number: 0x3C (60)
        0x00, 23,     // Message Element Length: 23 байта
        0x00,           // Flags: 0

        // =================================================================
        // 3. Message Elements (63 байта)
        // =================================================================

        // --- Элемент 1: Decryption Error Report (Type 15) - 13 байт ---
        // Назначение: Сообщить об ошибке расшифровки для одного клиента на радио #1.
        0x00, 0x0F,     // Type: 15
        0x00, 0x09,     // Length: 9 байт (1+1+7=9)
        0x01,           // Value: Radio ID = 1 (например, 2.4 ГГц)
        0x01,           // Value: Num of Entries = 1
        0x06,           // Value: Длина MAC-адреса = 6
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66, // Value: MAC = 11:22:33:44:55:66

        // 6. Unknown (5 байт)
        0xFF, 0xFF, 0x00, 0x01, 0x00,
        // 7. Unknown (5 байт)
        0xFF, 0xFE, 0x00, 0x01, 0x00,
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableWTPEventRequest read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, read_data.unknown_elements);
}

TEST(WTPEventRequestTestsGroup, Serialization_clears_items) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    const uint8_t mac_6_0[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    const uint8_t mac_6_1[] = { 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD };
    const uint8_t mac_6_2[] = { 0xAB, 0xBC, 0xCD, 0xDE, 0xEF, 0xFF };
    const uint8_t mac_8_0[] = { 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD, 0xEE, 0xFF };
    const uint8_t mac_8_1[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0xEE, 0xFF };

    WritableDecryptionErrorReportArray decryption_error_report;
    decryption_error_report.Add(3, { { mac_6_0 }, { mac_8_0 } });

    WritableDuplicateIPv4AdrArray duplicate_ipv4_address;
    duplicate_ipv4_address.Add(inet_addr("192.168.100.10"), DuplicateStatus::Detected, { mac_6_1 });
    duplicate_ipv4_address.Add(inet_addr("192.168.100.11"), DuplicateStatus::Cleared, { mac_8_1 });

    WritableWTPRadioStatisticsArray wtp_radio_statistics;
    wtp_radio_statistics.Add({ 1,
                               WTPRadioStatistics::LastFailureType::HardwareFailure,
                               40,
                               41,
                               42,
                               43,
                               44,
                               45,
                               46,
                               47,
                               -48 });

    std::optional<WTPRebootStatistics> wtp_reboot_statistics{
        std::in_place, 12340, 12341,
        12342,         12343, 12344,
        12345,         12346, WTPRebootStatistics::LastFailureType::LinkFailure
    };

    WritableDeleteStationArray delete_station;
    delete_station.Add(7, { mac_6_2 });
    delete_station.Add(8, { mac_6_0 });

    WritableVendorSpecificPayloadArray vendor_specific_payloads;
    vendor_specific_payloads.Add(123456, 789, "01234567890ABCDEF0123");

    WritableWTPEventRequest write_data(decryption_error_report,
                                       duplicate_ipv4_address,
                                       wtp_radio_statistics,
                                       wtp_reboot_statistics,
                                       delete_station,
                                       vendor_specific_payloads);

    CHECK_FALSE(decryption_error_report.Empty());
    CHECK_FALSE(duplicate_ipv4_address.Empty());
    CHECK_FALSE(wtp_radio_statistics.Empty());

    CHECK_TRUE(wtp_reboot_statistics.has_value());

    CHECK_FALSE(delete_station.Empty());
    CHECK_FALSE(vendor_specific_payloads.Empty());

    write_data.Serialize(&raw_data);
    write_data.Clear();
    CHECK_TRUE(decryption_error_report.Empty());
    CHECK_TRUE(duplicate_ipv4_address.Empty());
    CHECK_TRUE(wtp_radio_statistics.Empty());

    CHECK_FALSE(wtp_reboot_statistics.has_value());

    CHECK_TRUE(delete_station.Empty());
    CHECK_TRUE(vendor_specific_payloads.Empty());
}

TEST(WTPEventRequestTestsGroup, One_or_more_serialize) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    const uint8_t mac_6_0[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };

    WritableDecryptionErrorReportArray decryption_error_report;
    decryption_error_report.Add(3, { { mac_6_0 } });

    WritableDuplicateIPv4AdrArray duplicate_ipv4_address;
    WritableWTPRadioStatisticsArray wtp_radio_statistics;
    std::optional<WTPRebootStatistics> wtp_reboot_statistics;
    WritableDeleteStationArray delete_station;
    WritableVendorSpecificPayloadArray vendor_specific_payloads;

    WritableWTPEventRequest write_data(decryption_error_report,
                                       duplicate_ipv4_address,
                                       wtp_radio_statistics,
                                       wtp_reboot_statistics,
                                       delete_station,
                                       vendor_specific_payloads);

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 13, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x0F, 0x00, 0x09, 0x03, 0x01, 0x06,
                                  0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };

    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + 13 };
    ReadableWTPEventRequest read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(1, read_data.decryption_error_report.Get().size());
    CHECK_EQUAL(3, read_data.decryption_error_report.Get()[0].GetRadioID());
    CHECK_EQUAL(1, read_data.decryption_error_report.Get()[0].Get().size());
    CHECK_EQUAL(6, read_data.decryption_error_report.Get()[0].Get()[0]->Length);
    MEMCMP_EQUAL(mac_6_0,
                 (char *)read_data.decryption_error_report.Get()[0].Get()[0]->MACAddresses,
                 sizeof(mac_6_0));

    CHECK_EQUAL(0, read_data.duplicate_ipv4_address.Get().size());
    CHECK_EQUAL(0, read_data.wtp_radio_statistics.Get().size());
    CHECK_EQUAL(nullptr, read_data.wtp_reboot_statistics);
    CHECK_EQUAL(0, read_data.delete_station.Get().size());
    CHECK_EQUAL(0, read_data.vendor_specific_payloads.Get().size());
    CHECK_EQUAL(0, read_data.unknown_elements);
}