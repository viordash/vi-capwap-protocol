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
