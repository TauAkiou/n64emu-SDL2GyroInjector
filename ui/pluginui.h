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



/*
 * pluginui.h - UI interface for WIN32 dialog window & settings configuration.
 */

#ifndef INC_1964_INPUT_JOYSHOCKCPP_PLUGINUI_H
#define INC_1964_INPUT_JOYSHOCKCPP_PLUGINUI_H

#include "../1964_plugin.h"
#include "resource.h"
#include "ui.rc"
#include "CallbackWrapper.h"

class PluginUi {
private:
    bool _configdialogisopen = false;
    bool _guibusy = false;

    void _init(const HWND hW);
    void _refresh(const HWND hW, const int revertbtn);
    static CALLBACK BOOL _config(HWND hW, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static BOOL _openconfigdialog(void* param);


public:
    PluginUi() = default;
    void OpenDialogWindow(HWND hW, HINSTANCE inst);
    bool GetConfigDialogState();
};


#endif //INC_1964_INPUT_JOYSHOCKCPP_PLUGINUI_H
