# Compiler and flags
CC = gcc

# Find all subdirectories in include (including nested ones)
INCLUDE_DIRS = include $(shell find include -type d)
CFLAGS = $(addprefix -I,$(INCLUDE_DIRS)) -Wall -Wextra # add -g for debugging

# Directories
SRC_DIR = src
BUILD_DIR = build

# Recursively find all source files
SRCS = $(shell find $(SRC_DIR) -name '*.c')
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Target executable
TARGET = cjavap

# Default target
all: $(BUILD_DIR) $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $@

# Link object files into the executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

# Compile each source file to an object file
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Rebuild from scratch
rebuild: clean all

# Debug target to see what's being included
debug:
	@echo "Include directories:"
	@echo $(INCLUDE_DIRS)
	@echo "Source files:"
	@echo $(SRCS)
	@echo "Object files:"
	@echo $(OBJS)

.PHONY: all clean rebuild debug