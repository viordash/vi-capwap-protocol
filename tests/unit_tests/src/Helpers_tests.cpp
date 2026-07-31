#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "Helpers.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(HelpersTestsGroup){ //
                               TEST_SETUP(){
                                   //

                               }

                               TEST_TEARDOWN(){}
};

TEST(HelpersTestsGroup, RoundTo4) {
    CHECK_EQUAL(4, RoundTo4(1));
    CHECK_EQUAL(4, RoundTo4(2));
    CHECK_EQUAL(4, RoundTo4(3));
    CHECK_EQUAL(4, RoundTo4(4));
    CHECK_EQUAL(8, RoundTo4(5));
    CHECK_EQUAL(8, RoundTo4(6));
    CHECK_EQUAL(8, RoundTo4(7));
    CHECK_EQUAL(8, RoundTo4(8));
    CHECK_EQUAL(12, RoundTo4(9));
}

TEST(HelpersTestsGroup, ToNetworkOrder_matches_the_detected_byte_order) {
#if VI_CAPWAP_BIG_ENDIAN
    CHECK_EQUAL(0x0001, ToNetworkOrder16(1));
    CHECK_EQUAL(0x1234, ToNetworkOrder16(0x1234));
    CHECK_EQUAL(0x00000001, ToNetworkOrder32(1));
    CHECK_EQUAL(0x12345678, ToNetworkOrder32(0x12345678));
#else
    CHECK_EQUAL(0x0100, ToNetworkOrder16(1));
    CHECK_EQUAL(0x3412, ToNetworkOrder16(0x1234));
    CHECK_EQUAL(0x01000000, ToNetworkOrder32(1));
    CHECK_EQUAL(0x78563412, ToNetworkOrder32(0x12345678));
#endif
}

TEST(HelpersTestsGroup, ToNetworkOrder_places_bytes_in_wire_order) {
    const uint16_t value16 = ToNetworkOrder16(0x1234);
    const uint8_t *bytes16 = (const uint8_t *)&value16;
    CHECK_EQUAL(0x12, bytes16[0]);
    CHECK_EQUAL(0x34, bytes16[1]);

    const uint32_t value32 = ToNetworkOrder32(0x12345678);
    const uint8_t *bytes32 = (const uint8_t *)&value32;
    CHECK_EQUAL(0x12, bytes32[0]);
    CHECK_EQUAL(0x34, bytes32[1]);
    CHECK_EQUAL(0x56, bytes32[2]);
    CHECK_EQUAL(0x78, bytes32[3]);
}

TEST(HelpersTestsGroup, ToHostOrder_reverts_ToNetworkOrder) {
    CHECK_EQUAL(0x1234, ToHostOrder16(ToNetworkOrder16(0x1234)));
    CHECK_EQUAL(0x12345678, ToHostOrder32(ToNetworkOrder32(0x12345678)));
}
