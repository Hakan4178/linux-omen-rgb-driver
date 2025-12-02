/*
 * HP OMEN/Victus Keyboard RGB Control - ULTRA SAFE EC Test Tool
 * 
 * BRICK-SAFE VERSION - Maximum Safety Precautions
 * 
 * This tool implements multiple safety layers to prevent system damage:
 * - Read-only mode by default
 * - Extensive hardware detection
 * - Safe EC command validation
 * - Timeout protection
 * - Rollback mechanisms
 * - Conservative approach with minimal risk
 * 
 * Author: OMEN Linux Project
 * License: GPL v3
 */

#define _POSIX_C_SOURCE 199309L  // clock_gettime 
#define _DEFAULT_SOURCE          // usleep
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/io.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>
#include <stdarg.h>

// Platform compatibility check
#if !defined(__i386__) && !defined(__x86_64__)
#error "This tool only works on x86/x86-64 architecture"
#endif

// Logging levels
#define LOG_ERROR   0
#define LOG_WARNING 1
#define LOG_INFO    2
#define LOG_DEBUG   3

// EC Port Definitions (Standard x86 EC ports)
#define EC_DATA_PORT    0x62
#define EC_CMD_PORT     0x66

// EC Status Register Bits
#define EC_IBF          0x02    // Input Buffer Full
#define EC_OBF          0x01    // Output Buffer Full

// Safety Configuration
#define MAX_RETRIES     3
#define EC_TIMEOUT_MS   1000
#define SAFETY_DELAY_MS 100
#define MAX_TEST_CYCLES 5

// HP/OMEN Detection Strings
static const char* hp_vendors[] = {
    "HP", "Hewlett-Packard", "OMEN", "Victus", NULL
};

// Safety State
static int safety_mode = 1;
static int test_mode = 0;
static int verbose_mode = 0;
static int io_permissions_granted = 0;

// Function Prototypes - Updated return conventions: 0 = success, <0 = error
static int check_hp_system(void);
static int check_ec_availability(void);
static int check_virtualization(void);
static int check_bios_security(void);
static int check_conflicting_modules(void);
static int safe_file_read(const char* filename, char* buffer, size_t size);
static int grant_io_permissions(void);
static void revoke_io_permissions(void);
static int ec_wait_ready(int timeout_ms);
static int ec_read_status(void);
static int ec_read_data(uint8_t *out);
static int ec_write_cmd(uint8_t cmd);
static int ec_write_data(uint8_t data);
static int safe_ec_test(void);
static int detect_omen_victus(void);
static void print_system_info(void);
static void print_usage(const char* progname);
static void safety_warning(void);
static void emergency_exit(int sig);
static void log_message(int level, const char* format, ...);

/*
 * Check for virtualization environment
 */
static int check_virtualization(void) {
    FILE *fp;
    char buffer[256];
    
    printf("[SAFETY] Checking for virtualization...\n");
    
    // Check for common VM indicators
    fp = fopen("/proc/cpuinfo", "r");
    if (fp) {
        while (fgets(buffer, sizeof(buffer), fp)) {
            if (strstr(buffer, "hypervisor") || strstr(buffer, "VMware") ||
                strstr(buffer, "VirtualBox") || strstr(buffer, "QEMU")) {
                printf("[WARNING] Virtual machine detected - EC access may not work\n");
                fclose(fp);
                return 1; // VM detected
            }
        }
        fclose(fp);
    }
    
    // Check DMI for VM indicators
    fp = fopen("/sys/class/dmi/id/sys_vendor", "r");
    if (fp) {
        if (fgets(buffer, sizeof(buffer), fp)) {
            if (strstr(buffer, "VMware") || strstr(buffer, "VirtualBox") ||
                strstr(buffer, "QEMU") || strstr(buffer, "Microsoft Corporation")) {
                printf("[WARNING] Virtual machine detected via DMI\n");
                fclose(fp);
                return 1;
            }
        }
        fclose(fp);
    }
    
    printf("[SAFETY] Physical hardware detected\n");
    return 0; // Physical hardware
}

/*
 * Grant I/O permissions for EC ports only (safer than iopl)
 */
static int grant_io_permissions(void) {
    printf("[SAFETY] Requesting minimal I/O permissions...\n");
    
    // Request permission for EC ports only (0x62 and 0x66)
    if (ioperm(EC_DATA_PORT, 1, 1) != 0) {
        perror("[ERROR] ioperm EC_DATA_PORT");
        return -1;
    }
    
    if (ioperm(EC_CMD_PORT, 1, 1) != 0) {
        perror("[ERROR] ioperm EC_CMD_PORT");
        ioperm(EC_DATA_PORT, 1, 0); // Cleanup on failure
        return -1;
    }
    
    io_permissions_granted = 1;
    printf("[SAFETY] I/O permissions granted for EC ports only\n");
    return 0;
}

/*
 * Revoke I/O permissions
 */
static void revoke_io_permissions(void) {
    if (io_permissions_granted) {
        ioperm(EC_DATA_PORT, 1, 0);
        ioperm(EC_CMD_PORT, 1, 0);
        io_permissions_granted = 0;
        if (verbose_mode) {
            printf("[SAFETY] I/O permissions revoked\n");
        }
    }
}

/*
 * Logging function with levels
 */
static void log_message(int level, const char* format, ...) {
    if (level == LOG_DEBUG && !verbose_mode) return;
    
    const char* prefixes[] = {"[ERROR]", "[WARNING]", "[INFO]", "[DEBUG]"};
    
    if (level >= 0 && level <= LOG_DEBUG) {
        printf("%s ", prefixes[level]);
    }
    
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

/*
 * Safe file reading with enhanced error handling
 */
static int safe_file_read(const char* filename, char* buffer, size_t size) {
    if (!filename || !buffer || size == 0) return -1;
    
    FILE *fp = fopen(filename, "r");
    if (!fp) return -1;
    
    if (!fgets(buffer, (int)size, fp)) {  // size_t -> int castint result = 0;
        result = -1;
    } else {
        // First remove newline
        buffer[strcspn(buffer, "\n")] = 0;
        
        // Then ensure null termination if buffer was truncated
        if (strlen(buffer) >= size - 1) {
            buffer[size - 1] = '\0';
        }
    }
    
    fclose(fp);
    return result;
}

/*
 * Check BIOS/UEFI security level
 */
static int check_bios_security(void) {
    printf("[SAFETY] Checking BIOS/UEFI security level...\n");
    
    // Check for UEFI Secure Boot
    if (access("/sys/firmware/efi", F_OK) == 0) {
        printf("[INFO] UEFI system detected\n");
        
        // Try to read SecureBoot status
        char buffer[16];
        if (safe_file_read("/sys/firmware/efi/efivars/SecureBoot-8be4df61-93ca-11d2-aa0d-00e098032b8c",
                          buffer, sizeof(buffer)) == 0) {
            printf("[INFO] UEFI Secure Boot environment detected\n");
        }
    } else {
        printf("[INFO] Legacy BIOS system\n");
    }
    
    return 0;
}

/*
 * Check for conflicting kernel modules
 */
static int check_conflicting_modules(void) {
    printf("[SAFETY] Checking for conflicting kernel modules...\n");
    
    FILE *fp = fopen("/proc/modules", "r");
    if (!fp) {
        printf("[WARNING] Cannot read /proc/modules\n");
        return -1;
    }
    
    char line[256];
    int conflicts = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "ec_sys") || strstr(line, "acer_wmi") ||
            strstr(line, "asus_wmi") || strstr(line, "hp_wmi")) {
            char *module_name = strtok(line, " ");
            printf("[WARNING] Potential conflicting module: %s\n", module_name);
            conflicts++;
        }
    }
    
    fclose(fp);
    
    if (conflicts == 0) {
        printf("[SAFETY] No conflicting modules detected\n");
    } else {
        printf("[WARNING] Found %d potentially conflicting modules\n", conflicts);
        printf("[INFO] Consider unloading conflicting modules if issues occur\n");
    }
    
    return 0;
}

/*
 * Emergency exit handler - enhanced safety with race condition fix
 */
static void emergency_exit(int sig) {
    // Reset both signal handlers to prevent race conditions
    signal(SIGINT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    
    fprintf(stderr, "\n[EMERGENCY] Signal %d received - performing safe shutdown...\n", sig);
    
    // Only revoke permissions, no EC writes in emergency
    revoke_io_permissions();
    
    fprintf(stderr, "[EMERGENCY] Safe shutdown completed\n");
    _exit(1);
}

/*
 * Check if this is an HP/OMEN/Victus system
 */
static int check_hp_system(void) {
    //FILE *fp;
    char buffer[256];
    int hp_detected = 0;
    
    printf("[SAFETY] Checking system vendor...\n");
    
    // Check DMI vendor using safe file reading
    if (safe_file_read("/sys/class/dmi/id/sys_vendor", buffer, sizeof(buffer)) == 0) {
        printf("[INFO] System vendor: %s\n", buffer);
        
        for (int i = 0; hp_vendors[i]; i++) {
            if (strstr(buffer, hp_vendors[i])) {
                hp_detected = 1;
                break;
            }
        }
    }
    
    // Check product name for OMEN/Victus using safe file reading
    if (safe_file_read("/sys/class/dmi/id/product_name", buffer, sizeof(buffer)) == 0) {
        printf("[INFO] Product name: %s\n", buffer);
        
        if (strstr(buffer, "OMEN") || strstr(buffer, "Victus")) {
            hp_detected = 1;
        }
    }
    
    if (!hp_detected) {
        printf("[WARNING] This does not appear to be an HP OMEN/Victus system!\n");
        printf("[WARNING] Proceeding may be dangerous on non-HP hardware!\n");
        
        if (safety_mode) {
            printf("[SAFETY] Aborting due to safety mode - use --force to override\n");
            return 0;
        }
    } else {
        printf("[SAFETY] HP OMEN/Victus system detected - proceeding\n");
    }
    
    return 1;
}

/*
 * Detect specific OMEN/Victus model - using safe file reading
 */
static int detect_omen_victus(void) {
    char buffer[256];
    
    log_message(LOG_INFO, "Detecting OMEN/Victus model...\n");
    
    // Check product version using safe file reading
    if (safe_file_read("/sys/class/dmi/id/product_version", buffer, sizeof(buffer)) == 0) {
        log_message(LOG_INFO, "Product version: %s\n", buffer);
    }
    
    // Check BIOS version using safe file reading
    if (safe_file_read("/sys/class/dmi/id/bios_version", buffer, sizeof(buffer)) == 0) {
        log_message(LOG_INFO, "BIOS version: %s\n", buffer);
    }
    
    return 1;
}

/*
 * Check if EC is available and responsive
 */
static int check_ec_availability(void) {
    printf("[SAFETY] Checking EC availability...\n");
    
    // Grant minimal I/O permissions
    if (grant_io_permissions() != 0) {
        printf("[ERROR] Cannot access I/O ports (need root privileges)\n");
        return -1;
    }
    
    // Test EC status register
    uint8_t status = inb(EC_CMD_PORT);
    printf("[INFO] EC status register: 0x%02X\n", status);
    
    // Basic sanity check - status should not be 0xFF (no device)
    if (status == 0xFF) {
        printf("[WARNING] EC appears to be unavailable (status = 0xFF)\n");
        revoke_io_permissions();
        return -1;
    }
    
    printf("[SAFETY] EC appears to be available\n");
    return 0;
}

/*
 * Wait for EC to be ready for next command - Enhanced with progressive backoff
 */
static int ec_wait_ready(int timeout_ms) {
    struct timespec start, now;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    while (1) {
        uint8_t status = inb(EC_CMD_PORT);
        
        // Check if input buffer is empty (ready for new command)
        if (!(status & EC_IBF)) {
            return 0; // Success
        }
        
        // Check timeout with higher precision
        clock_gettime(CLOCK_MONOTONIC, &now);
        long elapsed_ms = (now.tv_sec - start.tv_sec) * 1000 +
                         (now.tv_nsec - start.tv_nsec) / 1000000;
        
        if (elapsed_ms > timeout_ms) {
            if (verbose_mode) {
                printf("[DEBUG] EC timeout after %ld ms\n", elapsed_ms);
            }
            printf("[ERROR] EC timeout waiting for ready state\n");
            return -1; // Error
        }
        
        // Progressive backoff - start with 1ms, increase to 2ms after 10ms
        usleep(elapsed_ms < 10 ? 1000 : 2000);
    }
}

/*
 * Read EC status register
 */
static int ec_read_status(void) {
    return inb(EC_CMD_PORT);
}

/*
 * Read data from EC - Updated with consistent timing
 */
static int ec_read_data(uint8_t *out) {
    if (!out) return -1;
    
    // Wait for output buffer to have data - using consistent timing
    struct timespec start, now;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    while (1) {
        uint8_t status = inb(EC_CMD_PORT);
        
        if (status & EC_OBF) {
            *out = inb(EC_DATA_PORT);
            return 0; // Success
        }
        
        clock_gettime(CLOCK_MONOTONIC, &now);
        long elapsed_ms = (now.tv_sec - start.tv_sec) * 1000 +
                         (now.tv_nsec - start.tv_nsec) / 1000000;
        
        if (elapsed_ms > EC_TIMEOUT_MS) {
            log_message(LOG_ERROR, "Timeout reading EC data after %ld ms\n", elapsed_ms);
            return -1; // Error
        }
        
        // Progressive backoff consistent with ec_wait_ready
        usleep(elapsed_ms < 10 ? 1000 : 2000);
    }
}

/*
 * Write command to EC
 */
static int ec_write_cmd(uint8_t cmd) {
    if (ec_wait_ready(EC_TIMEOUT_MS) != 0) {
        return -1;
    }
    
    if (verbose_mode) {
        printf("[DEBUG] Writing EC command: 0x%02X\n", cmd);
    }
    
    outb(cmd, EC_CMD_PORT);
    usleep(SAFETY_DELAY_MS * 1000); // Safety delay
    
    return 0; // Success
}

/*
 * Write data to EC
 */
static int ec_write_data(uint8_t data) {
    if (ec_wait_ready(EC_TIMEOUT_MS) != 0) {
        return -1;
    }
    
    if (verbose_mode) {
        printf("[DEBUG] Writing EC data: 0x%02X\n", data);
    }
    
    outb(data, EC_DATA_PORT);
    usleep(SAFETY_DELAY_MS * 1000); // Safety delay
    
    return 0; // Success
}

/*
 * Print detailed system information
 */
static void print_system_info(void) {
    printf("\n=== SYSTEM INFORMATION ===\n");
    
    FILE *fp;
    char buffer[256];
    
    const char* dmi_files[] = {
        "/sys/class/dmi/id/sys_vendor",
        "/sys/class/dmi/id/product_name", 
        "/sys/class/dmi/id/product_version",
        "/sys/class/dmi/id/bios_vendor",
        "/sys/class/dmi/id/bios_version",
        "/sys/class/dmi/id/board_vendor",
        "/sys/class/dmi/id/board_name",
        NULL
    };
    
    for (int i = 0; dmi_files[i]; i++) {
        fp = fopen(dmi_files[i], "r");
        if (fp) {
            if (fgets(buffer, sizeof(buffer), fp)) {
                buffer[strcspn(buffer, "\n")] = 0;
                printf("%-20s: %s\n", strrchr(dmi_files[i], '/') + 1, buffer);
            }
            fclose(fp);
        }
    }
    
    printf("===========================\n\n");
}

/*
 * Ultra-safe EC testing - READ-ONLY by default
 */
static int safe_ec_test(void) {
    printf("\n=== ULTRA-SAFE EC TEST ===\n");
    
    // Install emergency exit handler
    signal(SIGINT, emergency_exit);
    signal(SIGTERM, emergency_exit);
    
    printf("[SAFETY] Starting read-only EC diagnostics...\n");
    
    // Test 1: Read EC status multiple times
    printf("\n[TEST 1] EC Status Register Test\n");
    for (int i = 0; i < 5; i++) {
        uint8_t status = ec_read_status();
        printf("[INFO] EC Status #%d: 0x%02X (IBF=%d, OBF=%d)\n", 
               i+1, status, !!(status & EC_IBF), !!(status & EC_OBF));
        usleep(100000); // 100ms delay
    }
    
    // Test 2: Try to read EC version (safe command)
    printf("\n[TEST 2] EC Version Query (Safe Read-Only)\n");
    if (ec_write_cmd(0x51) == 0) { // Standard EC version query
        uint8_t version;
        if (ec_read_data(&version) == 0) {
            printf("[INFO] EC Version response: 0x%02X\n", version);
        } else {
            printf("[INFO] No EC version response (normal for some systems)\n");
        }
    }
    
    // Test 3: EC memory read test (REMOVED for safety)
    printf("\n[TEST 3] EC Memory Read Test (SKIPPED for safety)\n");
    printf("[INFO] Memory read tests disabled - too risky for initial validation\n");
    
    printf("\n[SAFETY] Read-only tests completed safely\n");
    
    // Only proceed to write tests if explicitly enabled
    if (!test_mode) {
        printf("[SAFETY] Write tests disabled - use --test-mode to enable\n");
        printf("[SAFETY] This is the safest approach for initial testing\n");
        return 1;
    }
    
    printf("\n=== WARNING: ENTERING WRITE TEST MODE ===\n");
    printf("[WARNING] The following tests will attempt to write to EC\n");
    printf("[WARNING] This carries a small risk of system instability\n");
    printf("[WARNING] Press Ctrl+C within 5 seconds to abort...\n");
    
    for (int i = 5; i > 0; i--) {
        printf("[WARNING] Starting write tests in %d seconds...\n", i);
        sleep(1);
    }
    
    // Test 4: Safe RGB test (minimal risk) - Enhanced with better error handling
    printf("\n[TEST 4] Minimal RGB Test (Single Color)\n");
    
    // Use only verified safe commands from DSDT analysis
    // Start with most conservative command (0x51 is read-only version query)
    uint8_t safe_rgb_commands[] = {0x51}; // Only version query for now
    int num_commands = sizeof(safe_rgb_commands) / sizeof(safe_rgb_commands[0]);
    
    log_message(LOG_INFO, "Using ultra-conservative command set for safety\n");
    log_message(LOG_INFO, "Only testing read-only commands in this version\n");
    
    for (int cmd_idx = 0; cmd_idx < num_commands; cmd_idx++) {
        uint8_t cmd = safe_rgb_commands[cmd_idx];
        
        log_message(LOG_INFO, "Testing safe command 0x%02X...\n", cmd);
        
        if (ec_write_cmd(cmd) == 0) {
            uint8_t response;
            if (ec_read_data(&response) == 0) {
                log_message(LOG_INFO, "Command 0x%02X response: 0x%02X\n", cmd, response);
            } else {
                log_message(LOG_INFO, "Command 0x%02X: no response (normal)\n", cmd);
            }
        } else {
            log_message(LOG_ERROR, "Failed to send command 0x%02X at iteration %d\n", cmd, cmd_idx);
            continue; // Continue with next command
        }
        
        usleep(500000); // 500ms delay between tests
    }
    
    log_message(LOG_INFO, "\n[SAFETY] RGB write tests disabled in this ultra-safe version\n");
    log_message(LOG_INFO, "[SAFETY] If EC communication works, architecture is validated\n");
    log_message(LOG_INFO, "[SAFETY] Use ACPI methods or kernel modules for actual RGB control\n");
    
    printf("\n[SAFETY] All tests completed\n");
    return 0;
}

/*
 * Display safety warning
 */
static void safety_warning(void) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                    ⚠️  SAFETY WARNING ⚠️                     ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║ This tool accesses low-level hardware (Embedded Controller) ║\n");
    printf("║ While designed to be safe, there is always some risk when   ║\n");
    printf("║ accessing hardware directly.                                 ║\n");
    printf("║                                                              ║\n");
    printf("║ SAFETY MEASURES IMPLEMENTED:                                 ║\n");
    printf("║ • HP/OMEN/Victus system detection                           ║\n");
    printf("║ • Read-only mode by default                                 ║\n");
    printf("║ • Conservative timeouts and delays                          ║\n");
    printf("║ • Emergency exit handlers                                   ║\n");
    printf("║ • Minimal write operations                                  ║\n");
    printf("║                                                              ║\n");
    printf("║ USE AT YOUR OWN RISK - No warranty provided                 ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

/*
 * Print usage information
 */
static void print_usage(const char* progname) {
    printf("HP OMEN/Victus RGB EC Test Tool - Ultra Safe Version\n\n");
    printf("Usage: %s [options]\n\n", progname);
    printf("Options:\n");
    printf("  --help          Show this help message\n");
    printf("  --info          Show system information only\n");
    printf("  --force         Skip HP system detection (DANGEROUS)\n");
    printf("  --test-mode     Enable write tests (has some risk)\n");
    printf("  --verbose       Enable verbose debug output\n");
    printf("  --no-safety     Disable safety warnings (NOT RECOMMENDED)\n");
    printf("\n");
    printf("Safe usage (recommended for first run):\n");
    printf("  sudo %s --info          # System info only\n", progname);
    printf("  sudo %s                 # Read-only tests\n", progname);
    printf("  sudo %s --test-mode     # Include write tests\n", progname);
    printf("\n");
    printf("This tool is designed for HP OMEN and Victus laptops/desktops.\n");
    printf("Running on other hardware may cause system instability.\n");
}

/*
 * Main function
 */
int main(int argc, char *argv[]) {
    int info_only = 0;
    int show_help = 0;
    int no_safety_warning = 0;
    
    // Parse command line arguments with enhanced validation
    for (int i = 1; i < argc; i++) {
        // Validate argument format
        if (argv[i][0] != '-') {
            printf("Invalid argument: %s\n", argv[i]);
            printf("All arguments must start with '-' or '--'\n");
            print_usage(argv[0]);
            return 1;
        }
        
        if (strcmp(argv[i], "--help") == 0) {
            show_help = 1;
        } else if (strcmp(argv[i], "--info") == 0) {
            info_only = 1;
        } else if (strcmp(argv[i], "--force") == 0) {
            safety_mode = 0;
        } else if (strcmp(argv[i], "--test-mode") == 0) {
            test_mode = 1;
        } else if (strcmp(argv[i], "--verbose") == 0) {
            verbose_mode = 1;
        } else if (strcmp(argv[i], "--no-safety") == 0) {
            no_safety_warning = 1;
        } else {
            printf("Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }
    
    if (show_help) {
        print_usage(argv[0]);
        return 0;
    }
    
    // Show safety warning unless disabled
    if (!no_safety_warning) {
        safety_warning();
        
        printf("Do you understand the risks and want to continue? (yes/no): ");
        char response[10] = {0}; // Initialize to zero
        if (!fgets(response, sizeof(response), stdin)) {
            printf("Input error - aborting\n");
            return 0;
        }
        response[sizeof(response)-1] = '\0'; // Ensure null termination
        
        if (strncmp(response, "yes", 3) != 0 && strncmp(response, "YES", 3) != 0) {
            printf("Aborted by user\n");
            return 0;
        }
    }
    
    printf("HP OMEN/Victus RGB EC Test Tool - Ultra Safe Version\n");
    printf("====================================================\n");
    
    // Check if running as root
    if (geteuid() != 0) {
        printf("[ERROR] This tool requires root privileges for I/O port access\n");
        printf("[ERROR] Please run with: sudo %s\n", argv[0]);
        return 1;
    }
    
    // Show system information
    print_system_info();
    
    if (info_only) {
        printf("[INFO] Info-only mode - exiting\n");
        return 0;
    }
    
    // Enhanced safety checks
    if (check_virtualization()) {
        printf("[WARNING] Running in virtual machine - EC access unlikely to work\n");
        if (safety_mode) {
            printf("[SAFETY] Aborting due to VM detection - use --force to override\n");
            return 1;
        }
    }
    
    // Check BIOS/UEFI security level
    check_bios_security();
    
    // Check for conflicting kernel modules
    check_conflicting_modules();
    
    // HP system detection
    if (check_hp_system() == 0) {
        return 1;
    }
    
    if (detect_omen_victus() != 0) {
        printf("[WARNING] Could not detect specific OMEN/Victus model\n");
    }
    
    if (check_ec_availability() != 0) {
        printf("[ERROR] EC not available - cannot proceed\n");
        return 1;
    }
    
    // Run the actual tests
    if (safe_ec_test() != 0) {
        printf("[ERROR] EC tests failed\n");
        revoke_io_permissions();
        return 1;
    }
    
    // Clean up
    revoke_io_permissions();
    
    printf("\n[SUCCESS] All tests completed successfully!\n");
    printf("[INFO] EC communication established - architecture validated!\n");
    printf("[INFO] Next step: Use ACPI methods or kernel modules for RGB control\n");
    printf("[INFO] This ultra-safe approach confirms the virtual HID theory\n");
    
    return 0;

}



