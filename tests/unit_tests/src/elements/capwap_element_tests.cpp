#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ClearHeader.h"
#include "ControlHeader.h"
#include "elements/DiscoveryType.h"
#include "elements/ElementHeader.h"
#include "elements/MTUDiscoveryPadding.h"
#include "elements/WTPFrameTunnelMode.h"
#include "elements/WTPMACType.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(CapwapElementTestsGroup){ //
                                     TEST_SETUP(){}

                                     TEST_TEARDOWN(){}
};

TEST(CapwapElementTestsGroup, DiscoveryType_deserialize) {
    uint8_t data[] = {
        // 1. Discovery Type (Тип: 20, Длина: 1) - Static Configuration
        0x00, 0x14, 0x00, 0x01, 0x01,
    };
    RawData raw_data{ data, data + sizeof(data) };
    auto element = DiscoveryType::Deserialize(&raw_data);

    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::DiscoveryType, element->GetElementType());
    CHECK_EQUAL(DiscoveryType::Type::StaticConfiguration, element->type);
}

TEST(CapwapElementTestsGroup, DiscoveryType_serialize) {
    uint8_t buffer[256] = {};
    DiscoveryType element_0{ DiscoveryType::Type::DNS };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 5, raw_data.current);
    const uint8_t reference[] = {
        0x00, 0x14, 0x00, 0x01, 0x03,
    };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = DiscoveryType::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(&buffer[0] + 5, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::DiscoveryType, element->GetElementType());
    CHECK_EQUAL(DiscoveryType::Type::DNS, element->type);
}

TEST(CapwapElementTestsGroup, WTPFrameTunnelMode_deserialize) {
    uint8_t data[] = {
        // 2. WTP Frame Tunnel Mode (Тип: 41, Длина: 1) - Local Bridging + native
        0x00, 0x29, 0x00, 0x01, 0x0A,
    };
    RawData raw_data{ data, data + sizeof(data) };
    auto element = WTPFrameTunnelMode::Deserialize(&raw_data);

    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::WTPFrameTunnelMode, element->GetElementType());
    CHECK_TRUE(element->L);
    CHECK_FALSE(element->E);
    CHECK_TRUE(element->N);
}

TEST(CapwapElementTestsGroup, WTPFrameTunnelMode_serialize) {
    uint8_t buffer[256] = {};
    WTPFrameTunnelMode element_0{ true, true, false };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 5, raw_data.current);
    const uint8_t reference[] = {
        0x00, 0x29, 0x00, 0x01, 0x06,
    };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = WTPFrameTunnelMode::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(&buffer[0] + 5, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::WTPFrameTunnelMode, element->GetElementType());
    CHECK_TRUE(element->L);
    CHECK_TRUE(element->E);
    CHECK_FALSE(element->N);
}

TEST(CapwapElementTestsGroup, WTPMACType_deserialize) {
    uint8_t data[] = {
        // 3. WTP MAC Type (Тип: 44, Длина: 1) - Local MAC
        0x00, 0x2c, 0x00, 0x01, 0x00,
    };
    RawData raw_data{ data, data + sizeof(data) };
    auto element = WTPMACType::Deserialize(&raw_data);

    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::WTPMACType, element->GetElementType());
    CHECK_EQUAL(WTPMACType::Type::Local_MAC, element->type);
}

TEST(CapwapElementTestsGroup, WTPMACType_serialize) {
    uint8_t buffer[256] = {};
    WTPMACType element_0{ WTPMACType::Type::Split_MAC };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 5, raw_data.current);
    const uint8_t reference[] = {
        0x00, 0x2c, 0x00, 0x01, 0x01,
    };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = WTPMACType::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(&buffer[0] + 5, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::WTPMACType, element->GetElementType());
    CHECK_EQUAL(WTPMACType::Type::Split_MAC, element->type);
}

TEST(CapwapElementTestsGroup, MTUDiscoveryPadding_deserialize) {
    uint8_t data[] = { // 7. MTU Discovery Padding (Тип: 52, Длина: 93)
                       0x00,
                       0x34, // Type = 52
                       0x00,
                       0x5D, // Length = 93
                       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };
    RawData raw_data{ data, data + sizeof(data) };
    auto element = MTUDiscoveryPadding::Deserialize(&raw_data);

    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::MTUDiscoveryPadding, element->GetElementType());
    CHECK_EQUAL(93, element->GetLength());
}

TEST(CapwapElementTestsGroup, MTUDiscoveryPadding_serialize) {
    uint8_t buffer[2048] = {};
    MTUDiscoveryPadding element_0{ 42 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 46, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x34, 0x00, 0x2A, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = MTUDiscoveryPadding::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(&buffer[0] + 46, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::MTUDiscoveryPadding, element->GetElementType());
    CHECK_EQUAL(42, element->GetLength());
}
