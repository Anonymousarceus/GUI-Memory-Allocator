# Dynamic Memory Allocator Makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -O2
INCLUDE_DIR = include
SRC_DIR = src
BUILD_DIR = build

# Source files
SOURCES = $(SRC_DIR)/allocator.c $(SRC_DIR)/test.c
OBJECTS = $(BUILD_DIR)/allocator.o $(BUILD_DIR)/test.o
TARGET = $(BUILD_DIR)/memory_allocator_test

# Default target
all: $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile allocator.c
$(BUILD_DIR)/allocator.o: $(SRC_DIR)/allocator.c $(INCLUDE_DIR)/allocator.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Compile test.c
$(BUILD_DIR)/test.o: $(SRC_DIR)/test.c $(INCLUDE_DIR)/allocator.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Link executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

# Run tests
test: $(TARGET)
	./$(TARGET)

# Run with debug info
debug: CFLAGS += -DDEBUG
debug: $(TARGET)
	./$(TARGET)

# Run with memory sanitizer (if available)
sanitize: CFLAGS += -fsanitize=address -fsanitize=undefined
sanitize: $(TARGET)
	./$(TARGET)

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

# Create demo executable with sample usage
demo: $(BUILD_DIR)/allocator.o | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $(SRC_DIR)/demo.c -o $(BUILD_DIR)/demo.o
	$(CC) $(BUILD_DIR)/allocator.o $(BUILD_DIR)/demo.o -o $(BUILD_DIR)/demo
	./$(BUILD_DIR)/demo

# Install (copy to system directory - requires sudo)
install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/

# Show help
help:
	@echo "Available targets:"
	@echo "  all      - Build the test executable (default)"
	@echo "  test     - Build and run tests"
	@echo "  debug    - Build with debug symbols and run"
	@echo "  sanitize - Build with sanitizers and run"
	@echo "  demo     - Build and run demo program"
	@echo "  clean    - Remove build files"
	@echo "  install  - Install to system (requires sudo)"
	@echo "  help     - Show this help message"

.PHONY: all test debug sanitize demo clean install help