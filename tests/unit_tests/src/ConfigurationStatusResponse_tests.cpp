#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ConfigurationStatusResponse.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(ConfigurationStatusResponseTestsGroup){ //
                                                   TEST_SETUP(){}

                                                   TEST_TEARDOWN(){}
};

TEST(ConfigurationStatusResponseTestsGroup, ConfigurationStatusResponse_serialize) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    CAPWAPTimers capwap_timers{ 42, 19 };

    WritableDecryptionErrorReportPeriodArray decryption_error_report_periods;
    decryption_error_report_periods.Add({ 0, 10 });
    decryption_error_report_periods.Add({ 1, 100 });
    decryption_error_report_periods.Add({ 31, 65535 });

    uint32_t idle_timeout = 1234;

    WTPFallback::Mode wtp_fallback = WTPFallback::Mode::Enabled;

    uint32_t ac_ipv4_list[] = {
        { inet_addr("192.168.1.110") },
        { inet_addr("192.168.1.111") },
        { inet_addr("192.168.1.112") },
    };

    WTPStaticIPAddressInformation wtp_static_ipaddress{ inet_addr("192.168.100.10"),
                                                        inet_addr("255.255.255.0"),
                                                        inet_addr("192.168.1.1"),
                                                        true };

    WritableVendorSpecificPayloadArray vendor_specific_payloads;
    vendor_specific_payloads.Add(123456, 789, "01234567890ABCDEF0123");

    WritableConfigurationStatusResponse write_data(capwap_timers,
                                                   decryption_error_report_periods,
                                                   idle_timeout,
                                                   wtp_fallback,
                                                   ac_ipv4_list,
                                                   &wtp_static_ipaddress,
                                                   vendor_specific_payloads);

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 120 - (sizeof(ClearHeader) + sizeof(ControlHeader)), raw_data.current);
    const uint8_t reference[] = {
        0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x2A, 0x00, 0x68,
        0x00, 0x00, 0x0C, 0x00, 0x02, 0x2A, 0x13, 0x00, 0x10, 0x00, 0x03, 0x00, 0x00, 0x0A, 0x00,
        0x10, 0x00, 0x03, 0x01, 0x00, 0x64, 0x00, 0x10, 0x00, 0x03, 0x1F, 0xFF, 0xFF, 0x00, 0x17,
        0x00, 0x04, 0x00, 0x00, 0x04, 0xD2, 0x00, 0x28, 0x00, 0x01, 0x01, 0x00, 0x02, 0x00, 0x0C,
        0xC0, 0xA8, 0x01, 0x6E, 0xC0, 0xA8, 0x01, 0x6F, 0xC0, 0xA8, 0x01, 0x70, 0x00, 0x31, 0x00,
        0x0D, 0xC0, 0xA8, 0x64, 0x0A, 0xFF, 0xFF, 0xFF, 0x00, 0xC0, 0xA8, 0x01, 0x01, 0x01, 0x00,
        0x25, 0x00, 0x1B, 0x00, 0x01, 0xE2, 0x40, 0x03, 0x15, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
        0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32, 0x33
    };

    MEMCMP_EQUAL(buffer,
                 reference + (sizeof(ClearHeader) + sizeof(ControlHeader)),
                 sizeof(reference) - (sizeof(ClearHeader) + sizeof(ControlHeader)));

    raw_data = { buffer, buffer + 120 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };
    ReadableConfigurationStatusResponse read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(42, read_data.capwap_timers->Discovery);
    CHECK_EQUAL(19, read_data.capwap_timers->EchoInterval);

    CHECK_EQUAL(3, read_data.decryption_error_report_periods.Get().size());
    CHECK_EQUAL(0, read_data.decryption_error_report_periods.Get()[0]->RadioID());
    CHECK_EQUAL(10, read_data.decryption_error_report_periods.Get()[0]->ReportInterval());
    CHECK_EQUAL(1, read_data.decryption_error_report_periods.Get()[1]->RadioID());
    CHECK_EQUAL(100, read_data.decryption_error_report_periods.Get()[1]->ReportInterval());
    CHECK_EQUAL(31, read_data.decryption_error_report_periods.Get()[2]->RadioID());
    CHECK_EQUAL(65535, read_data.decryption_error_report_periods.Get()[2]->ReportInterval());

    CHECK_EQUAL(1234, read_data.idle_timeout->GetTimeout());
    CHECK_EQUAL(WTPFallback::Mode::Enabled, read_data.wtp_fallback->mode);

    CHECK_EQUAL(3, read_data.ac_ipv4_list->GetCount());
    CHECK_EQUAL(inet_addr("192.168.1.110"), read_data.ac_ipv4_list->addresses[0]);
    CHECK_EQUAL(inet_addr("192.168.1.111"), read_data.ac_ipv4_list->addresses[1]);
    CHECK_EQUAL(inet_addr("192.168.1.112"), read_data.ac_ipv4_list->addresses[2]);

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

TEST(ConfigurationStatusResponseTestsGroup, ConfigurationStatusResponse_deserialize_image_data) {
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
        0x00, 0x00, 0x00, 0x06, // Message Type: 6 (Configuration Status Response)
        0x01,                   // Sequence Number: 1 (должен совпадать с Request)
        0x00, 0x37,             // Message Element Length: 55 (0x0037)
        0x00,                   // Flags: 0

        // ===================================================================
        // 3. Message Elements (общая длина 59 байт)
        // ===================================================================

        // --- Обязательный: CAPWAP Timers (Type 12) ---
        0x00, 0x0C,             // Type: 12
        0x00, 0x02,             // Length: 2
        0x14,                   // Discovery: 20 секунд
        0x1E,                   // Echo Request: 30 секунд

        // --- Обязательный: Decryption Error Report Period (Type 16) ---
        0x00, 0x10,             // Type: 16
        0x00, 0x03,             // Length: 3
        0x01,                   // Radio ID: 1
        0x00, 0x78,             // Report Interval: 120 секунд

        // --- Обязательный: Idle Timeout (Type 23) ---
        0x00, 0x17,             // Type: 23
        0x00, 0x04,             // Length: 4
        0x00, 0x00, 0x01, 0x2C, // Timeout: 300 секунд

        // --- Обязательный: WTP Fallback (Type 40) ---
        0x00, 0x28,             // Type: 40
        0x00, 0x01,             // Length: 1
        0x01,                   // Mode: 1 (Enabled)

        // --- Обязательный: AC IPv4 List (Type 2) ---
        0x00, 0x02,             // Type: 2
        0x00, 0x08,             // Length: 8 (для двух адресов)
        0x0A, 0x0A, 0x00, 0x01, // AC IP Address[0]: 10.10.0.1
        0x0A, 0x0A, 0x00, 0x02, // AC IP Address[1]: 10.10.0.2

        // --- Необязательный: WTP Static IP Address Information (Type 49) ---
        0x00, 0x31,             // Type: 49
        0x00, 0x0D,             // Length: 13
        0xC0, 0xA8, 0x0A, 0x32, // IP: 192.168.10.50
        0xFF, 0xFF, 0xFF, 0x00, // Mask: 255.255.255.0
        0xC0, 0xA8, 0x0A, 0x01, // Gateway: 192.168.10.1
        0x01                    // Static: 1 (Enabled)        
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableConfigurationStatusResponse read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);

    CHECK_EQUAL(20, read_data.capwap_timers->Discovery);
    CHECK_EQUAL(30, read_data.capwap_timers->EchoInterval);

    CHECK_EQUAL(1, read_data.decryption_error_report_periods.Get().size());
    CHECK_EQUAL(1, read_data.decryption_error_report_periods.Get()[0]->RadioID());
    CHECK_EQUAL(120, read_data.decryption_error_report_periods.Get()[0]->ReportInterval());

    CHECK_EQUAL(300, read_data.idle_timeout->GetTimeout());
    CHECK_EQUAL(WTPFallback::Mode::Enabled, read_data.wtp_fallback->mode);

    CHECK_EQUAL(2, read_data.ac_ipv4_list->GetCount());
    CHECK_EQUAL(inet_addr("10.10.0.1"), read_data.ac_ipv4_list->addresses[0]);
    CHECK_EQUAL(inet_addr("10.10.0.2"), read_data.ac_ipv4_list->addresses[1]);

    CHECK_EQUAL(inet_addr("192.168.10.50"), read_data.wtp_static_ipaddress->IpAddress);
    CHECK_EQUAL(inet_addr("255.255.255.0"), read_data.wtp_static_ipaddress->Netmask);
    CHECK_EQUAL(inet_addr("192.168.10.1"), read_data.wtp_static_ipaddress->Gateway);
    CHECK_EQUAL(1, read_data.wtp_static_ipaddress->Static);

    CHECK_EQUAL(0, read_data.vendor_specific_payloads.Get().size());
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(ConfigurationStatusResponseTestsGroup,
     ConfigurationStatusResponse_deserialize_handle_unknown_element) {
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
        0x00, 0x00, 0x00, 0x06, // Message Type: 6 (Configuration Status Response)
        0x01,                   // Sequence Number: 1 (должен совпадать с Request)
        0x00, 48,             //
        0x00,                   // Flags: 0

        // ===================================================================
        // 3. Message Elements (общая длина 59 байт)
        // ===================================================================

        // --- Обязательный: CAPWAP Timers (Type 12) ---
        0x00, 0x0C,             // Type: 12
        0x00, 0x02,             // Length: 2
        0x14,                   // Discovery: 20 секунд
        0x1E,                   // Echo Request: 30 секунд

        // --- Обязательный: Decryption Error Report Period (Type 16) ---
        0x00, 0x10,             // Type: 16
        0x00, 0x03,             // Length: 3
        0x01,                   // Radio ID: 1
        0x00, 0x78,             // Report Interval: 120 секунд

        // --- Обязательный: Idle Timeout (Type 23) ---
        0x00, 0x17,             // Type: 23
        0x00, 0x04,             // Length: 4
        0x00, 0x00, 0x01, 0x2C, // Timeout: 300 секунд

        // --- Обязательный: WTP Fallback (Type 40) ---
        0x00, 0x28,             // Type: 40
        0x00, 0x01,             // Length: 1
        0x01,                   // Mode: 1 (Enabled)

        // --- Обязательный: AC IPv4 List (Type 2) ---
        0x00, 0x02,             // Type: 2
        0x00, 0x08,             // Length: 8 (для двух адресов)
        0x0A, 0x0A, 0x00, 0x01, // AC IP Address[0]: 10.10.0.1
        0x0A, 0x0A, 0x00, 0x02, // AC IP Address[1]: 10.10.0.2


        // 6. Unknown (5 байт)
        0xFF, 0xFF, 0x00, 0x01, 0x00,
        // 7. Unknown (5 байт)
        0xFF, 0xFE, 0x00, 0x01, 0x00,
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableConfigurationStatusResponse read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, read_data.unknown_elements);
}
