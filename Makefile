# Compiler and flags
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra

# Directories
BIN_DIR = bin
SRC_DIR = .
TEST_DIR = tests
INCLUDE_DIR = .

# Source and header files
SOURCES = main.c scanner.c parser.c visualization.c symtable.c semantic_analysis.c frame_list.c
HEADERS = scanner.h parser.h visualization.h symtable.h semantic_analysis.h frame_list.h

# Test files
TEST_SOURCES = $(TEST_DIR)/test1.c
SYMTABLE_TEST = $(TEST_DIR)/symtable_test.c

# Output binaries
MAIN_BIN = $(BIN_DIR)/ifj_projekt
TEST_BIN = $(BIN_DIR)/tests/test_ifj_projekt
SYMTABLE_TEST_BIN = $(BIN_DIR)/symtable_test_ifj_projekt

# Default target
all: $(MAIN_BIN) $(TEST_BIN) $(SYMTABLE_TEST_BIN)

# Create output directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)
	mkdir -p $(BIN_DIR)/tests

# Build main executable
$(MAIN_BIN): $(SOURCES) $(HEADERS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(SOURCES)

# Build test executable (excluding main.c)
$(TEST_BIN): $(TEST_SOURCES) scanner.c parser.c visualization.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(TEST_SOURCES) scanner.c parser.c visualization.c

# Build symtable test executable
$(SYMTABLE_TEST_BIN): $(SYMTABLE_TEST) symtable.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(SYMTABLE_TEST) symtable.c

# Clean build artifacts
clean:
	rm -rf $(BIN_DIR)

.PHONY: all clean
