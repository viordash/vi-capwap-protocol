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


.PHONY: all certs clean

# check and format c code
export CF_SRC				:= src
export CF_EXCLUDE_DIRS 	:= 
include $(ROOT_DIR)/check_format.mk 