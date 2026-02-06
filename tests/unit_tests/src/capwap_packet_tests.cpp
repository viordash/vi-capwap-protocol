#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ClearHeader.h"
#include "ControlHeader.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(CapwapHeaderTestsGroup){ //
                                    TEST_SETUP(){}

                                    TEST_TEARDOWN(){}
};

TEST(CapwapHeaderTestsGroup, Preamble_deserialize) {
    uint8_t preamble_raw_0[] = { 0x00 };
    RawData raw_data{ preamble_raw_0, preamble_raw_0 + sizeof(preamble_raw_0) };
    auto preamble = Preamble::Deserialize(&raw_data);
    CHECK(preamble != nullptr);
    CHECK_EQUAL(Preamble::RFC_5415, preamble->version);
    CHECK_EQUAL(Preamble::ClearText, preamble->type);

    uint8_t preamble_raw_1[] = { 0x01 };
    raw_data = { preamble_raw_1, preamble_raw_1 + sizeof(preamble_raw_1) };
    preamble = Preamble::Deserialize(&raw_data);
    CHECK(preamble != nullptr);
    CHECK_EQUAL(Preamble::RFC_5415, preamble->version);
    CHECK_EQUAL(Preamble::DTLSCrypted, preamble->type);

    uint8_t version_err[] = { 0x10 };
    raw_data = { version_err, version_err + sizeof(version_err) };
    preamble = Preamble::Deserialize(&raw_data);
    CHECK(preamble == nullptr);

    raw_data = { preamble_raw_0, preamble_raw_0 };
    preamble = Preamble::Deserialize(&raw_data);
    CHECK(preamble == nullptr);
}

TEST(CapwapHeaderTestsGroup, ClearHeader_deserialize) {
    uint8_t capwap_header[] = { 0x00, 0x86, 0x83, 0xA0, 0x42, 0x19, 0x03, 0x20 };
    RawData raw_data{ capwap_header, capwap_header + sizeof(capwap_header) };
    auto header = ClearHeader::Deserialize(&raw_data);
    CHECK(header != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(16, header->HLEN());
    CHECK_EQUAL(26, header->RID());
    CHECK_EQUAL(1, header->WBID());
    CHECK_TRUE(header->T());
    CHECK_TRUE(header->F());
    CHECK_FALSE(header->L());
    CHECK_TRUE(header->W());
    CHECK_FALSE(header->M());
    CHECK_FALSE(header->K());
    CHECK_EQUAL(16921, header->Fragment_ID());
    CHECK_EQUAL(100, header->Fragment_Offset());

    uint8_t capwap_header_wireshark[] = { 0x00, 0x20, 0x43, 0x20, 0x00, 0x00, 0x00, 0x00 };
    raw_data = { capwap_header_wireshark,
                 capwap_header_wireshark + sizeof(capwap_header_wireshark) };
    header = ClearHeader::Deserialize(&raw_data);
    CHECK(header != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(4, header->HLEN());
    CHECK_EQUAL(1, header->RID());
    CHECK_EQUAL(WBIDType::IEEE_80211, header->WBID());
    CHECK_TRUE(header->T());
    CHECK_FALSE(header->F());
    CHECK_FALSE(header->L());
    CHECK_TRUE(header->W());
    CHECK_FALSE(header->M());
    CHECK_FALSE(header->K());
    CHECK_EQUAL(0, header->Fragment_ID());
    CHECK_EQUAL(0, header->Fragment_Offset());
}

TEST(CapwapHeaderTestsGroup, ClearHeader_DTLSCrypted_returns_nullptr) {
    uint8_t capwap_header[] = { 0x01, 0x1E, 0x83, 0xA0, 0x42, 0x19, 0x03, 0x20 };
    RawData raw_data{ capwap_header, capwap_header + sizeof(capwap_header) };
    auto header = ClearHeader::Deserialize(&raw_data);
    CHECK(header == nullptr);
}

TEST(CapwapHeaderTestsGroup, ClearHeader_HLEN_vale) {
    uint8_t capwap_header_min[] = { 0x00, 0x17, 0xC2, 0xC8, 0xFF, 0xFF, 0xFF, 0xF8 };
    RawData raw_data{ capwap_header_min, capwap_header_min + sizeof(capwap_header_min) };
    auto header = ClearHeader::Deserialize(&raw_data);
    CHECK(header != nullptr);
    CHECK_EQUAL(2, header->HLEN());

    uint8_t capwap_header_max[] = { 0x00, 0xF8, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00 };
    raw_data = { capwap_header_max, capwap_header_max + sizeof(capwap_header_max) };
    header = ClearHeader::Deserialize(&raw_data);
    CHECK(header != nullptr);
    CHECK_EQUAL(31, header->HLEN());
}

TEST(CapwapHeaderTestsGroup, ClearHeader_RID) {
    uint8_t capwap_header_min[] = { 0x00, 0xF8, 0x43, 0xF8, 0xFF, 0xFF, 0xFF, 0xF8 };
    RawData raw_data{ capwap_header_min, capwap_header_min + sizeof(capwap_header_min) };
    auto header = ClearHeader::Deserialize(&raw_data);
    CHECK(header != nullptr);
    CHECK_EQUAL(1, header->RID());

    uint8_t capwap_header_max[] = { 0x00, 0x17, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00 };
    raw_data = { capwap_header_max, capwap_header_max + sizeof(capwap_header_max) };
    header = ClearHeader::Deserialize(&raw_data);
    CHECK(header != nullptr);
    CHECK_EQUAL(31, header->RID());

    uint8_t capwap_header_zero[] = { 0x00, 0x10, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00 };
    raw_data = { capwap_header_zero, capwap_header_zero + sizeof(capwap_header_zero) };
    header = ClearHeader::Deserialize(&raw_data);
    CHECK(header != nullptr);
    CHECK_EQUAL(0, header->RID());
}

TEST(CapwapHeaderTestsGroup, ClearHeader_WBID) {
    uint8_t capwap_header_min[] = { 0x00, 0xFF, 0xC3, 0xF8, 0xFF, 0xFF, 0xFF, 0xF8 };
    RawData raw_data{ capwap_header_min, capwap_header_min + sizeof(capwap_header_min) };
    auto header = ClearHeader::Deserialize(&raw_data);
    CHECK(header != nullptr);
    CHECK_EQUAL(WBIDType::IEEE_80211, header->WBID());

    uint8_t capwap_header_0[] = { 0x00, 0x00, 0x80, 0x20, 0x00, 0x00, 0x00, 0x00 };
    raw_data = { capwap_header_0, capwap_header_0 + sizeof(capwap_header_0) };
    header = ClearHeader::Deserialize(&raw_data);
    CHECK(header == nullptr);

    uint8_t capwap_header_2[] = { 0x00, 0x00, 0x84, 0x20, 0x00, 0x00, 0x00, 0x00 };
    raw_data = { capwap_header_2, capwap_header_2 + sizeof(capwap_header_2) };
    header = ClearHeader::Deserialize(&raw_data);
    CHECK(header == nullptr);

    uint8_t capwap_header_3[] = { 0x00, 0x00, 0x86, 0x20, 0x00, 0x00, 0x00, 0x00 };
    raw_data = { capwap_header_3, capwap_header_3 + sizeof(capwap_header_3) };
    header = ClearHeader::Deserialize(&raw_data);
    CHECK(header == nullptr);
}

TEST(CapwapHeaderTestsGroup, ClearHeader_T_flag) {
    uint8_t capwap_header_false[] = { 0x00, 0xFF, 0xC2, 0xF8, 0xFF, 0xFF, 0xFF, 0xF8 };
    RawData raw_data{ capwap_header_false, capwap_header_false + sizeof(capwap_header_false) };
    auto header = ClearHeader::Deserialize(&raw_data);
    CHECK(header != nullptr);
    CHECK_FALSE(header->T());
    CHECK_EQUAL(ClearHeader::PayloadFrameFormat::IEEE_8023_frame, header->T());

    uint8_t capwap_header_true[] = { 0x00, 0x80, 0x83, 0x00, 0x00, 0x00, 0x00, 0x00 };
    raw_data = { capwap_header_true, capwap_header_true + sizeof(capwap_header_true) };
    header = ClearHeader::Deserialize(&raw_data);
    CHECK(header != nullptr);
    CHECK_EQUAL(ClearHeader::PayloadFrameFormat::native_frame, header->T());
}

TEST(CapwapHeaderTestsGroup, ClearHeader_F_flag) {
    uint8_t capwap_header_false[] = { 0x00, 0xFF, 0xC3, 0x78, 0xFF, 0xFF, 0xFF, 0xF8 };
    RawData raw_data{ capwap_header_false, capwap_header_false + sizeof(capwap_header_false) };
    auto header = ClearHeader::Deserialize(&raw_data);
    CHECK(header != nullptr);
    CHECK_FALSE(header->F());

    uint8_t capwap_header_true[] = { 0x00, 0x80, 0x82, 0x80, 0x00, 0x00, 0x00, 0x00 };
    raw_data = { capwap_header_true, capwap_header_true + sizeof(capwap_header_true) };
    header = ClearHeader::Deserialize(&raw_data);
    CHECK(header != nullptr);
    CHECK_TRUE(header->F());
}

TEST(CapwapHeaderTestsGroup, ClearHeader_L_flag) {
    uint8_t capwap_header_false[] = { 0x00, 0xFF, 0xC3, 0xB8, 0xFF, 0xFF, 0xFF, 0xF8 };
    RawData raw_data{ capwap_header_false, capwap_header_false + sizeof(capwap_header_false) };
    auto header = ClearHeader::Deserialize(&raw_data);
    CHECK(header != nullptr);
    CHECK_FALSE(header->L());

    uint8_t capwap_header_true[] = { 0x00, 0x80, 0x82, 0x40, 0x00, 0x00, 0x00, 0x00 };
    raw_data = { capwap_header_true, capwap_header_true + sizeof(capwap_header_true) };
    header = ClearHeader::Deserialize(&raw_data);
    CHECK(header != nullptr);
    CHECK_TRUE(header->L());
}

TEST(CapwapHeaderTestsGroup, ClearHeader_W_flag) {
    uint8_t capwap_header_false[] = { 0x00, 0xFF, 0xC3, 0xD8, 0xFF, 0xFF, 0xFF, 0xF8 };
    RawData raw_data{ capwap_header_false, capwap_header_false + sizeof(capwap_header_false) };
    auto header = ClearHeader::Deserialize(&raw_data);
    CHECK(header != nullptr);
    CHECK_FALSE(header->W());

    uint8_t capwap_header_true[] = { 0x00, 0x80, 0x82, 0x20, 0x00, 0x00, 0x00, 0x00 };
    raw_data = { capwap_header_true, capwap_header_true + sizeof(capwap_header_true) };
    header = ClearHeader::Deserialize(&raw_data);
    CHECK(header != nullptr);
    CHECK_TRUE(header->W());
}

TEST(CapwapHeaderTestsGroup, ClearHeader_M_flag) {
    uint8_t capwap_header_false[] = { 0x00, 0xFF, 0xC3, 0xE8, 0xFF, 0xFF, 0xFF, 0xF8 };
    RawData raw_data{ capwap_header_false, capwap_header_false + sizeof(capwap_header_false) };
    auto header = ClearHeader::Deserialize(&raw_data);
    CHECK(header != nullptr);
    CHECK_FALSE(header->M());

    uint8_t capwap_header_true[] = { 0x00, 0x80, 0x82, 0x10, 0x00, 0x00, 0x00, 0x00 };
    raw_data = { capwap_header_true, capwap_header_true + sizeof(capwap_header_true) };
    header = ClearHeader::Deserialize(&raw_data);
    CHECK(header != nullptr);
    CHECK_TRUE(header->M());
}

TEST(CapwapHeaderTestsGroup, ClearHeader_K_flag) {
    uint8_t capwap_header_false[] = { 0x00, 0xFF, 0xC3, 0xF0, 0xFF, 0xFF, 0xFF, 0xF8 };
    RawData raw_data{ capwap_header_false, capwap_header_false + sizeof(capwap_header_false) };
    auto header = ClearHeader::Deserialize(&raw_data);
    CHECK(header != nullptr);
    CHECK_FALSE(header->K());

    uint8_t capwap_header_true[] = { 0x00, 0x80, 0x82, 0x08, 0x00, 0x00, 0x00, 0x00 };
    raw_data = { capwap_header_true, capwap_header_true + sizeof(capwap_header_true) };
    header = ClearHeader::Deserialize(&raw_data);
    CHECK(header != nullptr);
    CHECK_TRUE(header->K());
}

TEST(CapwapHeaderTestsGroup, ClearHeader_Fragment_ID) {
    uint8_t capwap_header_min[] = { 0x00, 0xFF, 0xC3, 0xF8, 0x00, 0x00, 0xFF, 0xF8 };
    RawData raw_data{ capwap_header_min, capwap_header_min + sizeof(capwap_header_min) };
    auto header = ClearHeader::Deserialize(&raw_data);
    CHECK(header != nullptr);
    CHECK_EQUAL(0, header->Fragment_ID());

    uint8_t capwap_header_max[] = { 0x00, 0x80, 0x82, 0x00, 0xFF, 0xFF, 0x00, 0x00 };
    raw_data = { capwap_header_max, capwap_header_max + sizeof(capwap_header_max) };
    header = ClearHeader::Deserialize(&raw_data);
    CHECK(header != nullptr);
    CHECK_EQUAL(65535, header->Fragment_ID());
}

TEST(CapwapHeaderTestsGroup, ClearHeader_Fragment_Offset) {
    uint8_t capwap_header_min[] = { 0x00, 0xFF, 0xC3, 0xF8, 0xFF, 0xFF, 0x00, 0x00 };
    RawData raw_data{ capwap_header_min, capwap_header_min + sizeof(capwap_header_min) };
    auto header = ClearHeader::Deserialize(&raw_data);
    CHECK(header != nullptr);
    CHECK_EQUAL(0, header->Fragment_Offset());

    uint8_t capwap_header_max[] = { 0x00, 0x80, 0x82, 0x00, 0x00, 0x00, 0xFF, 0xF8 };
    raw_data = { capwap_header_max, capwap_header_max + sizeof(capwap_header_max) };
    header = ClearHeader::Deserialize(&raw_data);
    CHECK(header != nullptr);
    CHECK_EQUAL(8191, header->Fragment_Offset());
}

TEST(CapwapHeaderTestsGroup, ClearHeader_serialize) {
    uint8_t buffer[256] = { 0xFF };
    ClearHeader header_0(2,
                         3,
                         ClearHeader::PayloadFrameFormat::IEEE_8023_frame,
                         false,
                         true,
                         false,
                         false,
                         true,
                         100,
                         42);

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    header_0.Serialize(&raw_data);
    CHECK_EQUAL(raw_data.current, buffer + sizeof(ClearHeader));
    const uint8_t buffer_0[] = { 0x00, 0x10, 0xC2, 0x48, 0x00, 0x64, 0x01, 0x50 };
    MEMCMP_EQUAL(buffer_0, buffer, sizeof(buffer_0));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto header = ClearHeader::Deserialize(&raw_data);
    CHECK(header != nullptr);
    CHECK_EQUAL(raw_data.current, buffer + sizeof(ClearHeader));
    CHECK_EQUAL(2, header->HLEN());
    CHECK_EQUAL(3, header->RID());
    CHECK_EQUAL(WBIDType::IEEE_80211, header->WBID());
    CHECK_EQUAL(ClearHeader::PayloadFrameFormat::IEEE_8023_frame, header->T());
    CHECK_FALSE(header->F());
    CHECK_TRUE(header->L());
    CHECK_FALSE(header->W());
    CHECK_FALSE(header->M());
    CHECK_TRUE(header->K());
    CHECK_EQUAL(100, header->Fragment_ID());
    CHECK_EQUAL(42, header->Fragment_Offset());
}

TEST(CapwapHeaderTestsGroup, RadioMACAddress_deserialize) {
    uint8_t radio_mac_field_eui48[] = { // Длина MAC-адреса
                                        0x06,
                                        // Сам MAC-адрес (6 байт)
                                        0x00,
                                        0x1A,
                                        0x2B,
                                        0x3C,
                                        0x4D,
                                        0x5E,
                                        // Дополнение (Padding) до 8 байт
                                        0x00
    };
    RawData raw_data{ radio_mac_field_eui48,
                      radio_mac_field_eui48 + sizeof(radio_mac_field_eui48) };
    auto mac_address = RadioMACAddress::Deserialize(&raw_data);

    CHECK(mac_address != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(6, mac_address->Length());
    CHECK_EQUAL(8, mac_address->RoundedSize());

    const uint8_t mac_6[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    MEMCMP_EQUAL(mac_address->MACAddress(), mac_6, mac_address->Length());

    uint8_t radio_mac_field_eui64[] = { // Длина MAC-адреса
                                        0x08,
                                        // Сам MAC-адрес (8 байт)
                                        0xAA,
                                        0xBB,
                                        0xCC,
                                        0xFF,
                                        0xFE,
                                        0xDD,
                                        0xEE,
                                        0xFF,
                                        // Дополнение (Padding) до 12 байт (3 байта)
                                        0x00,
                                        0x00,
                                        0x00
    };

    raw_data = { radio_mac_field_eui64, radio_mac_field_eui64 + sizeof(radio_mac_field_eui64) };
    mac_address = RadioMACAddress::Deserialize(&raw_data);

    CHECK(mac_address != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(8, mac_address->Length());
    CHECK_EQUAL(12, mac_address->RoundedSize());

    const uint8_t mac_8[] = { 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD, 0xEE, 0xFF };
    MEMCMP_EQUAL(mac_address->MACAddress(), mac_8, mac_address->Length());
}

TEST(CapwapHeaderTestsGroup, RadioMACAddress_serialize) {
    uint8_t buffer[32] = {};
    const uint8_t mac_6[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    RadioMACAddress mac_address_6{ 6, mac_6 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    mac_address_6.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 8, raw_data.current);
    MEMCMP_EQUAL(mac_address_6.MACAddress(), mac_6, mac_address_6.Length());

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto mac_address = RadioMACAddress::Deserialize(&raw_data);
    CHECK(mac_address != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.current);
    CHECK_EQUAL(mac_address_6.Length(), mac_address->Length());

    const uint8_t mac_8[] = { 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD, 0xEE, 0xFF };
    RadioMACAddress mac_address_8{ 8, mac_8 };
    raw_data = { buffer, buffer + sizeof(buffer) };

    mac_address_8.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 12, raw_data.current);
    MEMCMP_EQUAL(mac_address_8.MACAddress(), mac_8, mac_address_8.Length());

    raw_data = { buffer, buffer + sizeof(buffer) };
    mac_address = RadioMACAddress::Deserialize(&raw_data);
    CHECK(mac_address != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.current);
    CHECK_EQUAL(mac_address_8.Length(), mac_address->Length());
}

TEST(CapwapHeaderTestsGroup, IEEE80211FrameInfo_deserialize) {
    uint8_t frameinfo_80211[] = { // Внутренняя длина данных (4 байта)
                                  0x04,
                                  0x4B, // RSSI = 75
                                  0x19, // SNR = 25
                                  0x00,
                                  0x6C, // Data Rate = 108 (в Big Endian)
                                  // Дополнение (Padding) до 8 байт (3 байта)
                                  0x00,
                                  0x00,
                                  0x00
    };
    RawData raw_data{ frameinfo_80211, frameinfo_80211 + sizeof(frameinfo_80211) };
    auto frameinfo = IEEE80211FrameInfo::Deserialize(&raw_data);

    CHECK(frameinfo != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(75, frameinfo->RSSI());
    CHECK_EQUAL(25, frameinfo->SNR());
    CHECK_EQUAL(108, frameinfo->DataRate());

    uint8_t frameinfo_80211_sample[] = { 0x04, 0xBC, 0x17, 0x00, 0x00, 0x00, 0x00, 0x00 };
    raw_data = { frameinfo_80211_sample, frameinfo_80211_sample + sizeof(frameinfo_80211_sample) };
    frameinfo = IEEE80211FrameInfo::Deserialize(&raw_data);

    CHECK(frameinfo != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(188, frameinfo->RSSI());
    CHECK_EQUAL(23, frameinfo->SNR());
    CHECK_EQUAL(0, frameinfo->DataRate());
}

TEST(CapwapHeaderTestsGroup, IEEE80211FrameInfo_serialize) {
    uint8_t buffer[10] = {};
    IEEE80211FrameInfo frameinfo_0{ 12, 34, 567 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    frameinfo_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 8, raw_data.current);
    const uint8_t frameinfo_80211[] = { 0x04, 0x0C, 0x22, 0x02, 0x37, 0x00, 0x00, 0x00 };
    MEMCMP_EQUAL(buffer, frameinfo_80211, sizeof(frameinfo_80211));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto frameinfo = IEEE80211FrameInfo::Deserialize(&raw_data);
    CHECK(frameinfo != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.current);
    CHECK_EQUAL(12, frameinfo->RSSI());
    CHECK_EQUAL(34, frameinfo->SNR());
    CHECK_EQUAL(567, frameinfo->DataRate());
}

TEST(CapwapHeaderTestsGroup, CapwapHeader_serialize_with_M_and_W) {
    uint8_t buffer[256] = { 0xFF };
    const uint8_t reference[] = { 0x00,
                                  0x30,
                                  0xC2,
                                  0x30,
                                  0x00,
                                  0x64,
                                  0x01,
                                  0x50, // Длина MAC-адреса
                                  0x06,
                                  // Сам MAC-адрес (6 байт)
                                  0x00,
                                  0x1A,
                                  0x2B,
                                  0x3C,
                                  0x4D,
                                  0x5E,
                                  // Дополнение (Padding) до 8 байт
                                  0x00,
                                  // Внутренняя длина данных (4 байта)
                                  0x04,
                                  0x4B, // RSSI = 75
                                  0x19, // SNR = 25
                                  0x00,
                                  0x6C, // Data Rate = 108 (в Big Endian)
                                  // Дополнение (Padding) до 8 байт (3 байта)
                                  0x00,
                                  0x00,
                                  0x00 };

    const uint8_t mac_6[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    RadioMACAddress mac_address_6{ 6, mac_6 };
    IEEE80211FrameInfo frameinfo{ 75, 25, 108 };

    WritableCapwapHeader capwap_header(3,
                                       ClearHeader::PayloadFrameFormat::IEEE_8023_frame,
                                       false,
                                       false,
                                       false,
                                       100,
                                       42,
                                       &mac_address_6,
                                       &frameinfo);

    CHECK_EQUAL(sizeof(reference) / 4, capwap_header.HLEN());

    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    capwap_header.Serialize(&raw_data);

    size_t total_size = raw_data.current - buffer;
    CHECK_EQUAL(total_size, sizeof(reference));
    MEMCMP_EQUAL(reference, buffer, total_size);
}

TEST(CapwapHeaderTestsGroup, CapwapHeader_deserialize_with_M_and_W) {
    uint8_t data[] = { 0x00,
                       0x36,
                       0x83,
                       0xB0,
                       0x42,
                       0x19,
                       0x03,
                       0x20,
                       // Длина MAC-адреса
                       0x06,
                       // Сам MAC-адрес (6 байт)
                       0x00,
                       0x1A,
                       0x2B,
                       0x3C,
                       0x4D,
                       0x5E,
                       // Дополнение (Padding) до 8 байт
                       0x00,
                       // Внутренняя длина данных (4 байта)
                       0x04,
                       0x4B, // RSSI = 75
                       0x19, // SNR = 25
                       0x00,
                       0x6C, // Data Rate = 108 (в Big Endian)
                       // Дополнение (Padding) до 8 байт (3 байта)
                       0x00,
                       0x00,
                       0x00 };
    RawData raw_data{ data, data + sizeof(data) };

    ReadableCapwapHeader capwap_header;

    CHECK_TRUE(capwap_header.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(sizeof(data) / 4, capwap_header.header->HLEN());
    CHECK_EQUAL(26, capwap_header.header->RID());
    CHECK_EQUAL(WBIDType::IEEE_80211, capwap_header.header->WBID());
    CHECK_TRUE(capwap_header.header->T());
    CHECK_TRUE(capwap_header.header->F());
    CHECK_FALSE(capwap_header.header->L());
    CHECK_TRUE(capwap_header.header->W());
    CHECK_TRUE(capwap_header.header->M());
    CHECK_FALSE(capwap_header.header->K());
    CHECK_EQUAL(16921, capwap_header.header->Fragment_ID());
    CHECK_EQUAL(100, capwap_header.header->Fragment_Offset());

    CHECK(capwap_header.radio_mac_address != nullptr);
    const uint8_t mac_6[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    MEMCMP_EQUAL(capwap_header.radio_mac_address->MACAddress(),
                 mac_6,
                 capwap_header.radio_mac_address->Length());

    CHECK(capwap_header.wireless_specific_information != nullptr);
    CHECK_EQUAL(75, capwap_header.wireless_specific_information->RSSI());
    CHECK_EQUAL(25, capwap_header.wireless_specific_information->SNR());
    CHECK_EQUAL(108, capwap_header.wireless_specific_information->DataRate());
}

TEST(CapwapHeaderTestsGroup, CapwapHeader_deserialize_with_W) {
    uint8_t data[] = { 0x00,
                       0x26,
                       0x83,
                       0xA0,
                       0x42,
                       0x19,
                       0x03,
                       0x20,
                       // Внутренняя длина данных (4 байта)
                       0x04,
                       0x4B, // RSSI = 75
                       0x19, // SNR = 25
                       0x00,
                       0x6C, // Data Rate = 108 (в Big Endian)
                       // Дополнение (Padding) до 8 байт (3 байта)
                       0x00,
                       0x00,
                       0x00 };
    RawData raw_data{ data, data + sizeof(data) };

    ReadableCapwapHeader capwap_header;

    CHECK_TRUE(capwap_header.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(sizeof(data) / 4, capwap_header.header->HLEN());
    CHECK_TRUE(capwap_header.header->W());
    CHECK_FALSE(capwap_header.header->M());
    CHECK(capwap_header.radio_mac_address == nullptr);

    CHECK(capwap_header.wireless_specific_information != nullptr);
    CHECK_EQUAL(75, capwap_header.wireless_specific_information->RSSI());
    CHECK_EQUAL(25, capwap_header.wireless_specific_information->SNR());
    CHECK_EQUAL(108, capwap_header.wireless_specific_information->DataRate());
}

TEST(CapwapHeaderTestsGroup, CapwapHeader_deserialize_with_M) {
    uint8_t data[] = { 0x00,
                       0x26,
                       0x83,
                       0x90,
                       0x42,
                       0x19,
                       0x03,
                       0x20,
                       // Длина MAC-адреса
                       0x06,
                       // Сам MAC-адрес (6 байт)
                       0x00,
                       0x1A,
                       0x2B,
                       0x3C,
                       0x4D,
                       0x5E,
                       // Дополнение (Padding) до 8 байт
                       0x00 };

    RawData raw_data{ data, data + sizeof(data) };

    ReadableCapwapHeader capwap_header;

    CHECK_TRUE(capwap_header.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(sizeof(data) / 4, capwap_header.header->HLEN());
    CHECK_FALSE(capwap_header.header->W());
    CHECK_TRUE(capwap_header.header->M());

    CHECK(capwap_header.radio_mac_address != nullptr);
    const uint8_t mac_6[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    MEMCMP_EQUAL(capwap_header.radio_mac_address->MACAddress(),
                 mac_6,
                 capwap_header.radio_mac_address->Length());

    CHECK(capwap_header.wireless_specific_information == nullptr);
}

TEST(CapwapHeaderTestsGroup, ControlHeader_deserialize) {
    uint8_t data[] = {
        // Message Type = 1, Seq Num = 10, ME Length = 87
        0x00, 0x00, 0x00, 0x01, 0x0a, 0x00, 0x57, 0x00,
    };
    RawData raw_data{ data, data + sizeof(data) };
    auto control_header = ControlHeader::Deserialize(&raw_data);

    CHECK(control_header != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ControlHeader::DiscoveryRequest, control_header->GetType());
    CHECK_EQUAL(10, control_header->GetSeqNum());
    CHECK_EQUAL(87, control_header->GetMsgElementLength());
}

TEST(CapwapHeaderTestsGroup, ControlHeader_serialize) {
    uint8_t buffer[10] = {};
    ControlHeader control_header_0{ ControlHeader::DiscoveryRequest, 10, 87 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    control_header_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 8, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x00, 0x00, 0x01, 0x0a, 0x00, 0x57, 0x00 };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto control_header = ControlHeader::Deserialize(&raw_data);
    CHECK(control_header != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.current);
    CHECK_EQUAL(ControlHeader::DiscoveryRequest, control_header->GetType());
    CHECK_EQUAL(10, control_header->GetSeqNum());
    CHECK_EQUAL(87, control_header->GetMsgElementLength());
}