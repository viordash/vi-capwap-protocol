#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ConfigurationUpdateRequest.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(ConfigurationUpdateRequestTestsGroup){ //
                                                  TEST_SETUP(){}

                                                  TEST_TEARDOWN(){}
};

TEST(ConfigurationUpdateRequestTestsGroup, ConfigurationUpdateRequest_serialize) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    WritableACNameWithPriorityArray ac_names_with_priority;
    ac_names_with_priority.Add(1, "ACNameWithPriority");

    std::optional<ACTimestamp> ac_timestamp{ 12345 };

    const uint8_t mac_6_0[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    const uint8_t mac_8_0[] = { 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD, 0xEE, 0xFF };
    const uint8_t mac_6_1[] = { 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD };
    const uint8_t mac_8_1[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0xEE, 0xFF };

    WritableAddMacAclEntry add_mac_acl_entry;
    add_mac_acl_entry.Add({ mac_6_0 });
    add_mac_acl_entry.Add({ mac_8_0 });

    std::optional<CAPWAPTimers> capwap_timers{ std::in_place, 42, 19 };

    WritableDecryptionErrorReportPeriodArray decryption_error_report_periods;
    decryption_error_report_periods.Add({ 0, 10 });
    decryption_error_report_periods.Add({ 1, 100 });

    WritableDeleteMacAclEntry delete_mac_acl_entry;
    delete_mac_acl_entry.Add({ mac_8_1 });
    delete_mac_acl_entry.Add({ mac_6_1 });

    std::optional<IdleTimeout> idle_timeout{ std::in_place, 12345 };

    std::optional<WritableLocationData> location_data{ std::in_place, "abcdefабвгд" };

    WritableRadioAdministrativeStateArray radio_states;
    radio_states.Add({ 0, RadioAdministrativeState::States::Enabled });
    radio_states.Add({ 1, RadioAdministrativeState::States::Disabled });

    std::optional<StatisticsTimer> statistics_timer{ std::in_place, 12345 };

    std::optional<WTPFallback> wtp_fallback{ std::in_place, WTPFallback::Mode::Reserved };

    std::optional<WritableWTPName> wtp_name{ std::in_place, "abcdefабвгд" };

    std::optional<WTPStaticIPAddressInformation> wtp_static_ipaddress{ std::in_place,
                                                                       inet_addr("192.168.100.10"),
                                                                       inet_addr("255.255.255.0"),
                                                                       inet_addr("192.168.1.1"),
                                                                       true };

    std::optional<WritableImageIdentifier> image_identifier{ std::in_place,
                                                             123456,
                                                             "abcdef Привет 1234" };

    WritableVendorSpecificPayloadArray vendor_specific_payloads;
    vendor_specific_payloads.Add(123456, 789, "01234567890ABCDEF0123");

    WritableConfigurationUpdateRequest write_data(ac_names_with_priority,
                                                  ac_timestamp,
                                                  add_mac_acl_entry,
                                                  capwap_timers,
                                                  decryption_error_report_periods,
                                                  delete_mac_acl_entry,
                                                  idle_timeout,
                                                  location_data,
                                                  radio_states,
                                                  statistics_timer,
                                                  wtp_fallback,
                                                  wtp_name,
                                                  wtp_static_ipaddress,
                                                  image_identifier,
                                                  vendor_specific_payloads);

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 260 - (sizeof(ClearHeader) + sizeof(ControlHeader)), raw_data.current);
    const uint8_t reference[] = {
        0x00, 0x10, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x01, 0x00, 0xF4,
        0x00, 0x00, 0x05, 0x00, 0x13, 0x01, 0x41, 0x43, 0x4E, 0x61, 0x6D, 0x65, 0x57, 0x69, 0x74,
        0x68, 0x50, 0x72, 0x69, 0x6F, 0x72, 0x69, 0x74, 0x79, 0x00, 0x06, 0x00, 0x04, 0x00, 0x00,
        0x30, 0x39, 0x00, 0x07, 0x00, 0x11, 0x02, 0x06, 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x08,
        0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD, 0xEE, 0xFF, 0x00, 0x0C, 0x00, 0x02, 0x2A, 0x13, 0x00,
        0x10, 0x00, 0x03, 0x00, 0x00, 0x0A, 0x00, 0x10, 0x00, 0x03, 0x01, 0x00, 0x64, 0x00, 0x11,
        0x00, 0x11, 0x02, 0x08, 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0xEE, 0xFF, 0x06, 0xAA, 0xBB,
        0xCC, 0xFF, 0xFE, 0xDD, 0x00, 0x17, 0x00, 0x04, 0x00, 0x00, 0x30, 0x39, 0x00, 0x1C, 0x00,
        0x10, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0xD0, 0xB0, 0xD0, 0xB1, 0xD0, 0xB2, 0xD0, 0xB3,
        0xD0, 0xB4, 0x00, 0x1F, 0x00, 0x02, 0x00, 0x01, 0x00, 0x1F, 0x00, 0x02, 0x01, 0x02, 0x00,
        0x24, 0x00, 0x02, 0x30, 0x39, 0x00, 0x28, 0x00, 0x01, 0x00, 0x00, 0x2D, 0x00, 0x10, 0x61,
        0x62, 0x63, 0x64, 0x65, 0x66, 0xD0, 0xB0, 0xD0, 0xB1, 0xD0, 0xB2, 0xD0, 0xB3, 0xD0, 0xB4,
        0x00, 0x31, 0x00, 0x0D, 0xC0, 0xA8, 0x64, 0x0A, 0xFF, 0xFF, 0xFF, 0x00, 0xC0, 0xA8, 0x01,
        0x01, 0x01, 0x00, 0x19, 0x00, 0x1C, 0x00, 0x01, 0xE2, 0x40, 0x61, 0x62, 0x63, 0x64, 0x65,
        0x66, 0x20, 0xD0, 0x9F, 0xD1, 0x80, 0xD0, 0xB8, 0xD0, 0xB2, 0xD0, 0xB5, 0xD1, 0x82, 0x20,
        0x31, 0x32, 0x33, 0x34, 0x00, 0x25, 0x00, 0x1B, 0x00, 0x01, 0xE2, 0x40, 0x03, 0x15, 0x30,
        0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45,
        0x46, 0x30, 0x31, 0x32, 0x33
    };

    MEMCMP_EQUAL(buffer,
                 reference + (sizeof(ClearHeader) + sizeof(ControlHeader)),
                 sizeof(reference) - (sizeof(ClearHeader) + sizeof(ControlHeader)));

    raw_data = { buffer, buffer + 260 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };
    ReadableConfigurationUpdateRequest read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(1, read_data.ac_names_with_priority.Get().size());
    CHECK_EQUAL(19, read_data.ac_names_with_priority.Get()[0]->GetLength());
    CHECK_EQUAL(1, read_data.ac_names_with_priority.Get()[0]->GetPriority());
    STRNCMP_EQUAL("ACNameWithPriority",
                  (char *)read_data.ac_names_with_priority.Get()[0]->name,
                  18);
    CHECK_EQUAL(18, read_data.ac_names_with_priority.Get()[0]->GetNameLenght());

    CHECK_EQUAL(12345, read_data.ac_timestamp->GetTimestamp());

    CHECK_EQUAL(2, read_data.add_mac_acl_entry.Get().size());
    CHECK_EQUAL(6, read_data.add_mac_acl_entry.Get()[0]->Length);
    MEMCMP_EQUAL(mac_6_0,
                 (char *)read_data.add_mac_acl_entry.Get()[0]->MACAddresses,
                 sizeof(mac_6_0));
    CHECK_EQUAL(8, read_data.add_mac_acl_entry.Get()[1]->Length);
    MEMCMP_EQUAL(mac_8_0,
                 (char *)read_data.add_mac_acl_entry.Get()[1]->MACAddresses,
                 sizeof(mac_8_0));

    CHECK_EQUAL(42, read_data.capwap_timers->Discovery);
    CHECK_EQUAL(19, read_data.capwap_timers->EchoInterval);

    CHECK_EQUAL(2, read_data.decryption_error_report_periods.Get().size());
    CHECK_EQUAL(0, read_data.decryption_error_report_periods.Get()[0]->RadioID());
    CHECK_EQUAL(10, read_data.decryption_error_report_periods.Get()[0]->ReportInterval());
    CHECK_EQUAL(1, read_data.decryption_error_report_periods.Get()[1]->RadioID());
    CHECK_EQUAL(100, read_data.decryption_error_report_periods.Get()[1]->ReportInterval());

    CHECK_EQUAL(2, read_data.delete_mac_acl_entry.Get().size());
    CHECK_EQUAL(8, read_data.delete_mac_acl_entry.Get()[0]->Length);
    MEMCMP_EQUAL(mac_8_1,
                 (char *)read_data.delete_mac_acl_entry.Get()[0]->MACAddresses,
                 sizeof(mac_8_1));
    CHECK_EQUAL(6, read_data.delete_mac_acl_entry.Get()[1]->Length);
    MEMCMP_EQUAL(mac_6_1,
                 (char *)read_data.delete_mac_acl_entry.Get()[1]->MACAddresses,
                 sizeof(mac_6_1));

    CHECK_EQUAL(12345, read_data.idle_timeout->GetTimeout());

    CHECK_EQUAL(16, read_data.location_data->GetLength());
    STRNCMP_EQUAL("abcdefабвгд", (char *)read_data.location_data->data, 16);

    CHECK_EQUAL(2, read_data.radio_states.Get().size());
    CHECK_EQUAL(0, read_data.radio_states.Get()[0]->RadioID);
    CHECK_EQUAL(RadioAdministrativeState::States::Enabled,
                read_data.radio_states.Get()[0]->AdminState);
    CHECK_EQUAL(1, read_data.radio_states.Get()[1]->RadioID);
    CHECK_EQUAL(RadioAdministrativeState::States::Disabled,
                read_data.radio_states.Get()[1]->AdminState);

    CHECK_EQUAL(12345, read_data.statistics_timer->GetValue());

    CHECK_EQUAL(WTPFallback::Mode::Reserved, read_data.wtp_fallback->mode);

    CHECK_EQUAL(16, read_data.wtp_name->GetLength());
    STRNCMP_EQUAL("abcdefабвгд", (char *)read_data.wtp_name->name, 16);

    CHECK_EQUAL(inet_addr("192.168.100.10"), read_data.wtp_static_ipaddress->IpAddress);
    CHECK_EQUAL(inet_addr("255.255.255.0"), read_data.wtp_static_ipaddress->Netmask);
    CHECK_EQUAL(inet_addr("192.168.1.1"), read_data.wtp_static_ipaddress->Gateway);
    CHECK_EQUAL(1, read_data.wtp_static_ipaddress->Static);

    CHECK_EQUAL(24, read_data.image_identifier.GetData().size());
    CHECK_EQUAL(123456, read_data.image_identifier.GetVendorIdentifier());
    STRNCMP_EQUAL("abcdef Привет 1234", (char *)read_data.image_identifier.GetData().data(), 24);

    CHECK_EQUAL(1, read_data.vendor_specific_payloads.Get().size());
    CHECK_EQUAL(123456, read_data.vendor_specific_payloads.Get()[0]->GetVendorIdentifier());
    CHECK_EQUAL(789, read_data.vendor_specific_payloads.Get()[0]->GetElementId());
    STRNCMP_EQUAL("01234567890ABCDEF0123",
                  (char *)read_data.vendor_specific_payloads.Get()[0]->value,
                  21);
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(ConfigurationUpdateRequestTestsGroup, ConfigurationUpdateRequest_deserialize_image_data) {
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
        0x00, 0x07,     // Message Type: 7 (Configuration Update Request)
        0x1A,           // Sequence Number: 0x1A (26)
        0x00, 0x9B,     // Message Element Length: 155 байт (сумма длин всех следующих элементов) <-- ИСПРАВЛЕНО
        0x00,           // Flags: 0

        // =================================================================
        // 3. Message Elements (12 штук, 155 байт)
        // =================================================================

        // --- Элемент 1: WTP Name (Type 45) - 17 байт ---
        0x00, 0x2D,     // Type: 45
        0x00, 0x0D,     // Length: 13
        'O', 'f', 'f', 'i', 'c', 'e', '-', 'A', 'P', '-', '1', '0', '1', // Value: "Office-AP-101"

        // --- Элемент 2: Location Data (Type 28) - 24 байта --- 
        0x00, 0x1C,     // Type: 28
        0x00, 0x14,     // Length: 20 (длина строки "3rd Floor, East Wing") <
        '3', 'r', 'd', ' ', 'F', 'l', 'o', 'o', 'r', ',', ' ', // Value: "3rd Floor, East Wing"
        'E', 'a', 's', 't', ' ', 'W', 'i', 'n', 'g',

        // --- Элемент 3: WTP Fallback (Type 40) - 5 байт ---
        0x00, 0x28,     // Type: 40
        0x00, 0x01,     // Length: 1
        0x01,           // Value: 1 (Enabled)

        // --- Элемент 4: Radio Administrative State (Type 31) - 6 байт ---
        0x00, 0x1F,     // Type: 31
        0x00, 0x02,     // Length: 2
        0x01,           // Value: Radio ID = 1
        0x02,           // Value: Admin State = 2 (Disabled)

        // --- Элемент 5: Radio Administrative State (Type 31) - 6 байт ---
        0x00, 0x1F,     // Type: 31
        0x00, 0x02,     // Length: 2
        0x02,           // Value: Radio ID = 2
        0x01,           // Value: Admin State = 1 (Enabled)

        // --- Элемент 6: Idle Timeout (Type 23) - 8 байт ---
        0x00, 0x17,     // Type: 23
        0x00, 0x04,     // Length: 4
        0x00, 0x00,
        0x02, 0x58,     // Value: 600 (0x00000258)

        // --- Элемент 7: Statistics Timer (Type 36) - 6 байт ---
        0x00, 0x24,     // Type: 36
        0x00, 0x02,     // Length: 2
        0x00, 0xB4,     // Value: 180 (0x00B4)

        // --- Элемент 8: Add MAC ACL Entry (Type 7) - 19 байт ---
        0x00, 0x07,     // Type: 7
        0x00, 0x0F,     // Length: 15
        0x02,           // Value: Num of Entries = 2 (1 байт)
        0x06, 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, // Entry 1: Len=6, MAC=00:1A:2B:3C:4D:5E
        0x06, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, // Entry 2: Len=6, MAC=AA:BB:CC:DD:EE:FF

        // --- Элемент 9: Delete MAC ACL Entry (Type 17) - 12 байт ---
        0x00, 0x11,     // Type: 17
        0x00, 0x08,     // Length: 8
        0x01,           // Value: Num of Entries = 1 (1 байт)
        0x06,           // Value: Length = 6
        0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE, // Value: MAC=DE:AD:BE:EF:CA:FE

        // --- Элемент 10: Image Identifier (Type 25) - 24 байта ---
        0x00, 0x19,     // Type: 25
        0x00, 0x14,     // Length: 20 (4 байта Vendor ID + 16 байт имя)
        0x00, 0x00, 0x00, 0x09, // Value: Vendor Identifier = 9 (Cisco)
        'c', 'a', 'p', 'w', 'a', 'p', '-', 'f', // Value: "capwap-fw-v2.3.4"
        'w', '-', 'v', '2', '.', '3', '.', '4',

        // --- Элемент 11: AC Name with Priority (Type 5) - 14 байт ---
        0x00, 0x05,     // Type: 5
        0x00, 0x0A,     // Length: 10 (1 байт приоритет + 9 байт имя)
        0x02,           // Value: Priority = 2
        'A', 'C', '-', 'B', 'a', 'c', 'k', 'u', 'p', // Value: "AC-Backup"

        // --- Элемент 12: Vendor Specific Payload (Type 37) - 14 байт ---
        0x00, 0x25,     // Type: 37
        0x00, 0x0A,     // Length: 10 (4 байта Vendor ID + 2 байта Element ID + 4 байта данных)
        0x00, 0x00, 0x39, 0xE7, // Value: Vendor Identifier = 14823 (Aruba)
        0x01, 0x01,     // Value: Element ID = 0x0101 (условный ID)
        0xDE, 0xAD, 0xBE, 0xEF  // Value: Произвольные данные
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableConfigurationUpdateRequest read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);

    CHECK_EQUAL(1, read_data.ac_names_with_priority.Get().size());
    CHECK_EQUAL(10, read_data.ac_names_with_priority.Get()[0]->GetLength());
    CHECK_EQUAL(2, read_data.ac_names_with_priority.Get()[0]->GetPriority());
    STRNCMP_EQUAL("AC-Backup", (char *)read_data.ac_names_with_priority.Get()[0]->name, 9);
    CHECK_EQUAL(9, read_data.ac_names_with_priority.Get()[0]->GetNameLenght());

    CHECK_EQUAL(nullptr, read_data.ac_timestamp);

    const uint8_t mac_6_0[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    const uint8_t mac_6_1[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    CHECK_EQUAL(2, read_data.add_mac_acl_entry.Get().size());
    CHECK_EQUAL(6, read_data.add_mac_acl_entry.Get()[0]->Length);
    MEMCMP_EQUAL(mac_6_0,
                 (char *)read_data.add_mac_acl_entry.Get()[0]->MACAddresses,
                 sizeof(mac_6_0));
    CHECK_EQUAL(6, read_data.add_mac_acl_entry.Get()[1]->Length);
    MEMCMP_EQUAL(mac_6_1,
                 (char *)read_data.add_mac_acl_entry.Get()[1]->MACAddresses,
                 sizeof(mac_6_1));

    CHECK_EQUAL(nullptr, read_data.capwap_timers);

    CHECK_EQUAL(0, read_data.decryption_error_report_periods.Get().size());

    const uint8_t mac_6_2[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE };
    CHECK_EQUAL(1, read_data.delete_mac_acl_entry.Get().size());
    CHECK_EQUAL(6, read_data.delete_mac_acl_entry.Get()[0]->Length);
    MEMCMP_EQUAL(mac_6_2,
                 (char *)read_data.delete_mac_acl_entry.Get()[0]->MACAddresses,
                 sizeof(mac_6_2));

    CHECK_EQUAL(600, read_data.idle_timeout->GetTimeout());

    CHECK_EQUAL(20, read_data.location_data->GetLength());
    STRNCMP_EQUAL("3rd Floor, East Wing", (char *)read_data.location_data->data, 20);

    CHECK_EQUAL(2, read_data.radio_states.Get().size());
    CHECK_EQUAL(1, read_data.radio_states.Get()[0]->RadioID);
    CHECK_EQUAL(RadioAdministrativeState::States::Disabled,
                read_data.radio_states.Get()[0]->AdminState);
    CHECK_EQUAL(2, read_data.radio_states.Get()[1]->RadioID);
    CHECK_EQUAL(RadioAdministrativeState::States::Enabled,
                read_data.radio_states.Get()[1]->AdminState);

    CHECK_EQUAL(180, read_data.statistics_timer->GetValue());

    CHECK_EQUAL(WTPFallback::Mode::Enabled, read_data.wtp_fallback->mode);

    CHECK_EQUAL(13, read_data.wtp_name->GetLength());
    STRNCMP_EQUAL("Office-AP-101", (char *)read_data.wtp_name->name, 13);

    CHECK_EQUAL(nullptr, read_data.wtp_static_ipaddress);

    CHECK_EQUAL(16, read_data.image_identifier.GetData().size());
    CHECK_EQUAL(9, read_data.image_identifier.GetVendorIdentifier());
    STRNCMP_EQUAL("capwap-fw-v2.3.4", (char *)read_data.image_identifier.GetData().data(), 24);

    CHECK_EQUAL(1, read_data.vendor_specific_payloads.Get().size());
    CHECK_EQUAL(14823, read_data.vendor_specific_payloads.Get()[0]->GetVendorIdentifier());
    CHECK_EQUAL(0x0101, read_data.vendor_specific_payloads.Get()[0]->GetElementId());
    const uint8_t vendor_payload[] = { 0xDE, 0xAD, 0xBE, 0xEF };
    MEMCMP_EQUAL(vendor_payload,
                 read_data.vendor_specific_payloads.Get()[0]->value,
                 sizeof(vendor_payload));
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(ConfigurationUpdateRequestTestsGroup,
     ConfigurationUpdateRequest_deserialize_handle_unknown_element) {
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
        0x00, 0x07,     // Message Type: 7 (Configuration Update Request)
        0x1A,           // Sequence Number: 0x1A (26)
        0x00, 0x1B,     // Message Element Length: 27 байт
        0x00,           // Flags: 0

        // --- Элемент 1: WTP Name (Type 45) - 17 байт ---
        0x00, 0x2D,     // Type: 45
        0x00, 0x0D,     // Length: 13
        'O', 'f', 'f', 'i', 'c', 'e', '-', 'A', 'P', '-', '1', '0', '1', // Value: "Office-AP-101"

        // 6. Unknown (5 байт)
        0xFF, 0xFF, 0x00, 0x01, 0x00,
        // 7. Unknown (5 байт)
        0xFF, 0xFE, 0x00, 0x01, 0x00,
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableConfigurationUpdateRequest read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, read_data.unknown_elements);
}

TEST(ConfigurationUpdateRequestTestsGroup, One_or_more_serialize) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    WritableACNameWithPriorityArray ac_names_with_priority;
    std::optional<ACTimestamp> ac_timestamp;

    const uint8_t mac_6_0[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    WritableAddMacAclEntry add_mac_acl_entry;
    add_mac_acl_entry.Add({ mac_6_0 });

    std::optional<CAPWAPTimers> capwap_timers;
    WritableDecryptionErrorReportPeriodArray decryption_error_report_periods;
    WritableDeleteMacAclEntry delete_mac_acl_entry;
    std::optional<IdleTimeout> idle_timeout;
    std::optional<WritableLocationData> location_data;
    WritableRadioAdministrativeStateArray radio_states;
    std::optional<StatisticsTimer> statistics_timer;
    std::optional<WTPFallback> wtp_fallback;
    std::optional<WritableWTPName> wtp_name;
    std::optional<WTPStaticIPAddressInformation> wtp_static_ipaddress;
    std::optional<WritableImageIdentifier> image_identifier;
    WritableVendorSpecificPayloadArray vendor_specific_payloads;

    WritableConfigurationUpdateRequest write_data(ac_names_with_priority,
                                                  ac_timestamp,
                                                  add_mac_acl_entry,
                                                  capwap_timers,
                                                  decryption_error_report_periods,
                                                  delete_mac_acl_entry,
                                                  idle_timeout,
                                                  location_data,
                                                  radio_states,
                                                  statistics_timer,
                                                  wtp_fallback,
                                                  wtp_name,
                                                  wtp_static_ipaddress,
                                                  image_identifier,
                                                  vendor_specific_payloads);

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 12, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x07, 0x00, 0x08, 0x01, 0x06,
                                  0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };

    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + 12 };
    ReadableConfigurationUpdateRequest read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(0, read_data.ac_names_with_priority.Get().size());
    CHECK_EQUAL(nullptr, read_data.ac_timestamp);

    CHECK_EQUAL(1, read_data.add_mac_acl_entry.Get().size());
    CHECK_EQUAL(6, read_data.add_mac_acl_entry.Get()[0]->Length);
    MEMCMP_EQUAL(mac_6_0,
                 (char *)read_data.add_mac_acl_entry.Get()[0]->MACAddresses,
                 sizeof(mac_6_0));

    CHECK_EQUAL(nullptr, read_data.capwap_timers);
    CHECK_EQUAL(0, read_data.decryption_error_report_periods.Get().size());
    CHECK_EQUAL(0, read_data.delete_mac_acl_entry.Get().size());
    CHECK_EQUAL(nullptr, read_data.idle_timeout);
    CHECK_EQUAL(nullptr, read_data.location_data);
    CHECK_EQUAL(0, read_data.radio_states.Get().size());
    CHECK_EQUAL(nullptr, read_data.statistics_timer);
    CHECK_EQUAL(nullptr, read_data.wtp_fallback);
    CHECK_EQUAL(nullptr, read_data.wtp_name);
    CHECK_EQUAL(nullptr, read_data.wtp_static_ipaddress);
    CHECK_EQUAL(0, read_data.image_identifier.GetData().size());
    CHECK_EQUAL(0, read_data.vendor_specific_payloads.Get().size());

    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(ConfigurationUpdateRequestTestsGroup, Validate_returns_false_when_empty_elements) {
    WritableACNameWithPriorityArray ac_names_with_priority;
    std::optional<ACTimestamp> ac_timestamp;
    WritableAddMacAclEntry add_mac_acl_entry;
    std::optional<CAPWAPTimers> capwap_timers;
    WritableDecryptionErrorReportPeriodArray decryption_error_report_periods;
    WritableDeleteMacAclEntry delete_mac_acl_entry;
    std::optional<IdleTimeout> idle_timeout;
    std::optional<WritableLocationData> location_data;
    WritableRadioAdministrativeStateArray radio_states;
    std::optional<StatisticsTimer> statistics_timer;
    std::optional<WTPFallback> wtp_fallback;
    std::optional<WritableWTPName> wtp_name;
    std::optional<WTPStaticIPAddressInformation> wtp_static_ipaddress;
    std::optional<WritableImageIdentifier> image_identifier;
    WritableVendorSpecificPayloadArray vendor_specific_payloads;

    WritableConfigurationUpdateRequest write_data(ac_names_with_priority,
                                                  ac_timestamp,
                                                  add_mac_acl_entry,
                                                  capwap_timers,
                                                  decryption_error_report_periods,
                                                  delete_mac_acl_entry,
                                                  idle_timeout,
                                                  location_data,
                                                  radio_states,
                                                  statistics_timer,
                                                  wtp_fallback,
                                                  wtp_name,
                                                  wtp_static_ipaddress,
                                                  image_identifier,
                                                  vendor_specific_payloads);

    CHECK_FALSE(write_data.Validate());
}

TEST(ConfigurationUpdateRequestTestsGroup,
     Validate_returns_true_if_at_least_one_element_is_not_empty) {

    WritableACNameWithPriorityArray ac_names_with_priority;
    std::optional<ACTimestamp> ac_timestamp;
    WritableAddMacAclEntry add_mac_acl_entry;
    std::optional<CAPWAPTimers> capwap_timers;
    WritableDecryptionErrorReportPeriodArray decryption_error_report_periods;
    WritableDeleteMacAclEntry delete_mac_acl_entry;
    std::optional<IdleTimeout> idle_timeout{ std::in_place, 12345 };
    std::optional<WritableLocationData> location_data;
    WritableRadioAdministrativeStateArray radio_states;
    std::optional<StatisticsTimer> statistics_timer;
    std::optional<WTPFallback> wtp_fallback;
    std::optional<WritableWTPName> wtp_name;
    std::optional<WTPStaticIPAddressInformation> wtp_static_ipaddress;
    std::optional<WritableImageIdentifier> image_identifier;
    WritableVendorSpecificPayloadArray vendor_specific_payloads;

    WritableConfigurationUpdateRequest write_data(ac_names_with_priority,
                                                  ac_timestamp,
                                                  add_mac_acl_entry,
                                                  capwap_timers,
                                                  decryption_error_report_periods,
                                                  delete_mac_acl_entry,
                                                  idle_timeout,
                                                  location_data,
                                                  radio_states,
                                                  statistics_timer,
                                                  wtp_fallback,
                                                  wtp_name,
                                                  wtp_static_ipaddress,
                                                  image_identifier,
                                                  vendor_specific_payloads);

    CHECK_TRUE(write_data.Validate());
}