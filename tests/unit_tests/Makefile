PROJECT_NAME := vi-capwap-protocol-tests

SHELL := /bin/bash

ROOT_DIR ?= $(shell git rev-parse --show-toplevel)

# Cross build: override CC and set TARGET_SUFFIX to keep artifacts of different
# targets apart. Cross targets have no system spdlog, so logging is compiled out:
#   make build_tests CC=mips-linux-gnu-g++ TARGET_SUFFIX=-mips LOG_BACKEND=none
TARGET_SUFFIX	?=
LOG_BACKEND		?= spdlog

OUTPUT	:= $(ROOT_DIR)/output/tests/unit_tests$(TARGET_SUFFIX)
BUILD	:= $(ROOT_DIR)/build/tests/unit_tests$(TARGET_SUFFIX)
SRC		:= $(ROOT_DIR)/tests/unit_tests/src

LIB_SRC				:= $(ROOT_DIR)/src
UTILS_SRC			:= $(ROOT_DIR)/utils

CCACHE 			?= ccache
# CC has a built-in default, so ?= would not take effect here
ifeq ($(origin CC), default)
CC 				:= g++
endif
LIB_CXX			:= $(CC)

CFLAGS := -std=c++17 -Wall -g -O0 -Wno-unused-parameter
LDFLAGS := -lCppUTest$(TARGET_SUFFIX)
ifeq ($(LOG_BACKEND),none)
CFLAGS += -DLOG_LEVEL=LOG_LEVEL_NONE
else
CFLAGS += -DLOG_LEVEL=LOG_LEVEL_INFO
LDFLAGS += -lspdlog -lfmt
endif

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