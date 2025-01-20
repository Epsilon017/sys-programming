# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -g -Wnounused-parameter

# Target executable name
TARGET = stringfun.out

# Default target
all: $(TARGET)

# Compile source to executable
$(TARGET): stringfun.c
	$(CC) $(CFLAGS) -o $(TARGET) $^

# Clean up build files
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: all clean