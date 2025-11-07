CC ?= gcc
TARGET ?= stegobmp
TEST_TARGET ?= test_runner

SRC_DIR := src
TEST_DIR := $(SRC_DIR)/test
INC_DIR := $(SRC_DIR)/include
BUILD_DIR := build

# normal source files (exclude tests)
SRC := $(shell find $(SRC_DIR) -type f -name '*.c' ! -path "$(TEST_DIR)/*")
# exclude main.c so tests don’t link its main()
SRC_NO_MAIN := $(filter-out $(SRC_DIR)/main.c,$(SRC))

OBJ := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))
OBJ_NO_MAIN := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_NO_MAIN))
DEP := $(OBJ:.o=.d)

TEST_SRC := $(shell find $(TEST_DIR) -type f -name '*.c')
TEST_MAIN := $(TEST_DIR)/test.c
TEST_SRC_NO_MAIN := $(filter-out $(TEST_MAIN),$(TEST_SRC))
TEST_OBJ := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(TEST_SRC_NO_MAIN))
TEST_MAIN_OBJ := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(TEST_MAIN))
TEST_DEP := $(TEST_OBJ:.o=.d) $(TEST_MAIN_OBJ:.o=.d)

CPPFLAGS ?= -I$(INC_DIR)
CFLAGS ?= -Wall -Wextra -Wpedantic -std=c11
LDFLAGS ?= -lssl -lcrypto
LDLIBS ?=

BUILD ?= release
ifeq ($(BUILD),debug)
CFLAGS += -O0 -g -DDEBUG
else
CFLAGS += -O2 -DNDEBUG
endif

.PHONY: all debug release clean run test

all: $(TARGET)

debug:
	$(MAKE) BUILD=debug

release:
	$(MAKE) BUILD=release

$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) $(LDLIBS) -o $@

$(TEST_TARGET): $(OBJ_NO_MAIN) $(TEST_OBJ) $(TEST_MAIN_OBJ)
	@mkdir -p $(dir $@)
	$(CC) $(OBJ_NO_MAIN) $(TEST_OBJ) $(TEST_MAIN_OBJ) $(LDFLAGS) $(LDLIBS) -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TEST_TARGET)
	rm -rf public/*_stegobmp.*

-include $(DEP) $(TEST_DEP)
