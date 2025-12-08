CC = gcc
CFLAGS = -Wall -Wextra -O2 -I./include $(shell pkg-config --cflags gtk+-3.0 sqlite3)
LDFLAGS = $(shell pkg-config --libs gtk+-3.0 sqlite3)

SOURCES = src/main.c src/database/db_init.c src/database/db_student.c src/database/db_fee.c src/database/db_employee.c src/database/db_payroll.c src/logic/payroll_logic.c src/ui/payroll_ui.c src/ui/student_ui.c src/ui/fee_ui.c src/ui/employee_ui.c src/utils/logger.c src/utils/validators.c

OBJECTS = $(SOURCES:.c=.o)
BUILD_DIR = build
BIN_DIR = bin
TARGET = $(BIN_DIR)/college_finance

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	@echo "[LINKING] Creating executable: $(TARGET)"
	$(CC) -o $@ $^ $(LDFLAGS)
	@echo "[SUCCESS] Build complete: $(TARGET)"

%.o: %.c
	@mkdir -p $(BUILD_DIR)
	@echo "[COMPILING] $<"
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean distclean run debug check info help install-deps

clean:
	@echo "[CLEAN] Removing object files..."
	rm -f $(OBJECTS)

distclean: clean
	@echo "[DISTCLEAN] Removing all build artifacts..."
	rm -f $(TARGET)
	rm -rf $(BIN_DIR) $(BUILD_DIR)

run: $(TARGET)
	@echo "[RUN] Starting application..."
	$(TARGET)

debug: CFLAGS += -g -O0 -DDEBUG
debug: distclean $(TARGET)
	$(TARGET)

check:
	@echo "[CHECK] Checking compilation..."
	@for file in $(SOURCES); do $(CC) $(CFLAGS) -c $$file -o /dev/null || exit 1; done
	@echo "[CHECK] All files compile successfully."

info:
	@echo "Build Configuration:"
	@echo "  Compiler: $(CC)"
	@echo "  Target: $(TARGET)"
	@echo "  Sources: $(words $(SOURCES)) files"
	@pkg-config --modversion gtk+-3.0 2>/dev/null && echo "  GTK3: Found" || echo "  GTK3: NOT FOUND"
	@pkg-config --modversion sqlite3 2>/dev/null && echo "  SQLite3: Found" || echo "  SQLite3: NOT FOUND"

install-deps:
	@echo "[INSTALL] Installing packages..."
	pacman -S mingw-w64-ucrt64-gtk3 mingw-w64-ucrt64-sqlite3 --noconfirm
	@echo "[INSTALL] Done."

help:
	@echo "College Finance Management System - Build Help"
	@echo "make           - Build the application"
	@echo "make run       - Build and run"
	@echo "make clean     - Remove object files"
	@echo "make distclean - Remove all build files"
	@echo "make info      - Show build configuration"
	@echo "make check     - Check compilation"
	@echo "make debug     - Build with debug symbols"
	@echo "make install-deps - Install dependencies"
