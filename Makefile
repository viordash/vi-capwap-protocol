SHELL := /bin/bash

export ROOT_DIR := $(shell git rev-parse --show-toplevel)

all: build_tests build_benchmarks

clean: clean_tests clean_benchmarks

build_tests:
	$(MAKE) -C tests/unit_tests

clean_tests:
	$(MAKE) -C tests/unit_tests clean

build_benchmarks:
	$(MAKE) -C tests/benchmarks

clean_benchmarks:
	$(MAKE) -C tests/benchmarks clean


# Unit tests cross built for a big-endian target. The suite compares serialized
# output against literal wire bytes, so running it there verifies the byte order
# handling for real instead of only compiling it.
# Requires: apt install g++-mips-linux-gnu qemu-user-static
BE_CXX		?= mips-linux-gnu-g++
BE_SYSROOT	?= /usr/mips-linux-gnu
BE_QEMU		?= qemu-mips-static
BE_TESTS	:= $(ROOT_DIR)/output/tests/unit_tests-be/vi-capwap-protocol-tests

build_tests_be:
	$(MAKE) -C tests/unit_tests CC=$(BE_CXX) TARGET_SUFFIX=-be LOG_BACKEND=none

run_tests_be: build_tests_be
	$(BE_QEMU) -L $(BE_SYSROOT) $(BE_TESTS)

clean_tests_be:
	$(MAKE) -C tests/unit_tests CC=$(BE_CXX) TARGET_SUFFIX=-be clean


.PHONY: all certs clean build_tests_be run_tests_be clean_tests_be

# check and format c code
export CF_SRC				:= src
export CF_EXCLUDE_DIRS 	:= 
include $(ROOT_DIR)/check_format.mk 