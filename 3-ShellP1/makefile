# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -g

# Target executable names
TARGET_DSH = dsh.out
TARGET_ART = art_to_array.out

# Find all source and header files
SRCS = $(wildcard *.c)
HDRS = $(wildcard *.h)

# Exclude art_to_array.c from dsh.out build
DSH_SRCS := $(filter-out art_to_array.c, $(SRCS))
ART_SRCS := art_to_array.c

# Default target: build both
all: $(TARGET_DSH) $(TARGET_ART)

# Compile dsh.out
$(TARGET_DSH): $(DSH_SRCS) $(HDRS)
	$(CC) $(CFLAGS) -o $(TARGET_DSH) $(DSH_SRCS)

# Compile art_to_array.out
$(TARGET_ART): $(ART_SRCS)
	$(CC) $(CFLAGS) -o $(TARGET_ART) $(ART_SRCS)

# Clean up build files
clean:
	rm -f $(TARGET_DSH) $(TARGET_ART)

test:
	./test.sh

# Phony targets
.PHONY: all clean test