#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/CapwapTransportProtocol.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(CapwapTransportProtocolTestsGroup){ //
                                               TEST_SETUP(){}

                                               TEST_TEARDOWN(){}
};

TEST(CapwapTransportProtocolTestsGroup, CapwapTransportProtocol_deserialize) {
    uint8_t data[] = {
        0x00, 0x33, 0x00, 0x01, 0x01,
    };
    RawData raw_data{ data, data + sizeof(data) };
    ReadableCapwapTransportProtocol read_data;
    CHECK_FALSE(read_data.IsPresent());
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::CAPWAPTransportProtocol, read_data.GetElementType());
    CHECK_EQUAL(CapwapTransportProtocol::Type::UDPLite, read_data.Get()->type);
    CHECK_TRUE(read_data.IsPresent());
}

TEST(CapwapTransportProtocolTestsGroup, CapwapTransportProtocol_serialize) {
    uint8_t buffer[256] = {};
    WritableCapwapTransportProtocol element_0{ CapwapTransportProtocol::Type::UDP };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 5, raw_data.current);
    const uint8_t reference[] = {
        0x00, 0x33, 0x00, 0x01, 0x02,
    };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    ReadableCapwapTransportProtocol read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(&buffer[0] + 5, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::CAPWAPTransportProtocol, read_data.GetElementType());
    CHECK_EQUAL(CapwapTransportProtocol::Type::UDP, read_data.Get()->type);
}
