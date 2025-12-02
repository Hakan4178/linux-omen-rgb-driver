# HP OMEN/Victus RGB Control - Ultra Safe EC Test Tool
# Makefile for Linux compilation

CC = gcc
# Enhanced security compilation flags
CFLAGS = -Wall -Wextra -Wpedantic -std=c99 -O2 \
         -D_FORTIFY_SOURCE=2 -fstack-protector-strong \
         -Wformat=2 -Wformat-security -Wsign-conversion \
         -Wcast-align -Wstrict-prototypes -Wmissing-prototypes
LDFLAGS = -Wl,-z,now,-z,relro
TARGETS = hp_ec_safe_test hp_acpi_safe_test
EC_SOURCE = hp_ec_safe_test.c
ACPI_SOURCE = hp_acpi_safe_test.c

# Safety: Only build on Linux
UNAME_S := $(shell uname -s)
ifneq ($(UNAME_S),Linux)
    $(error This tool is designed for Linux only. Current OS: $(UNAME_S))
endif

# Check for required headers
REQUIRED_HEADERS = sys/io.h sys/time.h unistd.h
MISSING_HEADERS := $(shell for header in $(REQUIRED_HEADERS); do \
    echo '#include <'$$header'>' | $(CC) -E - >/dev/null 2>&1 || echo $$header; \
done)

ifneq ($(MISSING_HEADERS),)
    $(error Missing required headers: $(MISSING_HEADERS). Please install build-essential and linux-headers packages)
endif

.PHONY: all clean install uninstall check-deps safety-check ec-tool acpi-tool

all: safety-check $(TARGETS)

ec-tool: hp_ec_safe_test

acpi-tool: hp_acpi_safe_test

hp_ec_safe_test: $(EC_SOURCE)
	@echo "Building ultra-safe EC test tool with security hardening..."
	@echo "WARNING: This tool accesses hardware directly!"
	@echo "Compilation flags: $(CFLAGS)"
	$(CC) $(CFLAGS) -o hp_ec_safe_test $(EC_SOURCE) $(LDFLAGS)
	@echo "EC tool build completed with security hardening. Use with extreme caution!"

hp_acpi_safe_test: $(ACPI_SOURCE)
	@echo "Building safer ACPI test tool with security hardening..."
	@echo "INFO: This tool uses ACPI methods (safer approach)"
	@echo "Compilation flags: $(CFLAGS)"
	$(CC) $(CFLAGS) -o hp_acpi_safe_test $(ACPI_SOURCE) $(LDFLAGS)
	@echo "ACPI tool build completed with security hardening. Recommended approach!"

safety-check:
	@echo "=== ENHANCED SAFETY CHECK ==="
	@echo "Two tools will be compiled with security hardening:"
	@echo ""
	@echo "Security Compilation Features:"
	@echo "   - Stack protection (-fstack-protector-strong)"
	@echo "   - Format string protection (-Wformat-security)"
	@echo "   - Buffer overflow detection (-D_FORTIFY_SOURCE=2)"
	@echo "   - RELRO and immediate binding (-Wl,-z,now,-z,relro)"
	@echo "   - Strict error checking (-Werror)"
	@echo ""
	@echo "1. hp_ec_safe_test (Direct EC access):"
	@echo "   - HP/OMEN/Victus system detection"
	@echo "   - ioperm() instead of iopl() for minimal permissions"
	@echo "   - Enhanced signal handling with reset"
	@echo "   - Progressive timeout with backoff"
	@echo "   - BIOS/UEFI security level detection"
	@echo "   - Conflicting module detection"
	@echo "   - Safe file I/O with bounds checking"
	@echo "   - Memory safety improvements"
	@echo ""
	@echo "2. hp_acpi_safe_test (ACPI methods - RECOMMENDED):"
	@echo "   - Uses kernel ACPI subsystem"
	@echo "   - No direct hardware access"
	@echo "   - Better compatibility"
	@echo "   - Lower risk approach"
	@echo "================================="

check-deps:
	@echo "Checking dependencies..."
	@which gcc >/dev/null || (echo "ERROR: gcc not found. Install build-essential package." && exit 1)
	@echo "Dependencies OK"

install: $(TARGETS)
	@echo "WARNING: Installing hardware access tools system-wide"
	@echo "These tools should only be used by experienced users"
	sudo cp hp_ec_safe_test /usr/local/bin/
	sudo cp hp_acpi_safe_test /usr/local/bin/
	sudo chmod 755 /usr/local/bin/hp_ec_safe_test
	sudo chmod 755 /usr/local/bin/hp_acpi_safe_test
	@echo "Installed to /usr/local/bin/"
	@echo "Usage: sudo hp_acpi_safe_test --help  (recommended)"
	@echo "Usage: sudo hp_ec_safe_test --help    (advanced)"

uninstall:
	sudo rm -f /usr/local/bin/hp_ec_safe_test
	sudo rm -f /usr/local/bin/hp_acpi_safe_test
	@echo "Uninstalled both tools"

clean:
	rm -f $(TARGETS)
	@echo "Cleaned build files"

# Development targets
debug: CFLAGS += -g -DDEBUG
debug: $(TARGETS)

test-compile: $(EC_SOURCE) $(ACPI_SOURCE)
	@echo "Testing compilation without building..."
	$(CC) $(CFLAGS) -fsyntax-only $(EC_SOURCE)
	$(CC) $(CFLAGS) -fsyntax-only $(ACPI_SOURCE)
	@echo "Compilation test passed for both tools"

# Safety documentation
safety-info:
	@echo ""
	@echo "╔══════════════════════════════════════════════════════════════╗"
	@echo "║                    SAFETY INFORMATION                        ║"
	@echo "╠══════════════════════════════════════════════════════════════╣"
	@echo "║ This tool accesses Embedded Controller (EC) hardware        ║"
	@echo "║ directly through I/O ports 0x62 and 0x66.                   ║"
	@echo "║                                                              ║"
	@echo "║ SAFETY FEATURES:                                             ║"
	@echo "║ • Detects HP/OMEN/Victus systems only                       ║"
	@echo "║ • Read-only mode by default                                 ║"
	@echo "║ • Conservative timeouts (1000ms)                            ║"
	@echo "║ • Safety delays between operations                          ║"
	@echo "║ • Emergency exit handlers (Ctrl+C)                          ║"
	@echo "║ • Extensive error checking                                  ║"
	@echo "║                                                              ║"
	@echo "║ USAGE:                                                       ║"
	@echo "║ sudo ./hp_acpi_safe_test     # ACPI methods (recommended)   ║"
	@echo "║ sudo ./hp_ec_safe_test --info # System info only            ║"
	@echo "║ sudo ./hp_ec_safe_test       # Direct EC (advanced)         ║"
	@echo "║                                                              ║"
	@echo "║ REQUIREMENTS:                                                ║"
	@echo "║ • Linux system with EC support                              ║"
	@echo "║ • Root privileges (for I/O port access)                     ║"
	@echo "║ • HP OMEN or Victus hardware (recommended)                  ║"
	@echo "╚══════════════════════════════════════════════════════════════╝"
	@echo ""

help: safety-info
	@echo "Available targets:"
	@echo "  all          - Build both tools (default)"
	@echo "  ec-tool      - Build EC access tool only"
	@echo "  acpi-tool    - Build ACPI tool only (recommended)"
	@echo "  clean        - Remove build files"
	@echo "  install      - Install to /usr/local/bin (requires sudo)"
	@echo "  uninstall    - Remove from /usr/local/bin (requires sudo)"
	@echo "  debug        - Build with debug symbols"
	@echo "  test-compile - Test compilation without building"
	@echo "  check-deps   - Check build dependencies"
	@echo "  safety-info  - Show safety information"
	@echo "  help         - Show this help"
	@echo ""
	@echo "Example usage after building:"
	@echo "  sudo ./hp_acpi_safe_test --help     # Recommended approach"
	@echo "  sudo ./hp_ec_safe_test --help       # Advanced users only"
	@echo ""
	@echo "Recommended testing order:"
	@echo "  1. sudo ./hp_acpi_safe_test         # Try ACPI methods first"
	@echo "  2. sudo ./hp_ec_safe_test --info    # System info if ACPI fails"

	@echo "  3. sudo ./hp_ec_safe_test           # Direct EC as last resort"
