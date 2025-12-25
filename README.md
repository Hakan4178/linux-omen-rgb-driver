# linux-omen-rgb-driver
Linux kernel driver for HP Omen RGB keyboard - Mainline goal
CURRENTLY İN REVERSE ENGİNEERİNG!

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

THİS LİNE İS the daily *DEVelopment* line: But I searched for piped and couldn't find it, so I focused on the service part. SERVICE_NAME: HPOmenCap DISPLAY_NAME: HP Omen HSA Service I think I'll see all sorts of things on this road, it turns out that this kernel driver is not a very meaningful thing Still: GUID: 5B9766D8-6844-47AC-AB55-53952C9BE5BC  "OmenCap.exe = "HP Bridge" host The main job: Device.dll HIDService.dll Platform.dll SdkWrapperForNativeCode.dll This exe just: Security Authorization Object lifecycle IPC/RPC abstraction" 

   ↓
ACPI? → EC write

   ↓
Keyboard / RGB MCU






