#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ClearHeader.h"
#include "WlanConfigurationRequest.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(WlanConfigurationRequestTestsGroup){ //
                                                TEST_SETUP(){}

                                                TEST_TEARDOWN(){}
};

TEST(WlanConfigurationRequestTestsGroup, WlanConfigurationRequest_serialize_add_wlan) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    {
        WritableAddWlanArray add_wlan;
        uint8_t group_tsc[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
        uint8_t key[] = { 0x11, 0x22, 0x33, 0x44 };
        add_wlan.Add({ 1,
                       2,
                       0x0421,
                       0,
                       AddWlanHeader::KeyStatus::PerStationKeys,
                       nonstd::span<const uint8_t>(key, sizeof(key)),
                       group_tsc,
                       AddWlanHeader::QoS::BestEffort,
                       AddWlanHeader::AuthType::OpenSystem,
                       AddWlanHeader::MACMode::SplitMAC,
                       AddWlanHeader::TunnelMode::Tunnel8023,
                       0,
                       "TestSSID" });

        WritableInformationElementArray info_elements;
        uint8_t ie_data[] = { 0xDD, 0x08, 0x00, 0x50, 0xF2, 0x01, 0x01, 0x00, 0x00, 0x50 };
        info_elements.Add({ 1, 2, InformationElement::FLAG_BEACON, { ie_data, sizeof(ie_data) } });

        WritableVendorSpecificPayloadArray vendor_specific_payloads;
        vendor_specific_payloads.Add(123456, 789, "01234567890ABCDEF0123");

        IWritableWlanConfigurationRequestOptionalElement *const elems_1[] = { &info_elements, &vendor_specific_payloads };
        WritableWlanConfigurationRequest write_data(&add_wlan,
            elems_1);

        write_data.Serialize(&raw_data);
    }
    CHECK_EQUAL(&buffer[0] + 99 - (sizeof(ClearHeader) + sizeof(ControlHeader)), raw_data.current);
    const uint8_t reference[] = {
        0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xDD, 0x33, 0x00, 0x2C,
        0x00, 0x04, 0x00, 0x00, 0x1F, 0x01, 0x02, 0x04, 0x21, 0x00, 0x00, 0x00, 0x04, 0x11, 0x22,
        0x33, 0x44, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x00, 0x01, 0x01, 0x00, 0x54, 0x65,
        0x73, 0x74, 0x53, 0x53, 0x49, 0x44, 0x04, 0x05, 0x00, 0x0D, 0x01, 0x02, 0x80, 0xDD, 0x08,
        0x00, 0x50, 0xF2, 0x01, 0x01, 0x00, 0x00, 0x50, 0x00, 0x25, 0x00, 0x1B, 0x00, 0x01, 0xE2,
        0x40, 0x03, 0x15, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41,
        0x42, 0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32, 0x33
    };

    MEMCMP_EQUAL(buffer,
                 reference + (sizeof(ClearHeader) + sizeof(ControlHeader)),
                 sizeof(reference) - (sizeof(ClearHeader) + sizeof(ControlHeader)));

    raw_data = { buffer, buffer + 99 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };

    ReadableInformationElementArray info_elements;
    ReadableVendorSpecificPayloadArray vendor_specific_payloads;
    IReadableWlanConfigurationRequestOptionalElement *const elems_2[] = { &info_elements, &vendor_specific_payloads };
    ReadableWlanConfigurationRequest read_data(elems_2);

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_TRUE(read_data.add_wlan.IsPresent());
    CHECK_EQUAL(1, read_data.add_wlan.Get().size());
    CHECK_EQUAL(1, read_data.add_wlan.Get()[0].header->GetRadioID());
    CHECK_EQUAL(2, read_data.add_wlan.Get()[0].header->GetWlanID());
    CHECK_EQUAL(0x0421, read_data.add_wlan.Get()[0].header->GetCapability());
    CHECK_EQUAL(0, read_data.add_wlan.Get()[0].header->GetKeyIndex());
    CHECK_EQUAL(AddWlanHeader::KeyStatus::PerStationKeys,
                read_data.add_wlan.Get()[0].header->GetKeyStatus());
    CHECK_EQUAL(4, read_data.add_wlan.Get()[0].header->GetKeyLength());
    CHECK_EQUAL(AddWlanHeader::QoS::BestEffort, read_data.add_wlan.Get()[0].tail->qos);
    CHECK_EQUAL(AddWlanHeader::AuthType::OpenSystem, read_data.add_wlan.Get()[0].tail->auth_type);
    CHECK_EQUAL(AddWlanHeader::MACMode::SplitMAC, read_data.add_wlan.Get()[0].tail->mac_mode);
    CHECK_EQUAL(AddWlanHeader::TunnelMode::Tunnel8023,
                read_data.add_wlan.Get()[0].tail->tunnel_mode);
    CHECK_EQUAL(0, read_data.add_wlan.Get()[0].tail->suppress_ssid);
    STRNCMP_EQUAL("TestSSID", read_data.add_wlan.Get()[0].ssid, 8);
    CHECK_EQUAL(8, read_data.add_wlan.Get()[0].ssid_length);

    CHECK_FALSE(read_data.delete_wlan.IsPresent());
    CHECK_FALSE(read_data.update_wlan.IsPresent());

    CHECK_TRUE(info_elements.IsPresent());
    CHECK_EQUAL(1, info_elements.Get().size());
    CHECK_EQUAL(1, info_elements.Get()[0]->GetRadioID());
    CHECK_EQUAL(2, info_elements.Get()[0]->GetWlanID());
    CHECK_TRUE(info_elements.Get()[0]->GetBeaconFlag());
    CHECK_FALSE(info_elements.Get()[0]->GetProbeResponseFlag());
    CHECK_EQUAL(10, info_elements.Get()[0]->GetIELength());

    CHECK_TRUE(vendor_specific_payloads.IsPresent());
    CHECK_EQUAL(1, vendor_specific_payloads.Get().size());
    CHECK_EQUAL(123456, vendor_specific_payloads.Get()[0]->GetVendorIdentifier());
    CHECK_EQUAL(789, vendor_specific_payloads.Get()[0]->GetElementId());
    STRNCMP_EQUAL("01234567890ABCDEF0123", (char *)vendor_specific_payloads.Get()[0]->value, 21);

    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(WlanConfigurationRequestTestsGroup, WlanConfigurationRequest_serialize_delete_wlan) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    {
        WritableDeleteWlanArray delete_wlan;
        delete_wlan.Add({ 1, 2 });
        delete_wlan.Add({ 3, 4 });

        WritableWlanConfigurationRequest write_data(&delete_wlan, nonstd::span<IWritableWlanConfigurationRequestOptionalElement *const>{});

        write_data.Serialize(&raw_data);
    }
    CHECK_EQUAL(&buffer[0] + 28 - (sizeof(ClearHeader) + sizeof(ControlHeader)), raw_data.current);
    const uint8_t reference[] = { 0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x01, 0xDD, 0x33, 0x00, 0x08, 0x00, 0x04, 0x03, 0x00, 0x02,
                                  0x01, 0x02, 0x04, 0x03, 0x00, 0x02, 0x03, 0x04 };

    MEMCMP_EQUAL(buffer,
                 reference + (sizeof(ClearHeader) + sizeof(ControlHeader)),
                 sizeof(reference) - (sizeof(ClearHeader) + sizeof(ControlHeader)));

    raw_data = { buffer, buffer + 28 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };

    ReadableWlanConfigurationRequest read_data(nonstd::span<IReadableWlanConfigurationRequestOptionalElement *const>{});

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_FALSE(read_data.add_wlan.IsPresent());
    CHECK_TRUE(read_data.delete_wlan.IsPresent());
    CHECK_FALSE(read_data.update_wlan.IsPresent());

    CHECK_EQUAL(2, read_data.delete_wlan.Get().size());
    CHECK_EQUAL(1, read_data.delete_wlan.Get()[0]->RadioID);
    CHECK_EQUAL(2, read_data.delete_wlan.Get()[0]->WlanID);
    CHECK_EQUAL(3, read_data.delete_wlan.Get()[1]->RadioID);
    CHECK_EQUAL(4, read_data.delete_wlan.Get()[1]->WlanID);

    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(WlanConfigurationRequestTestsGroup, WlanConfigurationRequest_serialize_update_wlan) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    {
        WritableUpdateWlanArray update_wlan;
        uint8_t key[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
        update_wlan.Add({ 2,
                          5,
                          0x1234,
                          1,
                          UpdateWlan::KeyStatus::BeginRekeying,
                          nonstd::span<const uint8_t>(key, sizeof(key)) });

        WritableWlanConfigurationRequest write_data(&update_wlan, nonstd::span<IWritableWlanConfigurationRequestOptionalElement *const>{});

        write_data.Serialize(&raw_data);
    }
    CHECK_EQUAL(&buffer[0] + 34 - (sizeof(ClearHeader) + sizeof(ControlHeader)), raw_data.current);
    const uint8_t reference[] = { 0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x01, 0xDD, 0x33, 0x00, 0x0A, 0x00, 0x04, 0x14,
                                  0x00, 0x0E, 0x02, 0x05, 0x12, 0x34, 0x01, 0x02, 0x00,
                                  0x06, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

    MEMCMP_EQUAL(buffer,
                 reference + (sizeof(ClearHeader) + sizeof(ControlHeader)),
                 sizeof(reference) - (sizeof(ClearHeader) + sizeof(ControlHeader)));

    raw_data = { buffer, buffer + 34 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };

    ReadableWlanConfigurationRequest read_data(nonstd::span<IReadableWlanConfigurationRequestOptionalElement *const>{});

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_FALSE(read_data.add_wlan.IsPresent());
    CHECK_FALSE(read_data.delete_wlan.IsPresent());
    CHECK_TRUE(read_data.update_wlan.IsPresent());

    CHECK_EQUAL(1, read_data.update_wlan.Get().size());
    CHECK_EQUAL(2, read_data.update_wlan.Get()[0]->GetRadioID());
    CHECK_EQUAL(5, read_data.update_wlan.Get()[0]->GetWlanID());
    CHECK_EQUAL(0x1234, read_data.update_wlan.Get()[0]->GetCapability());
    CHECK_EQUAL(1, read_data.update_wlan.Get()[0]->GetKeyIndex());
    CHECK_EQUAL(UpdateWlan::KeyStatus::BeginRekeying,
                read_data.update_wlan.Get()[0]->GetKeyStatus());
    CHECK_EQUAL(6, read_data.update_wlan.Get()[0]->GetKeyLength());
    uint8_t expected_key[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    MEMCMP_EQUAL(expected_key, read_data.update_wlan.Get()[0]->data, 6);

    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(WlanConfigurationRequestTestsGroup,
     WlanConfigurationRequest_deserialize_handle_unknown_element) {
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
        0x01, 0xDD, 0x33, 0x00, // Message Type: WlanConfigurationRequest
        0x01,                   // Sequence Number: 1
        0x00, 0x08 + 10,        // Message Element Length: 8 + 10 bytes
        0x00,                   // Flags: 0

        // ---- Message Elements ----
        // DeleteWlan element (type 0x0304)
        0x04, 0x03, 0x00, 0x02, 0x01, 0x02,

        // Unknown (5 bytes)
        0xFF, 0xFF, 0x00, 0x01, 0x00,
        // Unknown (5 bytes)
        0xFF, 0xFE, 0x00, 0x01, 0x00,
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableWlanConfigurationRequest read_data(nonstd::span<IReadableWlanConfigurationRequestOptionalElement *const>{});

    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, read_data.unknown_elements);
}
TEST(WlanConfigurationRequestTestsGroup, GetOptionalElement) {
    ReadableVendorSpecificPayloadArray vendor_specific_payloads;
    IReadableWlanConfigurationRequestOptionalElement *const elems_3[] = { &vendor_specific_payloads };
    ReadableWlanConfigurationRequest read_data(elems_3);

    CHECK_EQUAL(&vendor_specific_payloads,
                read_data.GetOptionalElement<ReadableVendorSpecificPayloadArray>(
                    ElementHeader::VendorSpecificPayload));

    CHECK(read_data.GetOptionalElement<IReadableElement>((ElementHeader::ElementType)0xFFFF) ==
          nullptr);
}

TEST(WlanConfigurationRequestTestsGroup, MessageTypeIdentification) {
    WritableAddWlanArray add_wlan;
    WritableWlanConfigurationRequest write_data(&add_wlan, nonstd::span<IWritableWlanConfigurationRequestOptionalElement *const>{});

    CHECK_EQUAL(ControlHeader::WlanConfigurationRequest, write_data.GetMessageType());
}
