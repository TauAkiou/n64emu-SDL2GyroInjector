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

#include "pluginui.h"

bool PluginUi::GetConfigDialogState() {
    return _configdialogisopen;
}

void PluginUi::_init(const HWND hW) {
    // Dialog setup goes here.
    return;
}

void PluginUi::_refresh(const HWND hW, const int revertbtn) {
    _guibusy = false;
}

void PluginUi::OpenDialogWindow(HWND hW, HINSTANCE hinstance) {
    _configdialogisopen = true;
    _guibusy = true;
    DialogBox(hinstance, MAKEINTRESOURCE(IDC_PLUGINCONFIG), hW, (DLGPROC)&this->_config );
    _configdialogisopen = false;
}

BOOL CALLBACK PluginUi::_config(HWND hW, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch(uMsg) {
        case WM_INITDIALOG:
            _init(hW);
            _refresh(hW, 0);
        case WM_COMMAND: {
            if (_guibusy)
                break;
            switch(LOWORD(wParam))
            {
                case IDC_PLUGINCONFIG:
                    break;
                case IDC_PUSHBUTTON:
                    EndDialog(hW, FALSE);
                    return true;
            }
        }
        case WM_CLOSE:
        case WM_DESTROY:
            EndDialog(hW, FALSE);
            return true;
        default:
            break;
    }
    return FALSE;
}

