PROGRAM := costa

PROFILE ?= dist
BUILD_DIR ?= .build
SRC_DIR ?= src
TEST_SRC_DIR ?= test
INCLUDE_DIR ?= include

CC ?= gcc
CFLAGS ?= -Wall -Wextra
LDFLAGS ?=

include make/utils.mk

PROFILES := debug release dist
ifeq ($(filter $(PROFILE),$(PROFILES)),)
$(error Invalid PROFILE: '$(PROFILE)'! Must be one of: $(call delimlist,$(PROFILES:%='%'),$(comma),or))
endif

ifeq ($(PROFILE),debug)
	CFLAGS += -g -O0
else ifeq ($(PROFILE),release)
	CFLAGS += -g -O2
else ifeq ($(PROFILE),dist)
	CFLAGS += -O3
endif

TARGET_DIR := $(BUILD_DIR)/$(PROFILE)
OBJS_DIR := $(TARGET_DIR)/objs
TEST_DIR := $(TARGET_DIR)/test

C_SRCS := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/**/*.c)
OBJS := $(C_SRCS:$(SRC_DIR)/%.c=$(OBJS_DIR)/%.o)

TEST_C_SRCS := $(wildcard $(TEST_SRC_DIR)/*.c)
TEST_OBJS := $(TEST_C_SRCS:$(TEST_SRC_DIR)/%.c=$(TEST_DIR)/%.o) $(filter-out $(OBJS_DIR)/$(PROGRAM).o,$(OBJS))

TARGET := $(TARGET_DIR)/$(PROGRAM)
TEST_TARGET := $(TARGET_DIR)/test/$(PROGRAM)

.PHONY: all clean debug release dist

all: $(TARGET) $(TEST_TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(TEST_TARGET): $(TEST_OBJS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJS_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -I$(SRC_DIR) -c $< -o $@

$(TEST_DIR)/%.o: $(TEST_SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -I$(SRC_DIR) -c $< -o $@

clean:
	@rm -rf $(BUILD_DIR)

debug:
	$(MAKE) PROFILE=debug

release:
	$(MAKE) PROFILE=release

dist:
	$(MAKE) PROFILE=dist
