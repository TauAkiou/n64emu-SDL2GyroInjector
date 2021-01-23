#include "maindll.h"

MainDll* MainDll::instance = nullptr;

MainDll * MainDll::GetInstance(HINSTANCE hinstance) {
    if(!MainDll::instance) {
        MainDll::instance = new MainDll(hinstance);
    }
    return MainDll::instance;
}

bool MainDll::IsConfigDialogOpen() const {
    return _configdialogisopen;
}

MainDll::MainDll(HINSTANCE hinstance) {
    // Start by initialzing all pointers.
    _settingsptr = Settings::GetInstance();
    _emuctrlptr = ControlState::GetInstance();
    _jsdptr = JoyShockDriver::getInstance();
    _gameptr = Game::GetInstance();
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
    _ctrlptr = nullptr;
}

void MainDll::SetEmulatorOverclock(bool newoverclock) {
    _emuoverclock = newoverclock;
}

bool MainDll::InitiateControllers(HWND window, CONTROL *ptr) {
    _ctrlptr = ptr;

    if(!Initialize(window)) {
        for(int player = PLAYER1; player < ALLPLAYERS; player++) {
            _settingsptr->Profile[player].QuickConfigSetting = DISABLED;
        }
        UpdateControllerStatus();
        return false;
    }
        // Set up configuration objects.


    // Use the first detected FULL controller type.
    _settingsptr->Profile[PLAYER1].QuickConfigSetting = DEFAULT;

    auto ctrllst_full = _jsdptr->GetConnectedFullControllers();
    auto ctrllst_ljc = _jsdptr->GetConnectedLeftJoycons();
    auto ctrllst_rjc = _jsdptr->GetConnectedRightJoycons();

    /*
    if(ctrllist.empty()) {
        // No controller, just in case here
        _settingsptr->Profile[PLAYER1].SETTINGS[CONFIG] = DISABLED;
        return false;
    }
     */



    // Get the first controller for testing.
    _settingsptr->Profile[PLAYER1].ControllerMode = 0;
    _settingsptr->Profile[PLAYER1].AssignedDevicePrimary = ctrllst_full.front();
    //_settingsptr->Profile[PLAYER1].AssignedDeviceSecondary = ctrllst_rjc.front();
    //_settingsptr->Profile[PLAYER1].AssignedDevicePrimary = ctrllst_ljc.front();

    _settingsptr->Profile[PLAYER1].FreeAiming = true;
    _settingsptr->Profile[PLAYER1].StickMode = FLICK;
    _settingsptr->Profile[PLAYER1].UseStickToAim = false;
    _settingsptr->Profile[PLAYER1].DS4Color = 0x0000FF;

    _settingsptr->Profile[PLAYER1].AimStickSensitivity = {23000, 23000};
    _settingsptr->Profile[PLAYER1].GyroscopeSensitivity = {400, 400};
    _settingsptr->Profile[PLAYER1].AimstickDeadzone = {0.10, 0.10};
    
    _settingsptr->Profile[PLAYER1].Crosshair = 1;
    _settingsptr->Profile[PLAYER1].PitchInverted = false;
    _settingsptr->Profile[PLAYER1].CrouchToggle = true;
    _settingsptr->Profile[PLAYER1].GoldeneyeAimMode = true;
    _settingsptr->Profile[PLAYER1].PerfectDarkAimMode = true;

    _settingsptr->Profile[PLAYER1].BUTTONPRIM[FIRE] = JSMASK_ZR;
    _settingsptr->Profile[PLAYER1].BUTTONPRIM[AIM] = JSMASK_ZL;
    _settingsptr->Profile[PLAYER1].BUTTONPRIM[ACCEPT] = JSMASK_L;
    _settingsptr->Profile[PLAYER1].BUTTONPRIM[CANCEL] = JSMASK_R;
    _settingsptr->Profile[PLAYER1].BUTTONPRIM[START] = JSMASK_PLUS;
    _settingsptr->Profile[PLAYER1].BUTTONPRIM[CROUCH] = JSMASK_E;
    _settingsptr->Profile[PLAYER1].BUTTONPRIM[KNEEL] = JSMASK_S;
    _settingsptr->Profile[PLAYER1].BUTTONPRIM[PREVIOUSWEAPON] = JSMASK_W;
    _settingsptr->Profile[PLAYER1].BUTTONPRIM[NEXTWEAPON] = JSMASK_N;
    _settingsptr->Profile[PLAYER1].BUTTONPRIM[UP] = JSMASK_UP;
    _settingsptr->Profile[PLAYER1].BUTTONPRIM[DOWN] = JSMASK_DOWN;
    _settingsptr->Profile[PLAYER1].BUTTONPRIM[LEFT]= JSMASK_LEFT;
    _settingsptr->Profile[PLAYER1].BUTTONPRIM[RIGHT] = JSMASK_RIGHT;
    _settingsptr->Profile[PLAYER1].BUTTONPRIM[RESETGYRO] = JSMASK_MINUS;
    _settingsptr->Profile[PLAYER1].CalibrationButton = JSMASK_CAPTURE;

    // Use the first detected FULL controller type.
    _settingsptr->Profile[PLAYER2].QuickConfigSetting = DISABLED;
    _settingsptr->Profile[PLAYER3].QuickConfigSetting = DISABLED;
    _settingsptr->Profile[PLAYER4].QuickConfigSetting = DISABLED;

    _settingsptr->ShowGoldeneyeCrosshair = true;

    UpdateControllerStatus();
    return true;
}

void MainDll::UpdateControllerStatus() {
    if(_ctrlptr == nullptr)
        return;

    for(int player = PLAYER1; player < ALLPLAYERS; player++) {
        _ctrlptr[player].Present = _settingsptr->Profile[player].QuickConfigSetting;
        _ctrlptr[player].RawData = false;
        _ctrlptr[player].Plugin = player == PLAYER1 ? PLUGIN_MEMPACK : PLUGIN_NONE;
    }
}

