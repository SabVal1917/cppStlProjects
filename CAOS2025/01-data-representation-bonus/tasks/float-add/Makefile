default: test-stage-3

# You can change it to -O1, -O2 or even -O3 to
# speed up test_float64_parse_fuzz (and similar long tests)
# Make sure to run make clean afterwards.
OPTIMIZE_LEVEL = -O0

SOL_DIR = solution
DIR = $(shell pwd)
CXX = g++
CC = gcc
CXX_FLAGS = $(CFLAGS) -fsanitize=address -std=c++20 -I$(DIR)/$(SOL_DIR) -g $(OPTIMIZE_LEVEL)
CC_FLAGS = $(CFLAGS) -fsanitize=address -I$(DIR)/$(SOL_DIR) -g $(OPTIMIZE_LEVEL)
BUILD_DIR = ./build
BIN = tester

CPP_SRC=$(wildcard *.cpp) $(wildcard */*.cpp)
C_SRC=$(wildcard *.c) $(wildcard */*.c)

OBJ = $(CPP_SRC:%.cpp=$(BUILD_DIR)/%.o) $(C_SRC:%.c=$(BUILD_DIR)/%.o)
DEPFILES = $(OBJ:%.o=%.d)

depend: .depend

%.d : %.cpp
	rm -f "$@"
	$(CXX) $(CC_FLAGS) -MM $^ -MF "$@"

%.d : %.c
	rm -f "$@"
	$(CC) $(CC_FLAGS) -MM $^ -MF "$@"

.depend : $(DEPFILES)
	echo $(DEPFILES)
	cat $^ > "$@"

$(BUILD_DIR)/%.o : %.cpp
	mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) -MMD -c $< -o $@

$(BUILD_DIR)/%.o : %.c
	mkdir -p $(@D)
	$(CC) $(CC_FLAGS) -MMD -c $< -o $@

$(BUILD_DIR)/$(BIN) : $(OBJ)
	mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) $^ -o $@

test-all: $(BUILD_DIR)/$(BIN)
	$(BUILD_DIR)/$(BIN)

test-stage-1: $(BUILD_DIR)/$(BIN)
	$(BUILD_DIR)/$(BIN) system_audit construct mul # prev_next add div string

test-stage-2: $(BUILD_DIR)/$(BIN)
	$(BUILD_DIR)/$(BIN) system_audit construct mul prev_next # add div string

test-stage-3: $(BUILD_DIR)/$(BIN)
	$(BUILD_DIR)/$(BIN) system_audit construct mul prev_next add # div string

test-stage-4: $(BUILD_DIR)/$(BIN)
	$(BUILD_DIR)/$(BIN) system_audit construct mul prev_next add div # string

test-stage-5: $(BUILD_DIR)/$(BIN)
	$(BUILD_DIR)/$(BIN) system_audit construct mul prev_next add div string

clean:
	rm -rf .depend build

.PHONY: run

sinclude .depend
