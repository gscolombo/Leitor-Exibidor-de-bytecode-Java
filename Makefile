# Compiler and flags
CC = gcc
DEBUG_MODE = false

# Find all subdirectories in include (including nested ones)
INCLUDE_DIRS = $(shell find include -type d)
CFLAGS = $(addprefix -I,$(INCLUDE_DIRS)) -std=c99 -Wall -Wextra
DEBUG_FLAGS = -fsanitize=address -g
LDFLAGS = -static-libasan

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
	@mkdir -p $@

# Link object files into the executable
$(TARGET): $(OBJS)
ifeq ($(DEBUG_MODE), true)
	@$(CC) $(OBJS) -o $@ $(DEBUG_FLAGS) $(LDFLAGS)
else
	@$(CC) $(OBJS) -o $@ -lm
endif

# Compile each source file to an object file
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
ifeq ($(DEBUG_MODE), true)
	@$(CC) $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@
else
	@$(CC) $(CFLAGS) -c $< -o $@
endif

# Clean build artifacts
clean:
	@rm -rf $(BUILD_DIR) $(TARGET)

# Rebuild from scratch
rebuild: clean all

# Debug target to see what's being included
FILE= # Path to a .class file
debug:
	@echo "Include directories:"
	@echo "  "$(INCLUDE_DIRS)
	@echo "Source files:"
	@echo "  "$(SRCS)
	@echo "Object files:"
	@echo "  "$(OBJS)"\n"
	@make -e "DEBUG_MODE=true" -e "TARGET=cjavap_debug"
	@echo "\n------------------------------------------------\n"
	@cd .; ./cjavap_debug $(FILE) && rm cjavap_debug

.PHONY: all clean rebuild debug