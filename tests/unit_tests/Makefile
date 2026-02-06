PROJECT_NAME := vi-capwap-protocol-tests

SHELL := /bin/bash

ROOT_DIR ?= $(shell git rev-parse --show-toplevel)

OUTPUT	:= $(ROOT_DIR)/output/tests/unit_tests
BUILD	:= $(ROOT_DIR)/build/tests/unit_tests
SRC		:= $(ROOT_DIR)/tests/unit_tests/src

LIB_SRC				:= $(ROOT_DIR)/src
UTILS_SRC			:= $(ROOT_DIR)/utils

CCACHE 			?= ccache
CC 				:= g++

CFLAGS := -std=c++17 -Wall -g -O0 -Wno-unused-parameter
LDFLAGS := -lssl -lcrypto -luuid -lCppUTest
CFLAGS += -DLOG_LEVEL=LOG_LEVEL_INFO

MAIN						:= $(PROJECT_NAME)
SOURCEDIRS					:= $(shell find -L $(SRC) -type d)

FIXPATH = $1
RM 	:= rm -rf
MD	:= mkdir -p

INCLUDES	:= $(patsubst %,-I%, $(SOURCEDIRS:%/=%) $(BUILD:%/=%) $(LIB_SRC:%/=%) $(UTILS_SRC:%/=%))
INCLUDES += -I$(ROOT_DIR)
INCLUDES += -I$(ROOT_DIR)/tests/lib/CppUTest/include
INCLUDES += -I$(ROOT_DIR)/tests/lib/nanobench/src/include
LIBS		:= -L$(call FIXPATH,$(ROOT_DIR)/tests/lib)

SOURCES_EXCLUDE 		:= 
SOURCES					:= $(filter-out $(addprefix %/,$(SOURCES_EXCLUDE)), \
                         $(wildcard $(patsubst %,%/*.cpp,$(SOURCEDIRS))))

OBJECTS					:= $(SOURCES:$(SRC)/%.cpp=$(BUILD)/%.o)
DEPS					:= $(OBJECTS:.o=.d)

OUTPUTMAIN	:= $(call FIXPATH,$(OUTPUT)/$(MAIN))
 
all: $(OUTPUT) $(MAIN)

include $(ROOT_DIR)/tests/lib/cpputest.mk

$(OUTPUT): libCppUTest.a
	@$(MD) $(OUTPUT)

$(MAIN): libCppUTest.a $(OBJECTS) $(LIB_OBJECTS)
	@$(CCACHE) $(CC) $(CFLAGS) $(INCLUDES) -o $(OUTPUTMAIN) $(OBJECTS) $(LIB_OBJECTS) $(LDFLAGS) $(LIBS)
	@echo $(OUTPUTMAIN) complete!

$(OBJECTS):
	@$(MD) $(dir $@)
	@$(CCACHE) $(CC) $(CFLAGS) $(INCLUDES) -c -MMD $(@:$(BUILD)/%.o=$(SRC)/%.cpp)  -o $@

.PHONY: clean
clean: cleanlib
	@$(RM) $(OUTPUTMAIN)
	@$(RM) $(BUILD)
	@$(RM) $(call FIXPATH,$(OBJECTS))
	@$(RM) $(call FIXPATH,$(DEPS))
	@echo Cleanup complete!

# include all .d files
-include $(DEPS)