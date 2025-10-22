CC = gcc
CFLAGS = -Wall -Wextra -I./include `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0` -lsqlite3

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
TARGET = $(BIN_DIR)/college_finance

all: directories $(TARGET)

directories:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR) data

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build complete! Run with: ./$(TARGET)"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run directories
