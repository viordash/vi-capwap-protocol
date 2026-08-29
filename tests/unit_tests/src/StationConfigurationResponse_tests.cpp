#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ClearHeader.h"
#include "StationConfigurationResponse.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(StationConfigurationResponseTestsGroup){ //
                                                    TEST_SETUP(){} 

                                                    TEST_TEARDOWN(){}
};

TEST(StationConfigurationResponseTestsGroup, StationConfigurationResponse_serialize_success) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    {
        WritableVendorSpecificPayloadArray vendor_specific_payloads;
        vendor_specific_payloads.Add(123456, 789, "01234567890ABCDEF0123");

        IWritableStationConfigurationResponseOptionalElement *const elems_1[] = {
            &vendor_specific_payloads
        };
        WritableStationConfigurationResponse write_data(ResultCode::Type::Success, elems_1);

        write_data.Serialize(&raw_data);
    }

    // ResultCode: ElementHeader(4) + Type(4) = 8
    // VendorSpecific: ElementHeader(4) + VendorId(4) + ElementId(2) + Data(21) = 31
    size_t expected_size = 8 + 31;
    CHECK_EQUAL(&buffer[0] + expected_size, raw_data.current);

    // Verify ResultCode wire format: type 0x0021, length 0x0004, value 0x00000000
    const uint8_t result_code_ref[] = { 0x00, 0x21, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00 };
    MEMCMP_EQUAL(result_code_ref, buffer, sizeof(result_code_ref));

    raw_data = { buffer, buffer + expected_size };

    ReadableVendorSpecificPayloadArray vendor_specific_payloads;
    IReadableStationConfigurationResponseOptionalElement *const elems_2[] = {
        &vendor_specific_payloads
    };
    ReadableStationConfigurationResponse read_data(elems_2);

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_TRUE(read_data.result_code.IsPresent());
    CHECK_EQUAL(ResultCode::Type::Success, read_data.result_code.Get()->type);

    CHECK_TRUE(vendor_specific_payloads.IsPresent());
    CHECK_EQUAL(1, vendor_specific_payloads.Get().size());
    CHECK_EQUAL(123456, vendor_specific_payloads.Get()[0]->GetVendorIdentifier());
    CHECK_EQUAL(789, vendor_specific_payloads.Get()[0]->GetElementId());
    STRNCMP_EQUAL("01234567890ABCDEF0123", (char *)vendor_specific_payloads.Get()[0]->value, 21);

    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(StationConfigurationResponseTestsGroup,
     StationConfigurationResponse_deserialize_from_binary) {
    // clang-format off
    uint8_t data[] = {
        // CAPWAP Header (8 bytes)
        0x00,       // Preamble: Version 0, Type 0
        0x10,       // HLEN: 2 (8 bytes), RID high bits
        0x42,       // RID low bits, WBID: 1 (802.11), T=0
        0x00,       // F=0, L=0, W=0, M=0, K=0, Flags=0
        0x00, 0x00, // Fragment ID
        0x00, 0x00, // Fragment Offset

        // CAPWAP Control Message Header (8 bytes)
        0x1A, 0x00, 0x00, 0x00, // Message Type: StationConfigurationResponse
        0x01,                   // Sequence Number: 1
        0x00, 0x08,             // Message Element Length: 8
        0x00,                   // Flags: 0

        // ResultCode element (type 0x0021, length 0x0004)
        0x00, 0x21, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)),
                      data + sizeof(data) };

    ReadableStationConfigurationResponse read_data(
        nonstd::span<IReadableStationConfigurationResponseOptionalElement *const>{});

    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);

    CHECK_TRUE(read_data.result_code.IsPresent());
    CHECK_EQUAL(ResultCode::Type::Success, read_data.result_code.Get()->type);
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(StationConfigurationResponseTestsGroup,
     StationConfigurationResponse_deserialize_handle_unknown_element) {
    // clang-format off
    uint8_t data[] = {
        // CAPWAP Header (8 bytes)
        0x00,       // Preamble: Version 0, Type 0
        0x10,       // HLEN: 2 (8 bytes), RID high bits
        0x42,       // RID low bits, WBID: 1 (802.11), T=0
        0x00,       // F=0, L=0, W=0, M=0, K=0, Flags=0
        0x00, 0x00, // Fragment ID
        0x00, 0x00, // Fragment Offset

        // CAPWAP Control Message Header (8 bytes)
        0x1A, 0x00, 0x00, 0x00, // Message Type: StationConfigurationResponse
        0x01,                   // Sequence Number: 1
        0x00, 18,               // Message Element Length
        0x00,                   // Flags: 0

        // ResultCode element (type 0x0021, length 0x0004)
        0x00, 0x21, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,

        // Unknown (5 bytes)
        0xFF, 0xFF, 0x00, 0x01, 0x00,
        // Unknown (5 bytes)
        0xFF, 0xFE, 0x00, 0x01, 0x00,
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)),
                      data + sizeof(data) };

    ReadableStationConfigurationResponse read_data(
        nonstd::span<IReadableStationConfigurationResponseOptionalElement *const>{});

    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, read_data.unknown_elements);
}

TEST(StationConfigurationResponseTestsGroup, GetOptionalElement) {
    ReadableVendorSpecificPayloadArray vendor_specific_payloads;
    IReadableStationConfigurationResponseOptionalElement *const elems_5[] = {
        &vendor_specific_payloads
    };
    ReadableStationConfigurationResponse read_data(elems_5);

    CHECK_EQUAL(&vendor_specific_payloads,
                read_data.GetOptionalElement<ReadableVendorSpecificPayloadArray>(
                    ElementHeader::VendorSpecificPayload));

    CHECK(read_data.GetOptionalElement<IReadableElement>((ElementHeader::ElementType)0xFFFF)
          == nullptr);
}

TEST(StationConfigurationResponseTestsGroup, MessageTypeIdentification) {
    WritableStationConfigurationResponse write_data(
        ResultCode::Type::Success,
        nonstd::span<IWritableStationConfigurationResponseOptionalElement *const>{});

    CHECK_EQUAL(ControlHeader::StationConfigurationResponse, write_data.GetMessageType());
    CHECK_EQUAL(ControlHeader::StationConfigurationRequest, write_data.GetRequestMessageType());
}

TEST(StationConfigurationResponseTestsGroup,
     StationConfigurationResponse_deserialize_returns_false_without_result_code) {
    // clang-format off
    uint8_t data[] = {
        // Unknown element only, no ResultCode
        0xFF, 0xFF, 0x00, 0x01, 0x00,
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };

    ReadableStationConfigurationResponse read_data(
        nonstd::span<IReadableStationConfigurationResponseOptionalElement *const>{});

    CHECK_FALSE(read_data.Deserialize(&raw_data));
}
