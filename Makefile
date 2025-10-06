CC ?= gcc
TARGET ?= stego

SRC_DIR := src
INC_DIR := $(SRC_DIR)/include
BUILD_DIR := build

SRC := $(shell find $(SRC_DIR) -type f -name '*.c')
OBJ := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))
DEP := $(OBJ:.o=.d)

CPPFLAGS ?= -I$(INC_DIR)
CFLAGS ?= -Wall -Wextra -Wpedantic -std=c11
LDFLAGS ?=
LDLIBS ?=

BUILD ?= release
ifeq ($(BUILD),debug)
CFLAGS += -O0 -g -DDEBUG
else
CFLAGS += -O2 -DNDEBUG
endif

.PHONY: all debug release clean run

all: $(TARGET)

debug:
	$(MAKE) BUILD=debug

release:
	$(MAKE) BUILD=release

$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) $(LDLIBS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

-include $(DEP)


