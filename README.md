# linux-omen-rgb-driver
   reddit: u/ Linux kernel driver for HP Omen RGB keyboard - Mainline goal
CURRENTLY İN REVERSE ENGİNEERİNG!

Big Breakthrough!

Full architecture:

App (HP Omen Lighting Service / UI)
   ↓
OmenSDK.dll  (user‑mode API)
   ↓
WMI or DeviceIoControl
   ↓
OMENLighting.sys (kernel driver)
   ↓
ACPI → EC write
   ↓
Keyboard / RGB MCU


Note: Reverse engineering is finished for the minimum limited features. I'm going crazy with Linux user space virtual drive issues. I want this whole project to work on both the Victus and Omen series, or in general, on every device that runs Omen Light Studio. I'll also write a simple user space tool for testing purposes, but the main purpose is the kernel. I'll leave an advanced API for the user space. The program has an extremely complex flow.






