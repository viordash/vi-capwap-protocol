#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ClearHeader.h"
#include "WlanConfigurationResponse.h"
#include "elements/IEEE80211/AssignedWtpBssid.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(WlanConfigurationResponseTestsGroup){ //
                                                 TEST_SETUP(){}

                                                 TEST_TEARDOWN(){}
};

TEST(WlanConfigurationResponseTestsGroup, WlanConfigurationResponse_serialize_success) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    {
        WritableVendorSpecificPayloadArray vendor_specific_payloads;
        vendor_specific_payloads.Add(123456, 789, "01234567890ABCDEF0123");

        IWritableWlanConfigurationResponseOptionalElement *const elems_1[] = { &vendor_specific_payloads };
        WritableWlanConfigurationResponse write_data(ResultCode::Type::Success,
            elems_1);

        write_data.Serialize(&raw_data);
    }
    CHECK_EQUAL(&buffer[0] + 55 - (sizeof(ClearHeader) + sizeof(ControlHeader)), raw_data.current);
    const uint8_t reference[] = {
        0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xDD, 0x33, 0x00,
        0x04, 0x00, 0x00, 0x21, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x25, 0x00, 0x1B,
        0x00, 0x01, 0xE2, 0x40, 0x03, 0x15, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32, 0x33
    };

    MEMCMP_EQUAL(buffer,
                 reference + (sizeof(ClearHeader) + sizeof(ControlHeader)),
                 sizeof(reference) - (sizeof(ClearHeader) + sizeof(ControlHeader)));

    raw_data = { buffer, buffer + 55 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };

    ReadableVendorSpecificPayloadArray vendor_specific_payloads;
    IReadableWlanConfigurationResponseOptionalElement *const elems_2[] = { &vendor_specific_payloads };
    ReadableWlanConfigurationResponse read_data(elems_2);

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

TEST(WlanConfigurationResponseTestsGroup, WlanConfigurationResponse_serialize_with_assigned_bssid) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    {
        uint8_t bssid1[] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
        uint8_t bssid2[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

        WritableAssignedWtpBssidArray assigned_bssid;
        assigned_bssid.Add({ 1, 2, bssid1 });
        assigned_bssid.Add({ 3, 4, bssid2 });

        IWritableWlanConfigurationResponseOptionalElement *const elems_3[] = { &assigned_bssid };
        WritableWlanConfigurationResponse write_data(ResultCode::Type::Success,
            elems_3);

        write_data.Serialize(&raw_data);
    }
    CHECK_EQUAL(&buffer[0] + 48 - (sizeof(ClearHeader) + sizeof(ControlHeader)), raw_data.current);
    const uint8_t reference[] = { 0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x02, 0xDD, 0x33, 0x00, 0x1C, 0x00, 0x00, 0x21, 0x00, 0x04,
                                  0x00, 0x00, 0x00, 0x00, 0x04, 0x02, 0x00, 0x08, 0x01, 0x02,
                                  0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x04, 0x02, 0x00, 0x08,
                                  0x03, 0x04, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

    MEMCMP_EQUAL(buffer,
                 reference + (sizeof(ClearHeader) + sizeof(ControlHeader)),
                 sizeof(reference) - (sizeof(ClearHeader) + sizeof(ControlHeader)));

    raw_data = { buffer, buffer + 48 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };

    ReadableAssignedWtpBssidArray assigned_bssid;
    IReadableWlanConfigurationResponseOptionalElement *const elems_4[] = { &assigned_bssid };
    ReadableWlanConfigurationResponse read_data(elems_4);

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_TRUE(read_data.result_code.IsPresent());
    CHECK_EQUAL(ResultCode::Type::Success, read_data.result_code.Get()->type);

    CHECK_TRUE(assigned_bssid.IsPresent());
    CHECK_EQUAL(2, assigned_bssid.Get().size());
    CHECK_EQUAL(1, assigned_bssid.Get()[0]->RadioID);
    CHECK_EQUAL(2, assigned_bssid.Get()[0]->WlanID);
    MEMCMP_EQUAL(reference + (sizeof(ClearHeader) + sizeof(ControlHeader)) + 8 + 4 + 2,
                 assigned_bssid.Get()[0]->BSSID,
                 6);
    CHECK_EQUAL(3, assigned_bssid.Get()[1]->RadioID);
    CHECK_EQUAL(4, assigned_bssid.Get()[1]->WlanID);
    MEMCMP_EQUAL(reference + (sizeof(ClearHeader) + sizeof(ControlHeader)) + 8 + 4 + 2 + 12,
                 assigned_bssid.Get()[1]->BSSID,
                 6);

    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(WlanConfigurationResponseTestsGroup, WlanConfigurationResponse_deserialize_from_binary) {
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
        0x02, 0xDD, 0x33, 0x00, // Message Type: WlanConfigurationResponse
        0x01,                   // Sequence Number: 1
        0x00, 0x08,             // Message Element Length: 8
        0x00,                   // Flags: 0

        // ResultCode element (type 0x0021, length 0x0004)
        0x00, 0x21, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableWlanConfigurationResponse read_data(nonstd::span<IReadableWlanConfigurationResponseOptionalElement *const>{});

    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);

    CHECK_TRUE(read_data.result_code.IsPresent());
    CHECK_EQUAL(ResultCode::Type::Success, read_data.result_code.Get()->type);
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(WlanConfigurationResponseTestsGroup,
     WlanConfigurationResponse_deserialize_handle_unknown_element) {
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
        0x02, 0xDD, 0x33, 0x00, // Message Type: WlanConfigurationResponse
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
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableWlanConfigurationResponse read_data(nonstd::span<IReadableWlanConfigurationResponseOptionalElement *const>{});

    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, read_data.unknown_elements);
}
TEST(WlanConfigurationResponseTestsGroup, GetOptionalElement) {
    ReadableVendorSpecificPayloadArray vendor_specific_payloads;
    IReadableWlanConfigurationResponseOptionalElement *const elems_5[] = { &vendor_specific_payloads };
    ReadableWlanConfigurationResponse read_data(elems_5);

    CHECK_EQUAL(&vendor_specific_payloads,
                read_data.GetOptionalElement<ReadableVendorSpecificPayloadArray>(
                    ElementHeader::VendorSpecificPayload));

    CHECK(read_data.GetOptionalElement<IReadableElement>((ElementHeader::ElementType)0xFFFF) ==
          nullptr);
}

TEST(WlanConfigurationResponseTestsGroup, MessageTypeIdentification) {
    WritableWlanConfigurationResponse write_data(ResultCode::Type::Success, nonstd::span<IWritableWlanConfigurationResponseOptionalElement *const>{});

    CHECK_EQUAL(ControlHeader::WlanConfigurationResponse, write_data.GetMessageType());
    CHECK_EQUAL(ControlHeader::WlanConfigurationRequest, write_data.GetRequestMessageType());
}
