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

    {
        WritableCAPWAPTimers capwap_timers{ 42, 19 };

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

        WritableWTPStaticIPAddressInformation wtp_static_ipaddress{ inet_addr("192.168.100.10"),
                                                                    inet_addr("255.255.255.0"),
                                                                    inet_addr("192.168.1.1"),
                                                                    true };

        WritableVendorSpecificPayloadArray vendor_specific_payloads;
        vendor_specific_payloads.Add(123456, 789, "01234567890ABCDEF0123");

        WritableConfigurationStatusResponse write_data(
            capwap_timers,
            decryption_error_report_periods,
            idle_timeout,
            wtp_fallback,
            ac_ipv4_list,
            { &wtp_static_ipaddress, &vendor_specific_payloads });

        write_data.Serialize(&raw_data);
    }
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

    ReadableWTPStaticIPAddressInformation wtp_static_ipaddress;
    ReadableVendorSpecificPayloadArray vendor_specific_payloads;
    ReadableConfigurationStatusResponse read_data{ &wtp_static_ipaddress,
                                                   &vendor_specific_payloads };

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(42, read_data.capwap_timers.Get()->Discovery);
    CHECK_EQUAL(19, read_data.capwap_timers.Get()->EchoInterval);

    CHECK_EQUAL(3, read_data.decryption_error_report_periods.Get().size());
    CHECK_EQUAL(0, read_data.decryption_error_report_periods.Get()[0]->RadioID());
    CHECK_EQUAL(10, read_data.decryption_error_report_periods.Get()[0]->ReportInterval());
    CHECK_EQUAL(1, read_data.decryption_error_report_periods.Get()[1]->RadioID());
    CHECK_EQUAL(100, read_data.decryption_error_report_periods.Get()[1]->ReportInterval());
    CHECK_EQUAL(31, read_data.decryption_error_report_periods.Get()[2]->RadioID());
    CHECK_EQUAL(65535, read_data.decryption_error_report_periods.Get()[2]->ReportInterval());

    CHECK_TRUE(read_data.idle_timeout.IsPresent());
    CHECK_EQUAL(1234, read_data.idle_timeout.Get()->GetTimeout());
    CHECK_EQUAL(WTPFallback::Mode::Enabled, read_data.wtp_fallback.Get()->mode);

    CHECK_TRUE(read_data.ac_ipv4_list.IsPresent());
    CHECK_EQUAL(3, read_data.ac_ipv4_list.Get()->GetCount());
    CHECK_EQUAL(inet_addr("192.168.1.110"), read_data.ac_ipv4_list.Get()->addresses[0]);
    CHECK_EQUAL(inet_addr("192.168.1.111"), read_data.ac_ipv4_list.Get()->addresses[1]);
    CHECK_EQUAL(inet_addr("192.168.1.112"), read_data.ac_ipv4_list.Get()->addresses[2]);

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

    ReadableWTPStaticIPAddressInformation wtp_static_ipaddress;
    ReadableVendorSpecificPayloadArray vendor_specific_payloads;
    ReadableConfigurationStatusResponse read_data{ &wtp_static_ipaddress,
                                                   &vendor_specific_payloads };

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);

    CHECK_EQUAL(20, read_data.capwap_timers.Get()->Discovery);
    CHECK_EQUAL(30, read_data.capwap_timers.Get()->EchoInterval);

    CHECK_EQUAL(1, read_data.decryption_error_report_periods.Get().size());
    CHECK_EQUAL(1, read_data.decryption_error_report_periods.Get()[0]->RadioID());
    CHECK_EQUAL(120, read_data.decryption_error_report_periods.Get()[0]->ReportInterval());

    CHECK_TRUE(read_data.idle_timeout.IsPresent());
    CHECK_EQUAL(300, read_data.idle_timeout.Get()->GetTimeout());
    CHECK_EQUAL(WTPFallback::Mode::Enabled, read_data.wtp_fallback.Get()->mode);

    CHECK_TRUE(read_data.ac_ipv4_list.IsPresent());
    CHECK_EQUAL(2, read_data.ac_ipv4_list.Get()->GetCount());
    CHECK_EQUAL(inet_addr("10.10.0.1"), read_data.ac_ipv4_list.Get()->addresses[0]);
    CHECK_EQUAL(inet_addr("10.10.0.2"), read_data.ac_ipv4_list.Get()->addresses[1]);

    CHECK_TRUE(wtp_static_ipaddress.IsPresent());
    CHECK_EQUAL(inet_addr("192.168.10.50"), wtp_static_ipaddress.Get()->IpAddress);
    CHECK_EQUAL(inet_addr("255.255.255.0"), wtp_static_ipaddress.Get()->Netmask);
    CHECK_EQUAL(inet_addr("192.168.10.1"), wtp_static_ipaddress.Get()->Gateway);
    CHECK_EQUAL(1, wtp_static_ipaddress.Get()->Static);

    CHECK_FALSE(vendor_specific_payloads.IsPresent());
    CHECK_EQUAL(0, vendor_specific_payloads.Get().size());
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

    ReadableConfigurationStatusResponse read_data({});

    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, read_data.unknown_elements);
}

TEST(ConfigurationStatusResponseTestsGroup, IEEE80211_specific_message_elements) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    std::vector<uint8_t> rates_0 = { 0x82, 0x84 };
    std::vector<int16_t> levels_0 = { 20 };
    std::vector<uint8_t> rate_set1 = { 0x82, 0x84 };
    const uint8_t bssid1[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x01 };
    {
        WritableCAPWAPTimers capwap_timers{ 42, 19 };

        WritableDecryptionErrorReportPeriodArray decryption_error_report_periods;
        decryption_error_report_periods.Add({ 0, 10 });

        uint32_t idle_timeout = 1234;

        WTPFallback::Mode wtp_fallback = WTPFallback::Mode::Enabled;

        uint32_t ac_ipv4_list[] = {
            { inet_addr("192.168.1.110") },
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

        WritableRateSetArray w_rate_sets;
        w_rate_sets.Add(1, rate_set1);

        WritableSupportedRatesArray w_rates;
        w_rates.Add({ 1, rates_0 });

        WritableTxPowerArray w_tps;
        w_tps.Add({ 1, 20 });

        WritableWTPQualityOfServiceArray w_qos;

        // P bit set (802.1p enabled) = 0x10
        WTPQualityOfService elem1{ 1, 0x10 };
        elem1.Voice.QueueDepth = 4;
        elem1.Voice.CWMin = NetworkU16{ 15 };
        elem1.Voice.CWMax = NetworkU16{ 31 };
        w_qos.Add(elem1);

        WritableWTPRadioConfigurationArray w_configs;
        w_configs.Add({ 1, 1, 4, 2, bssid1, 100, "US " });

        WritableConfigurationStatusResponse write_data(capwap_timers,
                                                       decryption_error_report_periods,
                                                       idle_timeout,
                                                       wtp_fallback,
                                                       ac_ipv4_list,
                                                       { &w_antennas,
                                                         &w_ctrls,
                                                         &w_ops,
                                                         &w_capabilities,
                                                         &w_controls,
                                                         &w_rate_sets,
                                                         &w_rates,
                                                         &w_tps,
                                                         &w_qos,
                                                         &w_configs });

        write_data.Serialize(&raw_data);
    }

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableAntennaArray r_antennas;
    ReadableDirectSequenceControlArray r_ctrls;
    ReadableMACOperationArray r_ops;
    ReadableMultiDomainCapabilityArray r_capabilities;
    ReadableOFDMControlArray r_controls;
    ReadableRateSetArray r_rate_sets;
    ReadableSupportedRatesArray r_rates;
    ReadableTxPowerArray r_tps;
    ReadableWTPQualityOfServiceArray r_qos;
    ReadableWTPRadioConfigurationArray r_configs;
    ReadableConfigurationStatusResponse read_data{ &r_antennas,     &r_ctrls,    &r_ops,
                                                   &r_capabilities, &r_controls, &r_rate_sets,
                                                   &r_rates,        &r_tps,      &r_qos,
                                                   &r_configs };

    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_TRUE(read_data.capwap_timers.IsPresent());
    CHECK_TRUE(read_data.decryption_error_report_periods.IsPresent());
    CHECK_TRUE(read_data.idle_timeout.IsPresent());
    CHECK_TRUE(read_data.ac_ipv4_list.IsPresent());

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

    CHECK_TRUE(r_rate_sets.IsPresent());
    CHECK_EQUAL(1, r_rate_sets.Get().size());
    CHECK_EQUAL(1, r_rate_sets.Get()[0]->GetRadioID());
    CHECK_EQUAL(2 + 1, r_rate_sets.Get()[0]->GetLength());
    MEMCMP_EQUAL(rate_set1.data(), r_rate_sets.Get()[0]->data, 2);

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

    CHECK_TRUE(r_qos.IsPresent());
    CHECK_EQUAL(1, r_qos.Get().size());
    CHECK_EQUAL(1, r_qos.Get()[0]->RadioID);
    CHECK_EQUAL(0x10, r_qos.Get()[0]->TaggingPolicy);
    CHECK_TRUE(r_qos.Get()[0]->GetP());
    CHECK_FALSE(r_qos.Get()[0]->GetD());
    CHECK_EQUAL(4, r_qos.Get()[0]->Voice.QueueDepth);
    CHECK_EQUAL(15, r_qos.Get()[0]->Voice.CWMin.Get());
    CHECK_EQUAL(31, r_qos.Get()[0]->Voice.CWMax.Get());

    CHECK_TRUE(r_tps.IsPresent());
    CHECK_EQUAL(1, r_tps.Get().size());
    CHECK_EQUAL(1, r_tps.Get()[0]->RadioID);
    CHECK_EQUAL(20, r_tps.Get()[0]->CurrentTxPower.Get());

    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(ConfigurationStatusResponseTestsGroup, GetOptionalElement) {
    ReadableVendorSpecificPayloadArray vendor_specific_payloads;
    ReadableConfigurationStatusResponse read_data({ &vendor_specific_payloads });

    CHECK_EQUAL(&vendor_specific_payloads,
                read_data.GetOptionalElement<ReadableVendorSpecificPayloadArray>(
                    ElementHeader::VendorSpecificPayload));

    CHECK(read_data.GetOptionalElement<IReadableElement>((ElementHeader::ElementType)0xFFFF)
          == nullptr);
}

TEST(ConfigurationStatusResponseTestsGroup, MessageTypeIdentification) {
    WritableCAPWAPTimers capwap_timers{ 42, 19 };
    WritableDecryptionErrorReportPeriodArray decryption_error_report_periods;

    uint32_t idle_timeout = 1234;
    WTPFallback::Mode wtp_fallback = WTPFallback::Mode::Enabled;

    uint32_t ac_ipv4_list[] = {
        { inet_addr("192.168.1.110") },
    };

    WritableConfigurationStatusResponse write_data(capwap_timers,
                                                   decryption_error_report_periods,
                                                   idle_timeout,
                                                   wtp_fallback,
                                                   ac_ipv4_list,
                                                   {});

    CHECK_EQUAL(ControlHeader::ConfigurationStatusResponse, write_data.GetMessageType());
    CHECK_EQUAL(ControlHeader::ConfigurationStatusRequest, write_data.GetRequestMessageType());
}
