#include "nanobench.h"
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

TEST(ConfigurationUpdateRequestTestsGroup, ConfigurationUpdateRequest_serialize_deserialize_perf) {
    ankerl::nanobench::Bench b;

    b.title("ConfigurationUpdateRequest").warmup(1000).minEpochIterations(150000).relative(true);
    b.performanceCounters(true);

    uint8_t buffer[4096] = {};

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

    b.run("serialization", [&] {
        RawData raw_data{ buffer, buffer + sizeof(buffer) };
        write_data.Serialize(&raw_data);
        ankerl::nanobench::doNotOptimizeAway(raw_data);

        raw_data = { buffer, buffer + 260 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };
        ReadableConfigurationUpdateRequest read_data;
        CHECK_TRUE(read_data.Deserialize(&raw_data));
        ankerl::nanobench::doNotOptimizeAway(raw_data);
        CHECK_EQUAL(0, read_data.unknown_elements);
    });

    const uint8_t reference[] = {
        0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x2A, 0x00, 0xF4,
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

    b.run("reference sum", [&] {
        size_t sum = 0;
        for (size_t i = 0; i < sizeof(reference); i++) {
            sum += reference[i];
        }
        ankerl::nanobench::doNotOptimizeAway(sum);
    });
}
