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

    {
        WritableRadioAdministrativeStateArray radio_states;
        radio_states.Add({ 0, RadioAdministrativeState::States::Enabled });
        radio_states.Add({ 1, RadioAdministrativeState::States::Disabled });

        WTPRebootStatistics wtp_reboot_statistics{
            21, 22, 23, 24, 25, 26, 27, WTPRebootStatistics::LastFailureType::HardwareFailure
        };

        WritableACNameWithPriorityArray ac_names_with_priority;
        ac_names_with_priority.Add(1, "ACNameWithPriority");
        ac_names_with_priority.Add(2, "ACNameWithPriority2");

        WritableCapwapTransportProtocol capwap_transport_protocol{
            CapwapTransportProtocol::Type::UDP
        };

        WritableWTPStaticIPAddressInformation wtp_static_ipaddress{ inet_addr("192.168.100.10"),
                                                                    inet_addr("255.255.255.0"),
                                                                    inet_addr("192.168.1.1"),
                                                                    true };

        WritableVendorSpecificPayloadArray vendor_specific_payloads;
        vendor_specific_payloads.Add(123456, 789, "01234567890ABCDEF0123");

        IWritableConfigurationStatusRequestOptionalElement *const elems_1[] = { &ac_names_with_priority, &capwap_transport_protocol, &wtp_static_ipaddress, &vendor_specific_payloads };
        WritableConfigurationStatusRequest write_data("abcdefабвгд",
                                                      radio_states,
                                                      12345,
                                                      wtp_reboot_statistics,
            elems_1);

        write_data.Serialize(&raw_data);
    }
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

    ReadableACNameWithPriorityArray ac_names_with_priority;
    ReadableCapwapTransportProtocol capwap_transport_protocol;
    ReadableWTPStaticIPAddressInformation wtp_static_ipaddress;
    ReadableVendorSpecificPayloadArray vendor_specific_payloads;

    IReadableConfigurationStatusRequestOptionalElement *const elems_2[] = { &ac_names_with_priority, &capwap_transport_protocol, &wtp_static_ipaddress, &vendor_specific_payloads };
    ReadableConfigurationStatusRequest read_data(elems_2);

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_TRUE(read_data.ac_name.IsPresent());
    STRNCMP_EQUAL("abcdefабвгд", (char *)read_data.ac_name.Get()->name, 14);

    CHECK_EQUAL(2, read_data.radio_states.Get().size());
    CHECK_EQUAL(0, read_data.radio_states.Get()[0]->RadioID);
    CHECK_EQUAL(RadioAdministrativeState::States::Enabled,
                read_data.radio_states.Get()[0]->AdminState);
    CHECK_EQUAL(1, read_data.radio_states.Get()[1]->RadioID);
    CHECK_EQUAL(RadioAdministrativeState::States::Disabled,
                read_data.radio_states.Get()[1]->AdminState);

    CHECK_TRUE(read_data.statistics_timer.IsPresent());
    CHECK_EQUAL(12345, read_data.statistics_timer.Get()->GetValue());

    CHECK_TRUE(read_data.wtp_reboot_statistics.IsPresent());
    CHECK_EQUAL(21, read_data.wtp_reboot_statistics.Get()->GetRebootCount());
    CHECK_EQUAL(22, read_data.wtp_reboot_statistics.Get()->GetACInitiatedCount());
    CHECK_EQUAL(23, read_data.wtp_reboot_statistics.Get()->GetLinkFailureCount());
    CHECK_EQUAL(24, read_data.wtp_reboot_statistics.Get()->GetSWFailureCount());
    CHECK_EQUAL(25, read_data.wtp_reboot_statistics.Get()->GetHWFailureCount());
    CHECK_EQUAL(26, read_data.wtp_reboot_statistics.Get()->GetOtherFailureCount());
    CHECK_EQUAL(27, read_data.wtp_reboot_statistics.Get()->GetUnknownFailureCount());
    CHECK_EQUAL(WTPRebootStatistics::LastFailureType::HardwareFailure,
                read_data.wtp_reboot_statistics.Get()->GetLastFailureType());

    CHECK_TRUE(ac_names_with_priority.IsPresent());
    CHECK_EQUAL(2, ac_names_with_priority.Get().size());
    CHECK_EQUAL(1, ac_names_with_priority.Get()[0]->GetPriority());
    STRNCMP_EQUAL("ACNameWithPriority", (char *)ac_names_with_priority.Get()[0]->name, 18);
    CHECK_EQUAL(18, ac_names_with_priority.Get()[0]->GetNameLenght());
    CHECK_EQUAL(2, ac_names_with_priority.Get()[1]->GetPriority());
    STRNCMP_EQUAL("ACNameWithPriority2", (char *)ac_names_with_priority.Get()[1]->name, 19);
    CHECK_EQUAL(19, ac_names_with_priority.Get()[1]->GetNameLenght());

    CHECK_TRUE(capwap_transport_protocol.IsPresent());
    CHECK_EQUAL(CapwapTransportProtocol::Type::UDP, capwap_transport_protocol.Get()->type);

    CHECK_TRUE(wtp_static_ipaddress.IsPresent());
    CHECK_EQUAL(inet_addr("192.168.100.10"), wtp_static_ipaddress.Get()->IpAddress);
    CHECK_EQUAL(inet_addr("255.255.255.0"), wtp_static_ipaddress.Get()->Netmask);
    CHECK_EQUAL(inet_addr("192.168.1.1"), wtp_static_ipaddress.Get()->Gateway);
    CHECK_EQUAL(1, wtp_static_ipaddress.Get()->Static);

    CHECK_TRUE(vendor_specific_payloads.IsPresent());
    CHECK_EQUAL(1, vendor_specific_payloads.Get().size());
    CHECK_EQUAL(123456, vendor_specific_payloads.Get()[0]->GetVendorIdentifier());
    CHECK_EQUAL(789, vendor_specific_payloads.Get()[0]->GetElementId());
    STRNCMP_EQUAL("01234567890ABCDEF0123", (char *)vendor_specific_payloads.Get()[0]->value, 21);
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

    ReadableACNameWithPriorityArray ac_names_with_priority;
    ReadableCapwapTransportProtocol capwap_transport_protocol;
    ReadableWTPStaticIPAddressInformation wtp_static_ipaddress;
    ReadableVendorSpecificPayloadArray vendor_specific_payloads;

    IReadableConfigurationStatusRequestOptionalElement *const elems_3[] = { &ac_names_with_priority, &capwap_transport_protocol, &wtp_static_ipaddress, &vendor_specific_payloads };
    ReadableConfigurationStatusRequest read_data(elems_3);

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);

    STRNCMP_EQUAL("AC-Primary", (char *)read_data.ac_name.Get()->name, 10);
    CHECK_EQUAL(10, read_data.ac_name.Get()->GetLength());

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

    CHECK_TRUE(read_data.statistics_timer.IsPresent());
    CHECK_EQUAL(120, read_data.statistics_timer.Get()->GetValue());

    CHECK_TRUE(read_data.wtp_reboot_statistics.IsPresent());
    CHECK_EQUAL(5, read_data.wtp_reboot_statistics.Get()->GetRebootCount());
    CHECK_EQUAL(2, read_data.wtp_reboot_statistics.Get()->GetACInitiatedCount());
    CHECK_EQUAL(1, read_data.wtp_reboot_statistics.Get()->GetLinkFailureCount());
    CHECK_EQUAL(1, read_data.wtp_reboot_statistics.Get()->GetSWFailureCount());
    CHECK_EQUAL(0, read_data.wtp_reboot_statistics.Get()->GetHWFailureCount());
    CHECK_EQUAL(1, read_data.wtp_reboot_statistics.Get()->GetOtherFailureCount());
    CHECK_EQUAL(0, read_data.wtp_reboot_statistics.Get()->GetUnknownFailureCount());
    CHECK_EQUAL(WTPRebootStatistics::LastFailureType::SoftwareFailure,
                read_data.wtp_reboot_statistics.Get()->GetLastFailureType());

    CHECK_EQUAL(2, ac_names_with_priority.Get().size());
    CHECK_EQUAL(1, ac_names_with_priority.Get()[0]->GetPriority());
    STRNCMP_EQUAL("AC-Primary", (char *)ac_names_with_priority.Get()[0]->name, 10);
    CHECK_EQUAL(10, ac_names_with_priority.Get()[0]->GetNameLenght());
    CHECK_EQUAL(2, ac_names_with_priority.Get()[1]->GetPriority());
    STRNCMP_EQUAL(" AC-Secondary ", (char *)ac_names_with_priority.Get()[1]->name, 14);
    CHECK_EQUAL(14, ac_names_with_priority.Get()[1]->GetNameLenght());

    CHECK_EQUAL(CapwapTransportProtocol::Type::UDP, capwap_transport_protocol.Get()->type);

    CHECK_EQUAL(inet_addr("192.168.1.100"), wtp_static_ipaddress.Get()->IpAddress);
    CHECK_EQUAL(inet_addr("255.255.255.0"), wtp_static_ipaddress.Get()->Netmask);
    CHECK_EQUAL(inet_addr("192.168.1.1"), wtp_static_ipaddress.Get()->Gateway);
    CHECK_EQUAL(1, wtp_static_ipaddress.Get()->Static);

    CHECK_EQUAL(1, vendor_specific_payloads.Get().size());
    CHECK_EQUAL(11134, vendor_specific_payloads.Get()[0]->GetVendorIdentifier());
    CHECK_EQUAL(1, vendor_specific_payloads.Get()[0]->GetElementId());
    STRNCMP_EQUAL("TestData", (char *)vendor_specific_payloads.Get()[0]->value, 8);

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

    ReadableConfigurationStatusRequest read_data(nonstd::span<IReadableConfigurationStatusRequestOptionalElement *const>{});

    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, read_data.unknown_elements);
}

TEST(ConfigurationStatusRequestTestsGroup, IEEE80211_specific_message_elements) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    std::vector<uint8_t> rates_0 = { 0x82, 0x84 };
    std::vector<NetworkU16> levels_0 = { 20 };
    const uint8_t bssid1[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x01 };

    {
        WritableRadioAdministrativeStateArray radio_states;
        radio_states.Add({ 0, RadioAdministrativeState::States::Enabled });

        WTPRebootStatistics wtp_reboot_statistics{
            21, 22, 23, 24, 25, 26, 27, WTPRebootStatistics::LastFailureType::HardwareFailure
        };

        WritableAntennaArray w_antennas;
        std::vector<Antenna::AntennaSelection> selection_0 = {
            Antenna::AntennaSelection::Internal
        };
        w_antennas.Add(1,
                       Antenna::Diversity::Enabled,
                       Antenna::Combiner::SectorizedLeft,
                       selection_0);

        WritableDirectSequenceControlArray w_ctrls;
        w_ctrls.Add({ 1, 1, DirectSequenceControl::CCAMode::EdOnly, 100 });

        WritableMACOperationArray w_ops;
        w_ops.Add({ 1, 2347, 7, 4, 2346, 512, 512 });

        WritableMultiDomainCapabilityArray w_capabilities;
        w_capabilities.Add({ 1, 1, 13, 20 });

        WritableOFDMControlArray w_controls;
        w_controls.Add({ 1, 36, 0x01, 100 });

        WritableSupportedRatesArray w_rates;
        w_rates.Add({ 1, rates_0 });

        WritableTxPowerArray w_tps;
        w_tps.Add({ 1, 20 });

        WritableTxPowerLevelArray w_levels;
        w_levels.Add({ 1, levels_0 });

        WritableWTPRadioConfigurationArray w_configs;
        w_configs.Add({ 1, 1, 4, 2, bssid1, 100, "US " });

        WritableWTPRadioInformationArray w_infos;
        w_infos.Add({ 1, true, false, true, true, false, false, false }); // b/g/n

        IWritableConfigurationStatusRequestOptionalElement *const elems_4[] = { &w_antennas, &w_ctrls, &w_ops, &w_capabilities, &w_controls, &w_rates, &w_tps, &w_levels, &w_configs, &w_infos };
        WritableConfigurationStatusRequest write_data("abcdefабвгд",
                                                      radio_states,
                                                      12345,
                                                      wtp_reboot_statistics,
            elems_4);

        write_data.Serialize(&raw_data);
    }

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableAntennaArray r_antennas;
    ReadableDirectSequenceControlArray r_ctrls;
    ReadableMACOperationArray r_ops;
    ReadableMultiDomainCapabilityArray r_capabilities;
    ReadableOFDMControlArray r_controls;
    ReadableSupportedRatesArray r_rates;
    ReadableTxPowerArray r_tps;
    ReadableTxPowerLevelArray r_levels;
    ReadableWTPRadioConfigurationArray r_configs;
    ReadableWTPRadioInformationArray r_infos;
    IReadableConfigurationStatusRequestOptionalElement *const elems_5[] = { &r_antennas, &r_ctrls, &r_ops, &r_capabilities, &r_controls, &r_rates, &r_tps, &r_levels, &r_configs, &r_infos };
    ReadableConfigurationStatusRequest read_data(elems_5);

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_TRUE(read_data.ac_name.IsPresent());
    CHECK_TRUE(read_data.radio_states.IsPresent());
    CHECK_TRUE(read_data.statistics_timer.IsPresent());
    CHECK_TRUE(read_data.wtp_reboot_statistics.IsPresent());

    CHECK_TRUE(r_antennas.IsPresent());
    CHECK_EQUAL(1, r_antennas.Get().size());
    CHECK_EQUAL(1, r_antennas.Get()[0]->GetRadioID());
    CHECK_EQUAL(Antenna::Diversity::Enabled, r_antennas.Get()[0]->GetDiversity());
    CHECK_EQUAL(Antenna::Combiner::SectorizedLeft, r_antennas.Get()[0]->GetCombiner());
    CHECK_EQUAL(1, r_antennas.Get()[0]->GetAntennaCount());
    CHECK_EQUAL(Antenna::AntennaSelection::Internal, r_antennas.Get()[0]->antenna_selection[0]);

    CHECK_TRUE(r_ctrls.IsPresent());
    CHECK_EQUAL(1, r_ctrls.Get().size());
    CHECK_EQUAL(1, r_ctrls.Get()[0]->GetRadioID());
    CHECK_EQUAL(1, r_ctrls.Get()[0]->GetCurrentChannel());
    CHECK_EQUAL(DirectSequenceControl::CCAMode::EdOnly, r_ctrls.Get()[0]->GetCurrentCCA());
    CHECK_EQUAL(100, r_ctrls.Get()[0]->GetEnergyDetectThreshold());

    CHECK_TRUE(r_ops.IsPresent());
    CHECK_EQUAL(1, r_ops.Get().size());
    CHECK_EQUAL(1, r_ops.Get()[0]->GetRadioID());
    CHECK_EQUAL(2347, r_ops.Get()[0]->GetRTSThreshold());
    CHECK_EQUAL(7, r_ops.Get()[0]->GetShortRetry());
    CHECK_EQUAL(4, r_ops.Get()[0]->GetLongRetry());
    CHECK_EQUAL(2346, r_ops.Get()[0]->GetFragmentationThreshold());
    CHECK_EQUAL(512, r_ops.Get()[0]->GetTxMSDULifetime());
    CHECK_EQUAL(512, r_ops.Get()[0]->GetRxMSDULifetime());

    CHECK_TRUE(r_capabilities.IsPresent());
    CHECK_EQUAL(1, r_capabilities.Get().size());
    CHECK_EQUAL(1, r_capabilities.Get()[0]->GetRadioID());
    CHECK_EQUAL(1, r_capabilities.Get()[0]->GetFirstChannel());
    CHECK_EQUAL(13, r_capabilities.Get()[0]->GetNumberOfChannels());
    CHECK_EQUAL(20, r_capabilities.Get()[0]->GetMaxTxPowerLevel());

    CHECK_TRUE(r_controls.IsPresent());
    CHECK_EQUAL(1, r_controls.Get().size());
    CHECK_EQUAL(1, r_controls.Get()[0]->GetRadioID());
    CHECK_EQUAL(36, r_controls.Get()[0]->GetCurrentChannel());
    CHECK_EQUAL(0x01, r_controls.Get()[0]->GetBandSupport());
    CHECK_EQUAL(100, r_controls.Get()[0]->GetTIThreshold());

    CHECK_TRUE(r_rates.IsPresent());
    CHECK_EQUAL(1, r_rates.Get().size());
    CHECK_EQUAL(1, r_rates.Get()[0]->GetRadioID());
    CHECK_EQUAL(2, r_rates.Get()[0]->GetRatesCount());
    CHECK_EQUAL(2 + 1, r_rates.Get()[0]->GetLength());
    MEMCMP_EQUAL(rates_0.data(), r_rates.Get()[0]->data, 2);

    CHECK_TRUE(r_tps.IsPresent());
    CHECK_EQUAL(1, r_tps.Get().size());
    CHECK_EQUAL(1, r_tps.Get()[0]->RadioID);
    CHECK_EQUAL(20, r_tps.Get()[0]->CurrentTxPower.Get());

    CHECK_TRUE(r_levels.IsPresent());
    CHECK_EQUAL(1, r_levels.Get().size());
    CHECK_EQUAL(1, r_levels.Get()[0]->GetRadioID());
    CHECK_EQUAL(1, r_levels.Get()[0]->GetNumLevels());
    CHECK_EQUAL(2 + 2, r_levels.Get()[0]->GetLength());
    MEMCMP_EQUAL(levels_0.data(), r_levels.Get()[0]->data, 2);

    CHECK_TRUE(r_configs.IsPresent());
    CHECK_EQUAL(1, r_configs.Get().size());
    CHECK_EQUAL(1, r_configs.Get()[0]->RadioID);
    CHECK_EQUAL(1, r_configs.Get()[0]->ShortPreamble);
    CHECK_EQUAL(4, r_configs.Get()[0]->NumBSSIDs);
    CHECK_EQUAL(2, r_configs.Get()[0]->DTIMPeriod);
    MEMCMP_EQUAL(bssid1, r_configs.Get()[0]->BSSID, 6);
    CHECK_EQUAL(100, r_configs.Get()[0]->BeaconPeriod.Get());

    CHECK_TRUE(r_infos.IsPresent());
    CHECK_EQUAL(1, r_infos.Get().size());
    CHECK_EQUAL(1, r_infos.Get()[0]->RadioID);
    CHECK_TRUE(r_infos.Get()[0]->B);
    CHECK_FALSE(r_infos.Get()[0]->A);
    CHECK_TRUE(r_infos.Get()[0]->G);
    CHECK_TRUE(r_infos.Get()[0]->N);
    CHECK_FALSE(r_infos.Get()[0]->AC);

    CHECK_EQUAL(0, read_data.unknown_elements);
}
TEST(ConfigurationStatusRequestTestsGroup, GetOptionalElement) {
    ReadableVendorSpecificPayloadArray vendor_specific_payloads;
    IReadableConfigurationStatusRequestOptionalElement *const elems_6[] = { &vendor_specific_payloads };
    ReadableConfigurationStatusRequest read_data(elems_6);

    CHECK_EQUAL(&vendor_specific_payloads,
                read_data.GetOptionalElement<ReadableVendorSpecificPayloadArray>(
                    ElementHeader::VendorSpecificPayload));

    CHECK(read_data.GetOptionalElement<IReadableElement>((ElementHeader::ElementType)0xFFFF) ==
          nullptr);
}

TEST(ConfigurationStatusRequestTestsGroup, MessageTypeIdentification) {
    WritableRadioAdministrativeStateArray radio_states;
    WTPRebootStatistics wtp_reboot_statistics{
        0, 0, 0, 0, 0, 0, 0, WTPRebootStatistics::LastFailureType::Unknown
    };
    WritableConfigurationStatusRequest write_data(
        "ac", radio_states, 0, wtp_reboot_statistics, nonstd::span<IWritableConfigurationStatusRequestOptionalElement *const>{});

    CHECK_EQUAL(ControlHeader::ConfigurationStatusRequest, write_data.GetMessageType());
}
