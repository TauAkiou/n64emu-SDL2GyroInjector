#include "maindll.h"


MainDll* MainDll::instance = nullptr;

MainDll * MainDll::getInstance(HINSTANCE hinstance) {
    if(!MainDll::instance) {
        MainDll::instance = new MainDll(hinstance);
    }
    return MainDll::instance;
}

bool MainDll::IsConfigDialogOpen() const {
    return _configdialogisopen;
}

MainDll::MainDll(HINSTANCE hinstance) {
    // As part of the instantiation, also get object pointers.
    _emuctrlptr = Controls::getInstance();
    _jsdptr = JoyShockDriver::getInstance();
    _gameptr = Game::getInstance();
    _hinst = hinstance;

    wchar_t filepath[MAX_PATH] = {L'\0'};
    wchar_t directory[MAX_PATH] = {L'\0'};

    GetModuleFileNameW(_hinst, filepath, MAX_PATH);
    if (filepath != nullptr) {
        const wchar_t slash[] = L"\\";
        wchar_t *dllname;
        unsigned int dllnamelength = 19;
        dllname = wcspbrk(filepath, slash);
        while (dllname !=
               nullptr) { // find the last filename in full filepath and set filename length to dllnamelength (skip to slash every loop until last filename is found)
            dllnamelength = wcslen(dllname);
            dllname = wcspbrk(dllname + 1, slash);
        }
        wcsncpy(directory, filepath, wcslen(filepath) - dllnamelength + 1);
        directory[wcslen(filepath) - dllnamelength + 1] = L'\0'; // string needs terminator so add zero character to end
        _jsonfilepath.append(directory);
        _jsonfilepath.append(L"mouseinjector.json");
    }
}

bool MainDll::Initialize(const HWND hW) {
    _jsdptr->Initialize(hW);
    if(!_jsdptr->GetConnectedDeviceCount())
        return false;
    return true;
}

void MainDll::StartInjection() {
    _jsdptr->StartInjectionThread();
}

void MainDll::EndInjection() {
    _jsdptr->EndInjectionThread();
}

void MainDll::End() {
    _configdialogisopen = false;
    _rdramptr = nullptr;
    _romptr = nullptr;
    _ctrlptr = nullptr;
}

void MainDll::SetEmulatorOverclock(bool newoverclock) {
    _emuoverclock = newoverclock;
}

bool MainDll::InitiateControllers(HWND window, CONTROL *ptr) {
    _ctrlptr = ptr;

    if(!Initialize(window)) {
        for(int player = PLAYER1; player < ALLPLAYERS; player++) {
            _emuctrlptr->Profile[player].SETTINGS[CONFIG] = DISABLED;
        }
        UpdateControllerStatus();
        return false;
    }
        // Set up configuration objects.


    // Use the first detected FULL controller type.
    _emuctrlptr->Profile[PLAYER1].SETTINGS[CONFIG] = DEFAULT;

    auto ctrllist = _jsdptr->GetConnectedFullControllers();

    if(ctrllist.empty()) {
        // No controller, just in case here
        _emuctrlptr->Profile[PLAYER1].SETTINGS[CONFIG] = DISABLED;
        return false;
    }

    // Get the first controller for testing.
    _emuctrlptr->Profile->AssignedDevicePrimary = ctrllist.front();

    _emuctrlptr->Profile[PLAYER1].SETTINGS[STICKSENSITIVITYX] = 250;
    _emuctrlptr->Profile[PLAYER1].SETTINGS[STICKSENSITIVITYY] = 250;

    _emuctrlptr->Profile[PLAYER1].SETTINGS[GYROSENSITIVITYX] = 1000;
    _emuctrlptr->Profile[PLAYER1].SETTINGS[GYROSENSITIVITYY] = 1000;

    _emuctrlptr->Profile[PLAYER1].SETTINGS[CROSSHAIR] = 1;
    _emuctrlptr->Profile[PLAYER1].SETTINGS[INVERTPITCH] = 0;
    _emuctrlptr->Profile[PLAYER1].SETTINGS[CROUCHTOGGLE] = 1;
    _emuctrlptr->Profile[PLAYER1].SETTINGS[GEAIMMODE] = 1;
    _emuctrlptr->Profile[PLAYER1].SETTINGS[CROSSHAIR] = 1;
    _emuctrlptr->Profile[PLAYER1].SETTINGS[PDAIMMODE] = 1;
    _emuctrlptr->Profile[PLAYER1].SETTINGS[CONMODE] = 1;
    _emuctrlptr->Profile[PLAYER1].SETTINGS[COLOR] = 0xFF0000;

    _emuctrlptr->Profile[PLAYER1].BUTTONPRIM[FIRE].Button = JSMASK_ZR;
    _emuctrlptr->Profile[PLAYER1].BUTTONPRIM[AIM].Button = JSMASK_ZL;
    _emuctrlptr->Profile[PLAYER1].BUTTONPRIM[ACCEPT].Button = JSMASK_L;
    _emuctrlptr->Profile[PLAYER1].BUTTONPRIM[CANCEL].Button = JSMASK_R;
    _emuctrlptr->Profile[PLAYER1].BUTTONPRIM[START].Button = JSMASK_PLUS;
    _emuctrlptr->Profile[PLAYER1].BUTTONPRIM[CROUCH].Button = JSMASK_E;
    _emuctrlptr->Profile[PLAYER1].BUTTONPRIM[KNEEL].Button = JSMASK_S;
    _emuctrlptr->Profile[PLAYER1].BUTTONPRIM[PREVIOUSWEAPON].Button = JSMASK_W;
    _emuctrlptr->Profile[PLAYER1].BUTTONPRIM[NEXTWEAPON].Button = JSMASK_N;
    _emuctrlptr->Profile[PLAYER1].BUTTONPRIM[UP].Button = JSMASK_UP;
    _emuctrlptr->Profile[PLAYER1].BUTTONPRIM[DOWN].Button = JSMASK_DOWN;
    _emuctrlptr->Profile[PLAYER1].BUTTONPRIM[LEFT].Button= JSMASK_LEFT;
    _emuctrlptr->Profile[PLAYER1].BUTTONPRIM[RIGHT].Button = JSMASK_RIGHT;
    _emuctrlptr->Profile[PLAYER1].BUTTONPRIM[RESETGYRO].Button = JSMASK_MINUS;

    _jsdptr->CalibrateGyroscope(_emuctrlptr->Profile[PLAYER1].AssignedDeviceSecondary);

    UpdateControllerStatus();
    return true;
}

void MainDll::UpdateControllerStatus() {
    if(_ctrlptr == nullptr)
        return;

    for(int player = PLAYER1; player < ALLPLAYERS; player++) {
        _ctrlptr[player].Present = _emuctrlptr->Profile->SETTINGS[CONFIG];
        _ctrlptr[player].RawData = false;
        _ctrlptr[player].Plugin = player == PLAYER1 ? PLUGIN_MEMPACK : PLUGIN_NONE;
    }
}

/* N64 Emulator Program Connection Methods */

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch(fdwReason)
    {
        case DLL_PROCESS_ATTACH:
        {
            // Initialize all state objects.
            MainDll::getInstance(hinstDLL);
            AllocConsole(); AttachConsole(GetCurrentProcessId()); freopen("CON", "w", stdout );
            std::cout << "test\n";
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
    MainDll::getInstance()->End();
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
{    //auto message = "JoyShockLibrary Plugin for 1964 (GE/PD) "__GYRO_INJECTOR_VERSION__" (Build: "__DATE__")\nCopyright (C) "__CURRENTYEAR__", Carnivorous, TauAkiou";
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
        MessageBoxA(hParent, "Controllers found.\n" , "JoyShock Injector - Controllers Found", MB_ICONERROR | MB_OK);

        //int laststate = mousetoggle;
        //configdialogopen = 1, mousetoggle = 0, lastinputbutton = 0, guibusy = 1;
        //DialogBox(hInst, MAKEINTRESOURCE(IDC_CONFIGWINDOW), hParent, (DLGPROC)GUI_Config);
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

    //auto cons = Controls::getInstance();
    //std::cout << "Controller: " << Control << " Submitting " <<  Emulator::Controller[Control].Value << "\n";
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