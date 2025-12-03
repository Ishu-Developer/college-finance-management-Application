CC = gcc
CFLAGS = -Wall -Wextra -O2 -I./include
CFLAGS += $(shell pkg-config --cflags gtk+-3.0 sqlite3)

LDFLAGS = $(shell pkg-config --libs gtk+-3.0 sqlite3)

SOURCES = \
	src/main.c \
	src/database/db_init.c \
	src/database/db_student.c \
	src/database/db_fee.c \
	src/database/db_employee.c \
	src/database/db_accounts.c \
	src/database/db_transaction.c \
	src/database/db_payroll.c \
	src/ui/student_ui.c \
	src/ui/fee_ui.c \
	src/ui/employee_ui.c \
	src/utils/logger.c \
	src/utils/validators.c

# Convert sources to object files
OBJECTS = $(SOURCES:.c=.o)

# Build directories
BUILD_DIR = build
BIN_DIR = bin

# Target executable
TARGET = $(BIN_DIR)/college_finance


# Default target
all: $(TARGET)

# Create target executable
$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	@echo "[LINKING] Creating executable: $(TARGET)"
	$(CC) -o $@ $^ $(LDFLAGS)
	@echo "[SUCCESS] Build complete: $(TARGET)"

# Compile source files to object files
%.o: %.c
	@mkdir -p $(BUILD_DIR)
	@echo "[COMPILING] $<"
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean distclean run debug help install-deps check info

# Clean object files only (keep executable)
clean:
	@echo "[CLEAN] Removing object files..."
	rm -f $(OBJECTS)
	@echo "[CLEAN] Done."

# Clean all build artifacts
distclean: clean
	@echo "[DISTCLEAN] Removing all build artifacts..."
	rm -f $(TARGET)
	rm -rf $(BIN_DIR)
	rm -rf $(BUILD_DIR)
	@echo "[DISTCLEAN] Done."

# Run the compiled application
run: $(TARGET)
	@echo "[RUN] Starting application..."
	$(TARGET)

# Build and run with debug output
debug: CFLAGS += -g -O0 -DDEBUG
debug: distclean $(TARGET)
	@echo "[DEBUG] Running with debug symbols..."
	$(TARGET)

# Run with GDB debugger
gdb: CFLAGS += -g -O0
gdb: distclean $(TARGET)
	@echo "[GDB] Running with GDB debugger..."
	gdb $(TARGET)

# Check compilation without linking
check:
	@echo "[CHECK] Checking compilation..."
	@for file in $(SOURCES); do \
		echo "[CHECK] $$file"; \
		$(CC) $(CFLAGS) -c $$file -o /tmp/$$(basename $$file .c).o || exit 1; \
	done
	@echo "[CHECK] All files compile successfully."

# Display build information
info:
	@echo "════════════════════════════════════════════════════════════"
	@echo "        College Finance Management System - Build Info"
	@echo "════════════════════════════════════════════════════════════"
	@echo ""
	@echo "[BUILD] Configuration:"
	@echo "  Compiler: $(CC)"
	@echo "  C Flags: $(CFLAGS)"
	@echo "  Link Flags: $(LDFLAGS)"
	@echo "  Target: $(TARGET)"
	@echo ""
	@echo "[SOURCES] Source Files ($(words $(SOURCES)) total):"
	@for file in $(SOURCES); do echo "    ✓ $$file"; done
	@echo ""
	@echo "[DEPENDENCIES] Library Versions:"
	@pkg-config --modversion gtk+-3.0 2>/dev/null && echo "    ✓ GTK+ 3" || echo "    ✗ GTK+ 3 NOT FOUND"
	@pkg-config --modversion sqlite3 2>/dev/null && echo "    ✓ SQLite 3" || echo "    ✗ SQLite 3 NOT FOUND"
	@echo ""
	@echo "[DIRECTORIES] Build Structure:"
	@echo "    Source:  src/"
	@echo "    Include: include/"
	@echo "    Build:   $(BUILD_DIR)/"
	@echo "    Binary:  $(BIN_DIR)/"
	@echo ""

# Install dependencies (MSYS2 specific)
install-deps:
	@echo "[INSTALL] Installing required packages..."
	pacman -S mingw-w64-ucrt64-gtk3 --noconfirm
	pacman -S mingw-w64-ucrt64-sqlite3 --noconfirm
	pacman -S mingw-w64-ucrt64-libharu --noconfirm
	pacman -S mingw-w64-ucrt64-curl --noconfirm
	@echo "[INSTALL] Dependencies installed."

# Display help
help:
	@echo ""
	@echo "╔════════════════════════════════════════════════════════════╗"
	@echo "║    College Finance Management System - Makefile Help       ║"
	@echo "╚════════════════════════════════════════════════════════════╝"
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Available Targets:"
	@echo "  all          Build the application (default)"
	@echo "  run          Build and run the application"
	@echo "  clean        Remove object files only"
	@echo "  distclean    Remove all build artifacts"
	@echo "  debug        Build with debug symbols and run"
	@echo "  gdb          Run with GDB debugger"
	@echo "  check        Check compilation without linking"
	@echo "  info         Display build configuration"
	@echo "  help         Display this help message"
	@echo "  install-deps Install required dependencies"
	@echo ""
	@echo "Examples:"
	@echo "  make              # Build application"
	@echo "  make run          # Build and run application"
	@echo "  make clean        # Remove object files"
	@echo "  make distclean    # Clean everything"
	@echo "  make debug        # Build with debug and run"
	@echo "  make info         # Show build configuration"
	@echo ""
	@echo "Configuration:"
	@echo "  CC:       $(CC)"
	@echo "  CFLAGS:   $(CFLAGS)"
	@echo "  LDFLAGS:  $(LDFLAGS)"
	@echo ""
	@echo "Project Structure:"
	@echo "  src/              Source files"
	@echo "  include/          Header files"
	@echo "  build/            Object files (temporary)"
	@echo "  bin/              Executable output"
	@echo ""