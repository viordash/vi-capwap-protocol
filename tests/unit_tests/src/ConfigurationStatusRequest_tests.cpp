#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ConfigurationStatusRequest.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(ConfigurationStatusRequestTestsGroup){ //
                                                  TEST_SETUP(){}

                                                  TEST_TEARDOWN(){}
};

TEST(ConfigurationStatusRequestTestsGroup, ConfigurationStatusRequest_serialize) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    WritableRadioAdministrativeStateArray radio_states;
    radio_states.Add({ 0, RadioAdministrativeState::States::Enabled });
    radio_states.Add({ 1, RadioAdministrativeState::States::Disabled });

    WTPRebootStatistics wtp_reboot_statistics{
        21, 22, 23, 24, 25, 26, 27, WTPRebootStatistics::LastFailureType::HardwareFailure
    };

    WritableACNameWithPriorityArray ac_names_with_priority;
    ac_names_with_priority.Add(1, "ACNameWithPriority");
    ac_names_with_priority.Add(2, "ACNameWithPriority2");

    CapwapTransportProtocol capwap_transport_protocol{ CapwapTransportProtocol::Type::UDP };

    WTPStaticIPAddressInformation wtp_static_ipaddress{ inet_addr("192.168.100.10"),
                                                        inet_addr("255.255.255.0"),
                                                        inet_addr("192.168.1.1"),
                                                        true };

    WritableVendorSpecificPayloadArray vendor_specific_payloads;
    vendor_specific_payloads.Add(123456, 789, "01234567890ABCDEF0123");

    WritableConfigurationStatusRequest write_data("abcdefабвгд",
                                                  radio_states,
                                                  12345,
                                                  wtp_reboot_statistics,
                                                  ac_names_with_priority,
                                                  &capwap_transport_protocol,
                                                  &wtp_static_ipaddress,
                                                  vendor_specific_payloads);

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 173 - (sizeof(ClearHeader) + sizeof(ControlHeader)), raw_data.current);
    const uint8_t reference[] = {
        0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x2A, 0x00, 0x9D,
        0x00, 0x00, 0x04, 0x00, 0x10, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0xD0, 0xB0, 0xD0, 0xB1,
        0xD0, 0xB2, 0xD0, 0xB3, 0xD0, 0xB4, 0x00, 0x1F, 0x00, 0x02, 0x00, 0x01, 0x00, 0x1F, 0x00,
        0x02, 0x01, 0x02, 0x00, 0x24, 0x00, 0x02, 0x30, 0x39, 0x00, 0x30, 0x00, 0x0F, 0x00, 0x15,
        0x00, 0x16, 0x00, 0x17, 0x00, 0x18, 0x00, 0x19, 0x00, 0x1A, 0x00, 0x1B, 0x04, 0x00, 0x05,
        0x00, 0x13, 0x01, 0x41, 0x43, 0x4E, 0x61, 0x6D, 0x65, 0x57, 0x69, 0x74, 0x68, 0x50, 0x72,
        0x69, 0x6F, 0x72, 0x69, 0x74, 0x79, 0x00, 0x05, 0x00, 0x14, 0x02, 0x41, 0x43, 0x4E, 0x61,
        0x6D, 0x65, 0x57, 0x69, 0x74, 0x68, 0x50, 0x72, 0x69, 0x6F, 0x72, 0x69, 0x74, 0x79, 0x32,
        0x00, 0x33, 0x00, 0x01, 0x02, 0x00, 0x31, 0x00, 0x0D, 0xC0, 0xA8, 0x64, 0x0A, 0xFF, 0xFF,
        0xFF, 0x00, 0xC0, 0xA8, 0x01, 0x01, 0x01, 0x00, 0x25, 0x00, 0x1B, 0x00, 0x01, 0xE2, 0x40,
        0x03, 0x15, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42,
        0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32, 0x33
    };

    MEMCMP_EQUAL(buffer,
                 reference + (sizeof(ClearHeader) + sizeof(ControlHeader)),
                 sizeof(reference) - (sizeof(ClearHeader) + sizeof(ControlHeader)));

    raw_data = { buffer, buffer + 173 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };
    ReadableConfigurationStatusRequest read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    STRNCMP_EQUAL("abcdefабвгд", (char *)read_data.ac_name->name, 14);

    CHECK_EQUAL(2, read_data.radio_states.Get().size());
    CHECK_EQUAL(0, read_data.radio_states.Get()[0]->RadioID);
    CHECK_EQUAL(RadioAdministrativeState::States::Enabled,
                read_data.radio_states.Get()[0]->AdminState);
    CHECK_EQUAL(1, read_data.radio_states.Get()[1]->RadioID);
    CHECK_EQUAL(RadioAdministrativeState::States::Disabled,
                read_data.radio_states.Get()[1]->AdminState);

    CHECK_EQUAL(12345, read_data.statistics_timer->GetValue());

    CHECK_EQUAL(21, read_data.wtp_reboot_statistics->GetRebootCount());
    CHECK_EQUAL(22, read_data.wtp_reboot_statistics->GetACInitiatedCount());
    CHECK_EQUAL(23, read_data.wtp_reboot_statistics->GetLinkFailureCount());
    CHECK_EQUAL(24, read_data.wtp_reboot_statistics->GetSWFailureCount());
    CHECK_EQUAL(25, read_data.wtp_reboot_statistics->GetHWFailureCount());
    CHECK_EQUAL(26, read_data.wtp_reboot_statistics->GetOtherFailureCount());
    CHECK_EQUAL(27, read_data.wtp_reboot_statistics->GetUnknownFailureCount());
    CHECK_EQUAL(WTPRebootStatistics::LastFailureType::HardwareFailure,
                read_data.wtp_reboot_statistics->GetLastFailureType());

    CHECK_EQUAL(2, read_data.ac_names_with_priority.Get().size());
    CHECK_EQUAL(1, read_data.ac_names_with_priority.Get()[0]->GetPriority());
    STRNCMP_EQUAL("ACNameWithPriority",
                  (char *)read_data.ac_names_with_priority.Get()[0]->name,
                  18);
    CHECK_EQUAL(18, read_data.ac_names_with_priority.Get()[0]->GetNameLenght());
    CHECK_EQUAL(2, read_data.ac_names_with_priority.Get()[1]->GetPriority());
    STRNCMP_EQUAL("ACNameWithPriority2",
                  (char *)read_data.ac_names_with_priority.Get()[1]->name,
                  19);
    CHECK_EQUAL(19, read_data.ac_names_with_priority.Get()[1]->GetNameLenght());

    CHECK_EQUAL(CapwapTransportProtocol::Type::UDP, read_data.capwap_transport_protocol->type);

    CHECK_EQUAL(inet_addr("192.168.100.10"), read_data.wtp_static_ipaddress->IpAddress);
    CHECK_EQUAL(inet_addr("255.255.255.0"), read_data.wtp_static_ipaddress->Netmask);
    CHECK_EQUAL(inet_addr("192.168.1.1"), read_data.wtp_static_ipaddress->Gateway);
    CHECK_EQUAL(1, read_data.wtp_static_ipaddress->Static);

    CHECK_EQUAL(1, read_data.vendor_specific_payloads.Get().size());
    CHECK_EQUAL(123456, read_data.vendor_specific_payloads.Get()[0]->GetVendorIdentifier());
    CHECK_EQUAL(789, read_data.vendor_specific_payloads.Get()[0]->GetElementId());
    STRNCMP_EQUAL("01234567890ABCDEF0123",
                  (char *)read_data.vendor_specific_payloads.Get()[0]->value,
                  21);
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(ConfigurationStatusRequestTestsGroup, ConfigurationStatusRequest_deserialize_image_data) {
    // clang-format off
    uint8_t data[] = {
        // ===================================================================
        // 1. CAPWAP Header (8 байт)
        //    (HLEN=2, RID=0, WBID=0, Not a fragment)
        // ===================================================================
        0x00,       // Preamble: Version 0, Type 0 (CAPWAP Header follows)
        0x10,       // HLEN: 2 (8 bytes), RID high bits
        0x42,       // RID low bits, WBID: 1 (802.11), T=0
        0x00,       // F=0, L=0, W=0, M=0, K=0, Flags=0
        0x00, 0x00, // Fragment ID
        0x00, 0x00, // Fragment Offset

        // ===================================================================
        // 2. CAPWAP Control Message Header (8 байт)
        // ===================================================================
        0x00, 0x00, 0x00, 0x05, // Message Type: 5 (Configuration Status Request)
        0x0A,                   // Sequence Number: 10 (пример)
        0x00, 0x82,             // Message Element Length: 130 байт (общая длина всех элементов ниже)
        0x00,                   // Flags: 0

        // ===================================================================
        // 3. Message Elements (общая длина 129 байт)
        // ===================================================================

        // --- Обязательный: AC Name (Type 4) ---
        // Имя AC, к которому WTP считает себя привязанной.
        0x00, 0x04,             // Type: 4
        0x00, 0x0A,             // Length: 10
        'A', 'C', '-', 'P', 'r', 'i', 'm', 'a', 'r', 'y', // Value: "AC-Primary"

        // --- Обязательный: Radio Administrative State (Type 31) - для всей WTP ---
        0x00, 0x1F,             // Type: 31
        0x00, 0x02,             // Length: 2
        0xFF,                   // Radio ID: 255 (означает всю WTP)
        0x01,                   // Admin State: 1 (Enabled)

        // --- Обязательный: Radio Administrative State (Type 31) - для Radio 1 ---
        0x00, 0x1F,             // Type: 31
        0x00, 0x02,             // Length: 2
        0x01,                   // Radio ID: 1
        0x01,                   // Admin State: 1 (Enabled)

        // --- Обязательный: Radio Administrative State (Type 31) - для Radio 2 ---
        0x00, 0x1F,             // Type: 31
        0x00, 0x02,             // Length: 2
        0x02,                   // Radio ID: 2
        0x02,                   // Admin State: 2 (Disabled)

        // --- Обязательный: Statistics Timer (Type 36) ---
        0x00, 0x24,             // Type: 36
        0x00, 0x02,             // Length: 2
        0x00, 0x78,             // Value: 120 секунд

        // --- Обязательный: WTP Reboot Statistics (Type 48) ---
        0x00, 0x30,             // Type: 48
        0x00, 0x0F,             // Length: 15
        0x00, 0x05, 0x00, 0x02, // Reboot Count=5, AC Initiated=2
        0x00, 0x01, 0x00, 0x01, // Link Failure=1, SW Failure=1
        0x00, 0x00, 0x00, 0x01, // HW Failure=0, Other Failure=1
        0x00, 0x00, 0x03,       // Unknown Failure=0, Last Failure Type=3 (SW Failure)

        // --- Необязательный: AC Name with Priority (Type 5) - Primary ---
        0x00, 0x05,             // Type: 5
        0x00, 0x0B,             // Length: 11 (1 байт priority + 10 байт имя)
        0x01,                   // Priority: 1
        'A', 'C', '-', 'P', 'r', 'i', 'm', 'a', 'r', 'y', // Value: "AC-Primary"

        // --- Необязательный: AC Name with Priority (Type 5) - Secondary ---
        0x00, 0x05,             // Type: 5
        0x00, 0x0F,             // Length: 15 (1 байт priority + 14 байт имя)
        0x02,                   // Priority: 2
        ' ', 'A', 'C', '-', 'S', 'e', 'c', 'o', 'n', 'd', 'a', 'r', 'y', ' ', // Value: " AC-Secondary "

        // --- Необязательный: CAPWAP Transport Protocol (Type 51) ---
        0x00, 0x33,             // Type: 51
        0x00, 0x01,             // Length: 1
        0x02,                   // Value: 2 (UDP)

        // --- Необязательный: WTP Static IP Address Information (Type 49) ---
        0x00, 0x31,             // Type: 49
        0x00, 0x0D,             // Length: 13
        0xC0, 0xA8, 0x01, 0x64, // IP: 192.168.1.100
        0xFF, 0xFF, 0xFF, 0x00, // Mask: 255.255.255.0
        0xC0, 0xA8, 0x01, 0x01, // Gateway: 192.168.1.1
        0x01,                   // Static: 1 (Enabled)

        // --- Необязательный: Vendor Specific Payload (Type 37) ---
        0x00, 0x25,             // Type: 37
        0x00, 0x0E,             // Length: 14
        0x00, 0x00, 0x2B, 0x7E, // Vendor Identifier: 11134 (пример)
        0x00, 0x01,             // Element ID: 1
        'T', 'e', 's', 't', 'D', 'a', 't', 'a' // Data: "TestData"
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableConfigurationStatusRequest read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);

    STRNCMP_EQUAL("AC-Primary", (char *)read_data.ac_name->name, 10);
    CHECK_EQUAL(10, read_data.ac_name->GetLength());

    CHECK_EQUAL(3, read_data.radio_states.Get().size());
    CHECK_EQUAL(255, read_data.radio_states.Get()[0]->RadioID);
    CHECK_EQUAL(RadioAdministrativeState::States::Enabled,
                read_data.radio_states.Get()[0]->AdminState);
    CHECK_EQUAL(1, read_data.radio_states.Get()[1]->RadioID);
    CHECK_EQUAL(RadioAdministrativeState::States::Enabled,
                read_data.radio_states.Get()[1]->AdminState);
    CHECK_EQUAL(2, read_data.radio_states.Get()[2]->RadioID);
    CHECK_EQUAL(RadioAdministrativeState::States::Disabled,
                read_data.radio_states.Get()[2]->AdminState);

    CHECK_EQUAL(120, read_data.statistics_timer->GetValue());

    CHECK_EQUAL(5, read_data.wtp_reboot_statistics->GetRebootCount());
    CHECK_EQUAL(2, read_data.wtp_reboot_statistics->GetACInitiatedCount());
    CHECK_EQUAL(1, read_data.wtp_reboot_statistics->GetLinkFailureCount());
    CHECK_EQUAL(1, read_data.wtp_reboot_statistics->GetSWFailureCount());
    CHECK_EQUAL(0, read_data.wtp_reboot_statistics->GetHWFailureCount());
    CHECK_EQUAL(1, read_data.wtp_reboot_statistics->GetOtherFailureCount());
    CHECK_EQUAL(0, read_data.wtp_reboot_statistics->GetUnknownFailureCount());
    CHECK_EQUAL(WTPRebootStatistics::LastFailureType::SoftwareFailure,
                read_data.wtp_reboot_statistics->GetLastFailureType());

    CHECK_EQUAL(2, read_data.ac_names_with_priority.Get().size());
    CHECK_EQUAL(1, read_data.ac_names_with_priority.Get()[0]->GetPriority());
    STRNCMP_EQUAL("AC-Primary", (char *)read_data.ac_names_with_priority.Get()[0]->name, 10);
    CHECK_EQUAL(10, read_data.ac_names_with_priority.Get()[0]->GetNameLenght());
    CHECK_EQUAL(2, read_data.ac_names_with_priority.Get()[1]->GetPriority());
    STRNCMP_EQUAL(" AC-Secondary ", (char *)read_data.ac_names_with_priority.Get()[1]->name, 14);
    CHECK_EQUAL(14, read_data.ac_names_with_priority.Get()[1]->GetNameLenght());

    CHECK_EQUAL(CapwapTransportProtocol::Type::UDP, read_data.capwap_transport_protocol->type);

    CHECK_EQUAL(inet_addr("192.168.1.100"), read_data.wtp_static_ipaddress->IpAddress);
    CHECK_EQUAL(inet_addr("255.255.255.0"), read_data.wtp_static_ipaddress->Netmask);
    CHECK_EQUAL(inet_addr("192.168.1.1"), read_data.wtp_static_ipaddress->Gateway);
    CHECK_EQUAL(1, read_data.wtp_static_ipaddress->Static);

    CHECK_EQUAL(1, read_data.vendor_specific_payloads.Get().size());
    CHECK_EQUAL(11134, read_data.vendor_specific_payloads.Get()[0]->GetVendorIdentifier());
    CHECK_EQUAL(1, read_data.vendor_specific_payloads.Get()[0]->GetElementId());
    STRNCMP_EQUAL("TestData", (char *)read_data.vendor_specific_payloads.Get()[0]->value, 8);
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(ConfigurationStatusRequestTestsGroup,
     ConfigurationStatusRequest_deserialize_handle_unknown_element) {
    // clang-format off
    uint8_t data[] = {
        // ---- CAPWAP Header (8 байт) ----
        0x00,       // Preamble: Version 0, Type 0 (CAPWAP Header follows)
        0x10,       // HLEN: 2 (8 bytes), RID high bits
        0x42,       // RID low bits, WBID: 1 (802.11), T=0
        0x00,       // F=0, L=0, W=0, M=0, K=0, Flags=0
        0x00, 0x00, // Fragment ID
        0x00, 0x00, // Fragment Offset

        // ===================================================================
        // 2. CAPWAP Control Message Header (8 байт)
        // ===================================================================
        0x00, 0x00, 0x00, 0x05, // Message Type: 5 (Configuration Status Request)
        0x01,                   // Sequence Number: 1 (пример)
        0x00, 0x33 + 10,             // Message Element Length: 51 + 10 байт 
        0x00,                   // Flags: 0

        // ===================================================================
        // 3. Message Elements (общая длина 51 байт)
        // ===================================================================

        // --- Обязательный: AC Name (Type 4) ---
        0x00, 0x04,             // Type: 4
        0x00, 0x0A,             // Length: 10
        'D', 'e', 'f', 'a', 'u', 'l', 't', '-', 'A', 'C', // Value: "Default-AC"

        // --- Обязательный: Radio Administrative State (Type 31) - для всей WTP ---
        // RFC требует элемент для WTP в целом...
        0x00, 0x1F,             // Type: 31
        0x00, 0x02,             // Length: 2
        0xFF,                   // Radio ID: 255 (означает всю WTP)
        0x01,                   // Admin State: 1 (Enabled)

        // --- Обязательный: Radio Administrative State (Type 31) - для Radio 1 ---
        // ... и по одному элементу для каждого радио. Минимально - одно радио.
        0x00, 0x1F,             // Type: 31
        0x00, 0x02,             // Length: 2
        0x01,                   // Radio ID: 1
        0x01,                   // Admin State: 1 (Enabled)

        // --- Обязательный: Statistics Timer (Type 36) ---
        0x00, 0x24,             // Type: 36
        0x00, 0x02,             // Length: 2
        0x00, 0x78,             // Value: 120 (секунд)

        // --- Обязательный: WTP Reboot Statistics (Type 48) ---
        0x00, 0x30,             // Type: 48
        0x00, 0x0F,             // Length: 15
        // Value (15 байт):
        0x00, 0x01,             // Reboot Count: 1
        0x00, 0x00,             // AC Initiated Count: 0
        0x00, 0x01,             // Link Failure Count: 1
        0x00, 0x00,             // SW Failure Count: 0
        0x00, 0x00,             // HW Failure Count: 0
        0x00, 0x00,             // Other Failure Count: 0
        0x00, 0x00,             // Unknown Failure Count: 0
        0x02,                   // Last Failure Type: 2 (Link Failure)

        // 6. Unknown (5 байт)
        0xFF, 0xFF, 0x00, 0x01, 0x00,
        // 7. Unknown (5 байт)
        0xFF, 0xFE, 0x00, 0x01, 0x00,
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableConfigurationStatusRequest read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, read_data.unknown_elements);
}
