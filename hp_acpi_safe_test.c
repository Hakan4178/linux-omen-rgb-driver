/*
 * HP OMEN/Victus Keyboard RGB Control - ACPI Method Test Tool
 * 
 * SAFER ALTERNATIVE to direct EC access
 * 
 * This tool uses ACPI methods instead of direct EC port access,
 * which is safer and more compatible with modern systems.
 * 
 * Prerequisites:
 * - modprobe acpi_call
 * - /proc/acpi/call interface available
 * 
 * Author: OMEN Linux Project
 * License: GPL v3
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

// ACPI Method Patterns for HP OMEN/Victus
static const char* hp_acpi_methods[] = {
    "\\_SB.PC00.LPCB.EC0.WRAM",     // Write RAM
    "\\_SB.PC00.LPCB.EC0.KBCL",     // Keyboard Color
    "\\_SB.PC00.LPCB.EC0.SKBL",     // Set Keyboard Backlight
    "\\_SB.PCI0.LPCB.EC0.WRAM",     // Alternative path
    "\\_SB.PCI0.LPCB.EC0.KBCL",     // Alternative path
    "\\_SB.PCI0.LPCB.EC0.SKBL",     // Alternative path
    NULL
};

// Global state
static int verbose_mode = 0;

// Function Prototypes
static int check_acpi_call_support(void);
static int test_acpi_method(const char* method);
static int call_acpi_method(const char* method, const char* args);
static void print_usage(const char* progname);
static void safety_info(void);

/*
 * Check if acpi_call module is loaded and accessible
 */
static int check_acpi_call_support(void) {
    printf("[SAFETY] Checking ACPI call support...\n");
    
    // Check if /proc/acpi/call exists
    if (access("/proc/acpi/call", F_OK) != 0) {
        printf("[ERROR] /proc/acpi/call not found\n");
        printf("[INFO] Please run: sudo modprobe acpi_call\n");
        return -1;
    }
    
    // Check if we can write to it
    if (access("/proc/acpi/call", W_OK) != 0) {
        printf("[ERROR] Cannot write to /proc/acpi/call (need root privileges)\n");
        return -1;
    }
    
    printf("[SAFETY] ACPI call interface available\n");
    return 0;
}

/*
 * Call an ACPI method with arguments
 */
static int call_acpi_method(const char* method, const char* args) {
    FILE *fp;
    char command[512];
    char response[256];
    
    // Construct ACPI call command
    if (args && strlen(args) > 0) {
        snprintf(command, sizeof(command), "%s %s", method, args);
    } else {
        snprintf(command, sizeof(command), "%s", method);
    }
    
    if (verbose_mode) {
        printf("[DEBUG] ACPI call: %s\n", command);
    }
    
    // Write command to /proc/acpi/call
    fp = fopen("/proc/acpi/call", "w");
    if (!fp) {
        printf("[ERROR] Cannot open /proc/acpi/call for writing\n");
        return -1;
    }
    
    if (fprintf(fp, "%s\n", command) < 0) {
        printf("[ERROR] Failed to write ACPI command\n");
        fclose(fp);
        return -1;
    }
    fclose(fp);
    
    // Read response
    fp = fopen("/proc/acpi/call", "r");
    if (!fp) {
        printf("[ERROR] Cannot open /proc/acpi/call for reading\n");
        return -1;
    }
    
    if (fgets(response, sizeof(response), fp)) {
        response[strcspn(response, "\n")] = 0;
        printf("[INFO] ACPI response: %s\n", response);
        fclose(fp);
        return 0;
    } else {
        printf("[WARNING] No ACPI response\n");
        fclose(fp);
        return -1;
    }
}

/*
 * Test an ACPI method for existence
 */
static int test_acpi_method(const char* method) {
    printf("[INFO] Testing ACPI method: %s\n", method);
    
    // Try to call the method without arguments first
    int result = call_acpi_method(method, NULL);
    
    if (result == 0) {
        printf("[SUCCESS] Method %s exists and responded\n", method);
        return 0;
    } else {
        printf("[INFO] Method %s not available or failed\n", method);
        return -1;
    }
}

/*
 * Display safety information
 */
static void safety_info(void) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                 ACPI METHOD TEST TOOL                       ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║ This tool uses ACPI methods instead of direct EC access.    ║\n");
    printf("║ This is SAFER than direct port I/O and more compatible.     ║\n");
    printf("║                                                              ║\n");
    printf("║ ADVANTAGES:                                                  ║\n");
    printf("║ • No direct hardware access                                 ║\n");
    printf("║ • Uses kernel ACPI subsystem                                ║\n");
    printf("║ • Better compatibility with modern systems                  ║\n");
    printf("║ • Lower risk of system instability                          ║\n");
    printf("║                                                              ║\n");
    printf("║ REQUIREMENTS:                                                ║\n");
    printf("║ • sudo modprobe acpi_call                                   ║\n");
    printf("║ • Root privileges                                           ║\n");
    printf("║ • HP OMEN/Victus system                                     ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

/*
 * Print usage information
 */
static void print_usage(const char* progname) {
    printf("HP OMEN/Victus ACPI Method Test Tool\n\n");
    printf("Usage: %s [options]\n\n", progname);
    printf("Options:\n");
    printf("  --help          Show this help message\n");
    printf("  --verbose       Enable verbose debug output\n");
    printf("  --method <name> Test specific ACPI method\n");
    printf("  --call <method> <args> Call method with arguments\n");
    printf("\n");
    printf("Examples:\n");
    printf("  sudo %s                                    # Test all known methods\n", progname);
    printf("  sudo %s --method \"\\_SB.PC00.LPCB.EC0.KBCL\" # Test specific method\n", progname);
    printf("  sudo %s --call \"\\_SB.PC00.LPCB.EC0.WRAM\" \"0x01 0xFF 0x00 0x00\" # Call with args\n", progname);
    printf("\n");
    printf("Prerequisites:\n");
    printf("  sudo modprobe acpi_call\n");
    printf("  # Verify: ls -la /proc/acpi/call\n");
}

/*
 * Main function
 */
int main(int argc, char *argv[]) {
    int verbose_mode = 0;
    char *test_method = NULL;
    char *call_method = NULL;
    char *call_args = NULL;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "--verbose") == 0) {
            verbose_mode = 1;
        } else if (strcmp(argv[i], "--method") == 0 && i + 1 < argc) {
            test_method = argv[++i];
        } else if (strcmp(argv[i], "--call") == 0 && i + 2 < argc) {
            call_method = argv[++i];
            call_args = argv[++i];
        } else {
            printf("Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }
    
    safety_info();
    
    printf("HP OMEN/Victus ACPI Method Test Tool\n");
    printf("====================================\n");
    
    // Check if running as root
    if (geteuid() != 0) {
        printf("[ERROR] This tool requires root privileges\n");
        printf("[ERROR] Please run with: sudo %s\n", argv[0]);
        return 1;
    }
    
    // Check ACPI call support
    if (check_acpi_call_support() != 0) {
        return 1;
    }
    
    // Handle specific method test
    if (test_method) {
        printf("\n=== TESTING SPECIFIC METHOD ===\n");
        return test_acpi_method(test_method);
    }
    
    // Handle method call with arguments
    if (call_method) {
        printf("\n=== CALLING METHOD WITH ARGUMENTS ===\n");
        return call_acpi_method(call_method, call_args);
    }
    
    // Test all known HP ACPI methods
    printf("\n=== TESTING ALL KNOWN HP ACPI METHODS ===\n");
    
    int found_methods = 0;
    for (int i = 0; hp_acpi_methods[i]; i++) {
        if (test_acpi_method(hp_acpi_methods[i]) == 0) {
            found_methods++;
        }
        printf("\n");
    }
    
    printf("=== RESULTS ===\n");
    printf("Found %d working ACPI methods out of %d tested\n", 
           found_methods, (int)(sizeof(hp_acpi_methods)/sizeof(hp_acpi_methods[0]) - 1));
    
    if (found_methods > 0) {
        printf("\n[SUCCESS] ACPI methods available for RGB control!\n");
        printf("[INFO] This confirms ACPI-based architecture\n");
        printf("[INFO] Next step: Reverse engineer method parameters\n");
    } else {
        printf("\n[INFO] No standard ACPI methods found\n");
        printf("[INFO] May need DSDT analysis to find custom methods\n");
        printf("[INFO] Try: sudo acpidump > acpi_dump.dat\n");
        printf("[INFO] Then: iasl -d acpi_dump.dat\n");
    }
    
    return 0;
}