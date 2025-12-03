##OMEN/Victus Linux RGB Control - User Guide 🐧

🎯 PHASE 1: Hardware Architecture Discovery
Purpose: To discover how RGB control works in your Victus/OMEN system and validate our virtual HID architecture theory.

🛠️ PREPARING THE TOOLS
1. Transferring Files to Linux:
# Copy all files to the Linux system:
# - hp_ec_safe_test.c
# - hp_acpi_safe_test.c
# - Makefile
# - README.md
# - SAFETY_GUIDE.md
2. Compilation:
# Install required packages (Ubuntu/Debian):
sudo apt update
sudo apt install build-essential gcc make

# Compilation:
make all

# Output:
# gcc -Wall -Wextra -O2 -fstack-protector-strong -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security -c hp_ec_safe_test.c
# gcc -Wall -Wextra -O2 -fstack-protector-strong -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security -c hp_acpi_safe_test.c
# gcc -Wl,-z,now,-z,relro -o hp_ec_safe_test hp_ec_safe_test.o
# gcc -Wl,-z,now,-z,relro -o hp_acpi_safe_test hp_acpi_safe_test.o
3. Checking File Permissions:
ls -la hp_*_test
# -rwxr-xr-x 1 user user 45632 Dec 1 22:00 hp_acpi_safe_test
# -rwxr-xr-x 1 user user 52480 Dec 1 22:00 hp_ec_safe_test

🔍 USING THE TOOLS

STEP 1: Gathering System Information (SAFE)
A) ACPI Method Discovery (SAFEEST):
sudo ./hp_acpi_safe_test
Expected Output:

[INFO] Starting HP OMEN/Victus ACPI method discovery...
[INFO] Product name: HP Victus by HP Laptop 16-e0xxx
[INFO] Product version: Type1ProductConfigId
[INFO] BIOS version: F.23
[INFO] Board name: 87B2
[INFO] Checking ACPI methods...
[INFO] Found ACPI method: \_SB.PC00.LPCB.EC0._Q66
[INFO] Found ACPI method: \_SB.WMID.WQMO
[INFO] RGB-related ACPI methods discovered: 2
[INFO] ACPI discovery completed successfully
What We Will Learn From This Output:

✅ ACPI Method Existence: Are there ACPI methods for RGB control?
✅ Presence of EC (Embedded Controller): Is there an EC0 device?
✅ WMI Interface: Is there WMID Windows Management Interface?
✅ HP-Specific Methods: HP-specific ACPI methods

B) System Hardware Analysis:

sudo ./hp_ec_safe_test --info
Expected Output:

[INFO] HP OMEN/Victus Hardware Analysis
[INFO] Product name: HP Victus by HP Laptop 16-e0xxx
[INFO] Product version: Type1ProductConfigId
[INFO] BIOS version: F.23
[INFO] Board name: 87B2
[INFO] System vendor: HP
[INFO] OMEN/Victus system detected: YES
[INFO] Security level: Standard (no Secure Boot restrictions)
[INFO] VM detection: Running on bare metal
[INFO] Conflicting modules: None detected
[INFO] EC ports accessible: YES (0x62, 0x66)
[INFO] System ready for EC testing
From This Output What we'll learn:

✅ HP System Confirmation: Is it really an HP OMEN/Victus system?
✅ EC Port Access: Is there access to the Embedded Controller ports?
✅ Security Status: Are there any BIOS security restrictions?
✅ VM Detection: Is it a virtual machine or real hardware?
STEP 2: EC Architecture Discovery (MEDIUM RISK)

C) EC Command Discovery:

sudo ./hp_ec_safe_test
Expected Output Scenarios:

SCENARIO 1 - SUCCESSFUL DISCOVERY:

[INFO] Starting HP OMEN/Victus EC discovery...
[INFO] Testing EC command: 0x51
[INFO] EC Status before: 0x00
[INFO] Sending command 0x51...
[INFO] EC Status after: 0x50
[INFO] EC Data: 0x01 0x02 0x03 0x04
[INFO] ✅ BREAKTHROUGH: EC responds to RGB commands!
[INFO] Command 0x51 successful - RGB controller found!
[INFO] EC discovery completed successfully

SCENARIO 2 - NO EC:

[INFO] Starting HP OMEN/Victus EC discovery...
[INFO] Testing EC command: 0x51
[INFO] EC Status: 0xFF (No EC response)
[INFO] EC not responding to RGB commands
[INFO] This system may use ACPI-only RGB control
[INFO] Try ACPI method approach instead

SCENARIO 3 - VIRTUAL DRIVER:

[INFO] Starting HP OMEN/Victus EC discovery...
[INFO] Testing EC command: 0x51
[INFO] EC Status: 0x00 (Ready)
[INFO] Command sent, but no RGB response
[INFO] ✅ BREAKTHROUGH: Virtual HID architecture confirmed!
[INFO] System uses Windows driver layer for RGB control
[INFO] Direct EC access blocked by virtual driver

📊 ANALYSIS OF RESULTS

SUCCESSFUL RESULT - Direct EC Access:

✅ EC responds to RGB commands
✅ Direct hardware control possible
✅ Can develop kernel module with EC interface
✅ Phase 2: Implementation full RGB protocol via EC
ACPI-Only Conclusion:
✅ ACPI methods available for RGB control
✅ Higher-level interface through ACPI
✅ Safer approach through ACPI calls
✅ Phase 2: Implement RGB control via ACPI methods
Virtual Driver Result:
✅ Virtual HID architecture confirmed
✅ Need to reverse engineer OMENLighting.sys
✅ Complex but achievable approach
✅ Phase 2: Implement virtual HID layer

🎯 STRATEGY ACCORDING TO PHASE 1 RESULTS

Result 1: Direct EC Access Discover

# Preparation for Phase 2:

# 1. Extend the EC command protocol
# 2. Discover RGB zone mapping
# 3. Develop the kernel module
Result 2: ACPI-Only Interface

# Preparation for Phase 2:

# 1. Discover ACPI method parameters
# 2. Analyze the WMI interface
# 3. Develop the platform driver
Result 3: Virtual HID Architecture
# Preparation for Phase 2:
# 1. Reverse engineer the OMENLighting.sys driver
# 2. Discover the IOCTL codes
# 3. Implement the Virtual HID layer

🚨 SECURITY WARNINGS

Before Running:
# 1. Take a system backup
sudo timeshift --create --comments "Before OMEN RGB testing"

# 2. Prepare a recovery USB
# 3. Note BIOS/UEFI settings
# 4. Back up important files
Test Order:
# 1. MOST SECURE: ACPI discovery
sudo ./hp_acpi_safe_test

# 2. MEDIUM SECURE: System info
sudo ./hp_ec_safe_test --info

# 3. MEDIUM RISK: EC discovery
sudo ./hp_ec_safe_test

# 4. Stop IMMEDIATELY if there are any problems!

📈 EXPECTED LEARNING OUTCOMES

Architecture Discovery:

✅ RGB Control Method: EC, ACPI, or Virtual HID?
✅ Hardware Interface: Which ports/methods are used?
✅ Command Protocol: Which commands control RGB?
✅ System Compatibility: Does this system support RGB control?
✅ Technical Intelligence:
✅ EC Command Set: Embedded Controller command set
✅ ACPI Method List: Available ACPI methods for RGB
✅ Hardware Topology: Location of the RGB controller within the system
✅ Driver Architecture: Which driver approach is required?

✅ Phase 2 Direction: Which approach will we proceed with?
✅ Development Priority: Prioritized development areas
✅ Risk Assessment: Which approach is more secure?
✅ Timeline Estimation: How long can it take to complete?

🎉 SUCCESS CRITERIA

Minimum Success:
✅ System recognized as HP OMEN/Victus
✅ RGB hardware detected
✅ At least one interface method discovered
Optimal Success:
✅ Direct EC access working
✅ RGB commands responding
✅ Hardware topology fully understood
Maximum Success:
✅ One-button RGB control successful
✅ Zone mapping discovered
✅ Full roadmap ready for Phase 2

🚀 NEXT STEPS
Phase 1 Success:
# Phase 2 development will begin:
# 1. Full RGB protocol implementation
# 2. Multi-zone support
# 3. Animation effects
# 4. Kernel module development
Phase 1 Partial Success:
# Alternative approach:
# 1. ACPI-based implementation
# 2. WMI interface development
# 3. Platform driver approach
If Phase 1 Fails:
# Fallback strategy:
# 1. Windows VM + USB passthrough
# 2. Firmware reverse engineering
# 3. Community collaboration (OpenRGB)

📞 SUPPORT and REPORTING

When Sharing Results:
# Copy the full output:

sudo ./hp_acpi_safe_test > acpi_results.txt 2>&1
sudo ./hp_ec_safe_test --info > ec_info.txt 2>&1
sudo ./hp_ec_safe_test > ec_test.txt 2>&1

# Share these files
In Case of Problem:
# Emergency recovery:
sudo systemctl reboot

# Check the logs:
dmesg | tail -50
journalctl -xe | tail -50

🎯 ARE YOU READY? Let's start discovering the secrets of RGB control in Linux! 🚀

