# linux-omen-rgb-driver
Linux kernel driver for HP Omen RGB keyboard - Mainline goal
CURRENTLY İN REVERSE ENGİNEERİNG!

I'm finally back, I'm still actively trying new ways every day.


Big Breakthrough!

Full architecture:

App (HP Omen Lighting Service / UI)

   ↓
   
WMI suspicion and pipe and Something strange happened, I learned a lot from the log file, the RGB change does not go directly to the hardware. 

UI → Named Pipe (CommandsServerIPC) → a one-stop point ACPI/WMI → background service. 

This means that: UI (WPF / Aurora layer) Sends command via Named Pipe The pipe name is dynamic but the pattern is fixed.

Aurora.dll (effects / bitmap)
             
Named Pipe (CommandsServerIPC) 
             
Omen Lighting Service (background) 

THİS LİNE İS the daily *DEVelopment* line: But I searched for piped and couldn't find it, so I focused on the service part. SERVICE_NAME: HPOmenCap DISPLAY_NAME: HP Omen HSA Service

   ↓
ACPI → EC write

   ↓
Keyboard / RGB MCU


Note: Reverse engineering is NOT finished for the minimum limited features. I'm going crazy with Linux user space virtual drive issues. I want this whole project to work on both the Victus and Omen series, or in general, on every device that runs Omen Light Studio. I'll also write a simple user space tool for testing purposes, but the main purpose is the kernel. I'll leave an advanced API for the user space. The program has an extremely complex flow.






