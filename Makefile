# Compiler and flags
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra

# Directories
BIN_DIR = bin

# Source and header files
SOURCES = \
    main.c \
    scanner.c \
    parser.c \
    visualization.c \
    symtable.c \
    semantic_analysis.c \
    frame_list.c \
    codegeneration.c \
    builtinfun.c

HEADERS = \
    scanner.h \
    parser.h \
    visualization.h \
    symtable.h \
    semantic_analysis.h \
    frame_list.h \
    codegeneration.h \
    builtinfun.h

# Targets
TARGET = ifj_projekt

# Default target
all: $(BIN_DIR)/$(TARGET)

# Main executable
$(BIN_DIR)/$(TARGET): $(SOURCES) $(HEADERS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(SOURCES) -o $@

# Clean
clean:
	rm -rf $(BIN_DIR)

# Phony targets
.PHONY: all clean
