# Compiler and flags
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra

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
all: $(TARGET)

# Main executable
$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o $@

# Clean
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: all clean
