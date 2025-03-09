PROGRAM := costa

PROFILE ?= dist
BUILD_DIR ?= .build

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
TARGET := $(TARGET_DIR)/$(PROGRAM)

SRCS := $(wildcard **/*.c)
OBJS := $(SRCS:%.c=$(OBJS_DIR)/%.o)

.PHONY: all clean debug release dist

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJS_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(BUILD_DIR)

debug:
	$(MAKE) PROFILE=debug

release:
	$(MAKE) PROFILE=release

dist:
	$(MAKE) PROFILE=dist
