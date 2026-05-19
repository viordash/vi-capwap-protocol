#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ClearHeader.h"
#include "StationConfigurationRequest.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(StationConfigurationRequestTestsGroup){ //
                                                   TEST_SETUP(){} 

                                                   TEST_TEARDOWN(){}
};

TEST(StationConfigurationRequestTestsGroup,
     StationConfigurationRequest_serialize_add_station_eui48) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    {
        WritableAddStationArray add_station;
        uint8_t mac1[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
        add_station.Add(1, MacAddress(nonstd::span<const uint8_t>(mac1, 6)));

        WritableStationConfigurationRequest write_data(
            &add_station,
            nonstd::span<IWritableStationConfigurationRequestOptionalElement *const>{});

        write_data.Serialize(&raw_data);
    }

    // AddStation: ElementHeader(4) + RadioID(1) + MACLength(1) + MAC(6) = 12 bytes
    size_t expected_size = 12;
    CHECK_EQUAL(&buffer[0] + expected_size, raw_data.current);

    // Verify wire format:
    // ElementType = 0x0800 (AddStation = 8), Length = 0x0008 (8 bytes of value)
    // RadioID = 0x01, MACLength = 0x06, MAC = 00:1A:2B:3C:4D:5E
    const uint8_t reference[] = { 0x00, 0x08, 0x00, 0x08, 0x01, 0x06,
                                  0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    MEMCMP_EQUAL(reference, buffer, sizeof(reference));

    // Deserialize
    raw_data = { buffer, buffer + expected_size };

    ReadableStationConfigurationRequest read_data(
        nonstd::span<IReadableStationConfigurationRequestOptionalElement *const>{});

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_TRUE(read_data.add_station.IsPresent());
    CHECK_FALSE(read_data.delete_station.IsPresent());
    CHECK_EQUAL(1, read_data.add_station.Get().size());
    CHECK_EQUAL(1, read_data.add_station.Get()[0]->RadioID);
    CHECK_EQUAL(6, read_data.add_station.Get()[0]->MACAddress.Length);
    uint8_t expected_mac[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    MEMCMP_EQUAL(
        expected_mac,
        read_data.add_station.Get()[0]->MACAddress.MACAddresses,
        6);
    CHECK_EQUAL(0, read_data.add_station.Get()[0]->GetVlanNameLength());
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(StationConfigurationRequestTestsGroup,
     StationConfigurationRequest_serialize_add_station_with_vlan) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    {
        WritableAddStationArray add_station;
        uint8_t mac1[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
        add_station.Add(
            1, MacAddress(nonstd::span<const uint8_t>(mac1, 6)), "VLAN100");

        WritableStationConfigurationRequest write_data(
            &add_station,
            nonstd::span<IWritableStationConfigurationRequestOptionalElement *const>{});

        write_data.Serialize(&raw_data);
    }

    // AddStation: ElementHeader(4) + RadioID(1) + MACLength(1) + MAC(6) + VLAN(7) = 19 bytes
    size_t expected_size = 19;
    CHECK_EQUAL(&buffer[0] + expected_size, raw_data.current);

    // ElementType = 0x0800, Length = 0x000F (15 bytes of value: 1+1+6+7)
    const uint8_t reference[] = { 0x00, 0x08, 0x00, 0x0F, 0x01, 0x06, 0x00, 0x1A, 0x2B, 0x3C,
                                  0x4D, 0x5E, 0x56, 0x4C, 0x41, 0x4E, 0x31, 0x30, 0x30 };
    MEMCMP_EQUAL(reference, buffer, sizeof(reference));

    // Deserialize
    raw_data = { buffer, buffer + expected_size };

    ReadableStationConfigurationRequest read_data(
        nonstd::span<IReadableStationConfigurationRequestOptionalElement *const>{});

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_TRUE(read_data.add_station.IsPresent());
    CHECK_EQUAL(1, read_data.add_station.Get().size());
    CHECK_EQUAL(7, read_data.add_station.Get()[0]->GetVlanNameLength());
    STRNCMP_EQUAL(
        "VLAN100",
        (const char *)(read_data.add_station.Get()[0]->MACAddress.MACAddresses
                       + read_data.add_station.Get()[0]->MACAddress.Length),
        7);
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(StationConfigurationRequestTestsGroup,
     StationConfigurationRequest_serialize_add_station_eui64) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    {
        WritableAddStationArray add_station;
        uint8_t mac1[] = { 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD, 0xEE, 0xFF };
        add_station.Add(2, MacAddress(nonstd::span<const uint8_t>(mac1, 8)));

        WritableStationConfigurationRequest write_data(
            &add_station,
            nonstd::span<IWritableStationConfigurationRequestOptionalElement *const>{});

        write_data.Serialize(&raw_data);
    }

    // AddStation: ElementHeader(4) + RadioID(1) + MACLength(1) + MAC(8) = 14 bytes
    size_t expected_size = 14;
    CHECK_EQUAL(&buffer[0] + expected_size, raw_data.current);

    // Deserialize
    raw_data = { buffer, buffer + expected_size };

    ReadableStationConfigurationRequest read_data(
        nonstd::span<IReadableStationConfigurationRequestOptionalElement *const>{});

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_TRUE(read_data.add_station.IsPresent());
    CHECK_EQUAL(1, read_data.add_station.Get().size());
    CHECK_EQUAL(2, read_data.add_station.Get()[0]->RadioID);
    CHECK_EQUAL(8, read_data.add_station.Get()[0]->MACAddress.Length);
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(StationConfigurationRequestTestsGroup,
     StationConfigurationRequest_serialize_delete_station) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    {
        WritableDeleteStationArray delete_station;
        uint8_t mac1[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
        uint8_t mac2[] = { 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD, 0xEE, 0xFF };
        delete_station.Add(7, MacAddress(nonstd::span<const uint8_t>(mac1, 6)));
        delete_station.Add(8, MacAddress(nonstd::span<const uint8_t>(mac2, 8)));

        WritableStationConfigurationRequest write_data(
            &delete_station,
            nonstd::span<IWritableStationConfigurationRequestOptionalElement *const>{});

        write_data.Serialize(&raw_data);
    }

    // DeleteStation #1: ElementHeader(4) + RadioID(1) + MACLength(1) + MAC(6) = 12
    // DeleteStation #2: ElementHeader(4) + RadioID(1) + MACLength(1) + MAC(8) = 14
    size_t expected_size = 12 + 14;
    CHECK_EQUAL(&buffer[0] + expected_size, raw_data.current);

    // Deserialize
    raw_data = { buffer, buffer + expected_size };

    ReadableStationConfigurationRequest read_data(
        nonstd::span<IReadableStationConfigurationRequestOptionalElement *const>{});

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_FALSE(read_data.add_station.IsPresent());
    CHECK_TRUE(read_data.delete_station.IsPresent());
    CHECK_EQUAL(2, read_data.delete_station.Get().size());
    CHECK_EQUAL(7, read_data.delete_station.Get()[0]->RadioID);
    CHECK_EQUAL(8, read_data.delete_station.Get()[1]->RadioID);
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(StationConfigurationRequestTestsGroup,
     StationConfigurationRequest_serialize_add_station_with_vendor) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    {
        WritableAddStationArray add_station;
        uint8_t mac1[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
        add_station.Add(1, MacAddress(nonstd::span<const uint8_t>(mac1, 6)));

        WritableVendorSpecificPayloadArray vendor_specific_payloads;
        vendor_specific_payloads.Add(123456, 789, "test_payload");

        IWritableStationConfigurationRequestOptionalElement *const elems_1[] = {
            &vendor_specific_payloads
        };
        WritableStationConfigurationRequest write_data(&add_station, elems_1);

        write_data.Serialize(&raw_data);
    }

    size_t serialized_size = raw_data.current - buffer;
    CHECK(serialized_size > 12); // At least AddStation + VendorSpecific

    raw_data = { buffer, buffer + serialized_size };

    ReadableVendorSpecificPayloadArray vendor_specific_payloads;
    IReadableStationConfigurationRequestOptionalElement *const elems_2[] = {
        &vendor_specific_payloads
    };
    ReadableStationConfigurationRequest read_data(elems_2);

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_TRUE(read_data.add_station.IsPresent());
    CHECK_FALSE(read_data.delete_station.IsPresent());
    CHECK_EQUAL(1, read_data.add_station.Get().size());

    CHECK_TRUE(vendor_specific_payloads.IsPresent());
    CHECK_EQUAL(1, vendor_specific_payloads.Get().size());
    CHECK_EQUAL(123456, vendor_specific_payloads.Get()[0]->GetVendorIdentifier());
    CHECK_EQUAL(789, vendor_specific_payloads.Get()[0]->GetElementId());
    STRNCMP_EQUAL("test_payload", (char *)vendor_specific_payloads.Get()[0]->value, 12);

    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(StationConfigurationRequestTestsGroup,
     StationConfigurationRequest_deserialize_handle_unknown_element) {
    // clang-format off
    uint8_t data[] = {
        // ---- CAPWAP Header (8 bytes) ----
        0x00,       // Preamble: Version 0, Type 0 (CAPWAP Header follows)
        0x10,       // HLEN: 2 (8 bytes), RID high bits
        0x42,       // RID low bits, WBID: 1 (802.11), T=0
        0x00,       // F=0, L=0, W=0, M=0, K=0, Flags=0
        0x00, 0x00, // Fragment ID
        0x00, 0x00, // Fragment Offset

        // ---- CAPWAP Control Message Header (8 bytes) ----
        0x19, 0x00, 0x00, 0x00, // Message Type: StationConfigurationRequest
        0x01,                   // Sequence Number: 1
        0x00, 12 + 10,          // Message Element Length: 12 + 10 bytes
        0x00,                   // Flags: 0

        // ---- Message Elements ----
        // AddStation element (type 0x0800, length 0x0008)
        0x00, 0x08, 0x00, 0x08, 0x01, 0x06,
        0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E,

        // Unknown (5 bytes)
        0xFF, 0xFF, 0x00, 0x01, 0x00,
        // Unknown (5 bytes)
        0xFF, 0xFE, 0x00, 0x01, 0x00,
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)),
                      data + sizeof(data) };

    ReadableStationConfigurationRequest read_data(
        nonstd::span<IReadableStationConfigurationRequestOptionalElement *const>{});

    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, read_data.unknown_elements);
}

TEST(StationConfigurationRequestTestsGroup, GetOptionalElement) {
    ReadableVendorSpecificPayloadArray vendor_specific_payloads;
    IReadableStationConfigurationRequestOptionalElement *const elems_3[] = {
        &vendor_specific_payloads
    };
    ReadableStationConfigurationRequest read_data(elems_3);

    CHECK_EQUAL(&vendor_specific_payloads,
                read_data.GetOptionalElement<ReadableVendorSpecificPayloadArray>(
                    ElementHeader::VendorSpecificPayload));

    CHECK(read_data.GetOptionalElement<IReadableElement>((ElementHeader::ElementType)0xFFFF)
          == nullptr);
}

TEST(StationConfigurationRequestTestsGroup, MessageTypeIdentification) {
    WritableAddStationArray add_station;
    WritableStationConfigurationRequest write_data(
        &add_station,
        nonstd::span<IWritableStationConfigurationRequestOptionalElement *const>{});

    CHECK_EQUAL(ControlHeader::StationConfigurationRequest, write_data.GetMessageType());
    CHECK_EQUAL(ControlHeader::StationConfigurationResponse, write_data.GetResponseMessageType());
}

TEST(StationConfigurationRequestTestsGroup,
     StationConfigurationRequest_deserialize_returns_false_with_add_and_delete) {
    // clang-format off
    uint8_t data[] = {
        // AddStation element (type 0x0800, length 0x0008)
        0x00, 0x08, 0x00, 0x08, 0x01, 0x06,
        0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E,
        // DeleteStation element (type 0x1200, length 0x0008)
        0x00, 0x12, 0x00, 0x08, 0x02, 0x06,
        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };

    ReadableStationConfigurationRequest read_data(
        nonstd::span<IReadableStationConfigurationRequestOptionalElement *const>{});

    CHECK_FALSE(read_data.Deserialize(&raw_data));
}

TEST(StationConfigurationRequestTestsGroup,
     StationConfigurationRequest_deserialize_returns_false_with_no_elements) {
    uint8_t data[] = {};
    RawData raw_data{ data, data + sizeof(data) };

    ReadableStationConfigurationRequest read_data(
        nonstd::span<IReadableStationConfigurationRequestOptionalElement *const>{});

    CHECK_FALSE(read_data.Deserialize(&raw_data));
}

TEST(StationConfigurationRequestTestsGroup,
     StationConfigurationRequest_serialize_multiple_add_stations) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    {
        WritableAddStationArray add_station;
        uint8_t mac1[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
        uint8_t mac2[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
        add_station.Add(1, MacAddress(nonstd::span<const uint8_t>(mac1, 6)));
        add_station.Add(2, MacAddress(nonstd::span<const uint8_t>(mac2, 6)), "VLAN200");

        WritableStationConfigurationRequest write_data(
            &add_station,
            nonstd::span<IWritableStationConfigurationRequestOptionalElement *const>{});

        write_data.Serialize(&raw_data);
    }

    // AddStation #1: 4 + 1 + 1 + 6 = 12
    // AddStation #2: 4 + 1 + 1 + 6 + 7 = 19
    size_t expected_size = 12 + 19;
    CHECK_EQUAL(&buffer[0] + expected_size, raw_data.current);

    raw_data = { buffer, buffer + expected_size };

    ReadableStationConfigurationRequest read_data(
        nonstd::span<IReadableStationConfigurationRequestOptionalElement *const>{});

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_TRUE(read_data.add_station.IsPresent());
    CHECK_EQUAL(2, read_data.add_station.Get().size());
    CHECK_EQUAL(1, read_data.add_station.Get()[0]->RadioID);
    CHECK_EQUAL(0, read_data.add_station.Get()[0]->GetVlanNameLength());
    CHECK_EQUAL(2, read_data.add_station.Get()[1]->RadioID);
    CHECK_EQUAL(7, read_data.add_station.Get()[1]->GetVlanNameLength());
    CHECK_EQUAL(0, read_data.unknown_elements);
}
