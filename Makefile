# Define the compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99

# Define the target executable
TARGET = main

# Define the source files and object files
SRCS = main.c scanner.c parser.c visualization.c
OBJS = main.o scanner.o parser.o visualization.o

# The default target, which will compile the program (same as 'compile')
all: compile

# Custom compile target (compiles the object files and creates the executable)
compile: $(TARGET)

# Rule to link the object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# Rule to compile main.c into an object file
main.o: main.c scanner.h parser.h
	$(CC) $(CFLAGS) -c main.c -o main.o

# Rule to compile scanner.c into an object file
scanner.o: scanner.c scanner.h parser.h
	$(CC) $(CFLAGS) -c scanner.c -o scanner.o

parser.o: parser.c parser.h  # Add rule for parser.c
	$(CC) $(CFLAGS) -c parser.c -o parser.o
# Clean up the compiled files (object files and executable)
clean:
	rm -f $(OBJS) $(TARGET)