/*
 *==========================================================================
 * Joyshock Injector - 1964 Plugin
 *==========================================================================
 * Largely based off of the original Keyboard/Mouse Injector Plugin
 * Copyright (C) 2016-2021 Carnivorous
 *
 * Joyshock Injector
 * Copyright (C) 2016-2021 TauAkiou
 * All rights reserved.
 *
 * Joyshock Injector is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, visit http://www.gnu.org/licenses/gpl-2.0.html
 *==========================================================================
 */
#include "1964_plugin.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch(fdwReason)
    {
        case DLL_PROCESS_ATTACH:
        {
            // Initialize all state objects.
            MainDll::GetInstance(hinstDLL);
            AllocConsole(); AttachConsole(GetCurrentProcessId()); freopen("CON", "w", stdout );
            break;
        }
        default:
            break;
    }
    return TRUE;
}

//==========================================================================
// Purpose: called when the emulator is closing down allowing the DLL to de-initialise
//==========================================================================
DLLEXPORT void CALL CloseDLL(void)
{
    MainDll::GetInstance()->End();
}
//==========================================================================
// Purpose: To process the raw data that has just been sent to a specific controller
// Input: Controller Number (0 to 3) and -1 signaling end of processing the pif ram. Pointer of data to be processed.
// Note: This function is only needed if the DLL is allowing raw data
// The data that is being processed looks like this
// initialize controller: 01 03 00 FF FF FF
// read controller:       01 04 01 FF FF FF FF
//==========================================================================
DLLEXPORT void CALL ControllerCommand(int Control, BYTE *Command)
{
}
//==========================================================================
// Purpose: Optional function that is provided to give further information about the DLL
// Input: A handle to the window that calls this function
//==========================================================================
DLLEXPORT void CALL DllAbout(HWND hParent)
{
    //std::string message = "JoyShockLibrary Plugin for 1964 (GE/PD) " << __GYRO_INJECTOR_VERSION__ << " (Build: "<< __DATE__ << ")\nCopyright (C) " << __CURRENTYEAR__ << ", Carnivorous, TauAkiou";
    //MessageBoxA(hParent, message.c_str() , "JoyShock Injector - About", MB_ICONINFORMATION | MB_OK);
}
//==========================================================================
// Purpose: Optional function that is provided to allow the user to configure the DLL
// Input: A handle to the window that calls this function
// Changed Globals: configdialogopen, mousetoggle, lastinputbutton, guibusy, windowactive
//==========================================================================
DLLEXPORT void CALL DllConfig(HWND hParent)
{
    if(JoyShockDriver::getInstance()->GetConnectedDeviceCount())
    {
        //MessageBoxA(hParent, "Controllers found.\n" , "JoyShock Injector - Controllers Found", MB_ICONERROR | MB_OK);

        //int laststate = mousetoggle;
        //configdialogopen = 1, mousetoggle = 0, lastinputbutton = 0, guibusy = 1;
        //DialogBox(hInst, MAKEINTRESOURCE(IDC_CONFIGWINDOW), hParent, (DLGPROC)GUI_Config);
        MainDll::GetInstance()->HandleConfigWindow(hParent);
        //UpdateControllerStatus();
        //configdialogopen = 0, windowactive = 1, guibusy = 1;
    }
    else
        MessageBoxA(hParent, "JoyShock Plugin did not detect any compatible controllers.\n\nPlease connect controllers and restart emulator." , "JoyShock Injector - Error", MB_ICONERROR | MB_OK);
}
//==========================================================================
// Purpose: Optional function that is provided to allow the user to test the DLL
// input: A handle to the window that calls this function
//==========================================================================
DLLEXPORT void CALL DllTest(HWND hParent)
{
    MessageBoxA(hParent, JoyShockDriver::getInstance()->GetConnectedDeviceCount() ? "Joyshock Injector detects a JoyShockLibrary compatible controller." : "Joyshock Injector could not find Mouse and Keyboard", "Mouse Injector - Testing", MB_ICONINFORMATION | MB_OK);
}
//==========================================================================
// Purpose: Allows the emulator to gather information about the DLL by filling in the PluginInfo structure
// Input: A pointer to a PLUGIN_INFO structure that needs to be filled by the function (see def above)
//==========================================================================
DLLEXPORT void CALL GetDllInfo(PLUGIN_INFO *PluginInfo)
{
    PluginInfo->Version = 0xFBAD; // no emulator supports this other than my disgusting version of 1964 (awful hack that i created because plugins are not complicated enough and i don't know what the f**k i am doing as evident from the code i've written)
    PluginInfo->Type = PLUGIN_TYPE_CONTROLLER;
    sprintf(PluginInfo->Name, "Joyshock for GE/PD (cpp) %s", __GYRO_INJECTOR_VERSION__);
#ifdef SPEEDRUN_BUILD
    sprintf(PluginInfo->Name, "%s (Speedrun Build)", PluginInfo->Name);
#endif
}

//==========================================================================
// Purpose: Get the current state of the controllers buttons
// Input: Controller Number (0 to 3) - A pointer to a BUTTONS structure to be filled with the controller state
//==========================================================================
DLLEXPORT void CALL GetKeys(int Control, BUTTONS *Keys)
{
    if(Keys == nullptr)
        return;

    Keys->Value = !MainDll::GetInstance()->IsConfigDialogOpen() ? Emulator::Controller[Control].Value : 0; // ignore input if config dialog is open
}
//==========================================================================
// Purpose: Initializes how each of the controllers should be handled
// Input: The handle to the main window - A controller structure that needs to be filled for the emulator to know how to handle each controller
// Changed Globals: ctrlptr, mousetoggle, PROFILE.SETTINGS
//==========================================================================
DLLEXPORT void CALL InitiateControllers(HWND hMainWindow, CONTROL Controls[4])
{
    if(!MainDll::GetInstance()->InitiateControllers(hMainWindow, Controls)) {
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
    MainDll::GetInstance()->EndInjection();
}
//==========================================================================
// Purpose: Called when a ROM is open (from the emulation thread)
// Changed Globals: emulatorwindow
//==========================================================================
DLLEXPORT void CALL RomOpen(void)
{
    JoyShockDriver::getInstance()->AssignEmulatorWindow(GetForegroundWindow());
    MainDll::GetInstance()->StartInjection();
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
    Game::GetInstance()->AssignRamPtr((const unsigned char **)Mem);
    MainDll::GetInstance()->SetEmulatorOverclock(OCFactor >= 3); // an overclock above 3 is guaranteed to be 60fps, so set to 0 if below 3 times overclock
//DRP_Update(); // init and update discord rich presence (discord will limit update rate to once every 15 seconds)
}
//==========================================================================
// Purpose: Give rom file pointer to the plugin on boot (for patching fov)
// Input: pointer to emulator's loaded rom
// Changed Globals: romptr
//==========================================================================
DLLEXPORT void CALL HookROM(DWORD *Rom)
{
    Game::GetInstance()->AssignRomPtr((const unsigned char **)Rom);
}