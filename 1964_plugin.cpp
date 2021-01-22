//
// Created by Robin on 1/22/2021.
//

#include "1964_plugin.h"

//==========================================================================
// Purpose: Get the current state of the controllers buttons
// Input: Controller Number (0 to 3) - A pointer to a BUTTONS structure to be filled with the controller state
//==========================================================================
DLLEXPORT void CALL GetKeys(int Control, BUTTONS *Keys)
{
    if(Keys == nullptr)
        return;

    Keys->Value = !MainDll::getInstance()->IsConfigDialogOpen() ? Emulator::Controller[Control].Value : 0; // ignore input if config dialog is open
}
//==========================================================================
// Purpose: Initializes how each of the controllers should be handled
// Input: The handle to the main window - A controller structure that needs to be filled for the emulator to know how to handle each controller
// Changed Globals: ctrlptr, mousetoggle, PROFILE.SETTINGS
//==========================================================================
DLLEXPORT void CALL InitiateControllers(HWND hMainWindow, CONTROL Controls[4])
{
if(!MainDll::getInstance()->InitiateControllers(hMainWindow, Controls)) {
MessageBoxA(hMainWindow,
"Joyshock Input did not locate any usable controllers.\n\nPlease connect devices and restart the emulator.",
"Joyshock Input - Error", MB_ICONERROR | MB_OK);
}
}
//==========================================================================
// Purpose: Initializes how each of the controllers should be handled
// Input: Controller Number (0 to 3) and -1 signaling end of processing the pif ram - Pointer of data to be processed
// Note: This function is only needed if the DLL is allowing raw data
//==========================================================================
DLLEXPORT void CALL ReadController(int Control, BYTE *Command)
{
    return;
}
//==========================================================================
// Purpose: Called when a ROM is closed
// Changed Globals: mousetoggle
//==========================================================================
DLLEXPORT void CALL RomClosed(void)
{
    MainDll::getInstance()->EndInjection();
}
//==========================================================================
// Purpose: Called when a ROM is open (from the emulation thread)
// Changed Globals: emulatorwindow
//==========================================================================
DLLEXPORT void CALL RomOpen(void)
{
    JoyShockDriver::getInstance()->AssignEmulatorWindow(GetForegroundWindow());
    MainDll::getInstance()->StartInjection();
}
//==========================================================================
// Purpose: To pass the WM_KeyDown message from the emulator to the plugin
// Input: wParam and lParam of the WM_KEYDOWN message
// Changed Globals: emulatorwindow, windowactive
//==========================================================================
DLLEXPORT void CALL WM_KeyDown(WPARAM wParam, LPARAM lParam)
{
}
//==========================================================================
// Purpose: To pass the WM_KEYUP message from the emulator to the plugin
// Input: wParam and lParam of the WM_KeyUp message
//==========================================================================
DLLEXPORT void CALL WM_KeyUp(WPARAM wParam, LPARAM lParam)
{
}
//==========================================================================
// Purpose: Give rdram pointer to the plugin (called every second)
// Input: pointer to emulator's rdram and overclock factor
// Changed Globals: rdramptr, emuoverclock
//==========================================================================
DLLEXPORT void CALL HookRDRAM(DWORD *Mem, int OCFactor)
{
EmulatorLink::getInstance()->SetRAMPointer((const unsigned char **)Mem);
MainDll::getInstance()->SetEmulatorOverclock(OCFactor >= 3); // an overclock above 3 is guaranteed to be 60fps, so set to 0 if below 3 times overclock
//DRP_Update(); // init and update discord rich presence (discord will limit update rate to once every 15 seconds)
}
//==========================================================================
// Purpose: Give rom file pointer to the plugin on boot (for patching fov)
// Input: pointer to emulator's loaded rom
// Changed Globals: romptr
//==========================================================================
DLLEXPORT void CALL HookROM(DWORD *Rom)
{
EmulatorLink::getInstance()->SetROMPointer((const unsigned char **)Rom);
}