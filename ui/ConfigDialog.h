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

#ifndef INC_1964_INPUT_JOYSHOCKCPP_CONFIGDIALOG_H
#define INC_1964_INPUT_JOYSHOCKCPP_CONFIGDIALOG_H

#include <QtWidgets>
#include "../settings/Settings.h"
#include "ui_1964_config.h"
#include "../input/JoyShockDriver.h"

namespace Ui {
    class ConfigDialog;
}

class ConfigDialog : public QDialog {
    Q_OBJECT
private:
    Ui::ConfigDialog* _configform;
    Settings* _settingsptr = Settings::GetInstance();
    JoyShockDriver* _jsdriver = JoyShockDriver::getInstance();
    PROFILE _localprofiles[4];
    Assignment _localassignments[4];

    std::vector<JSDevice> _loadedfull;
    std::vector<JSDevice> _loadedjoyconprimary;
    std::vector<JSDevice> _loadedjoyconsecondary;

    static inline bool _configdialogisopen = false;
    static inline bool _guibusy = false;
    void _loadDevicesIntoDeviceBox(JSD_ControllerType type);
    static BOOL _openconfigdialog(void* param);
    static void _init();
    static void _refresh();
    static BOOL _config();

private slots:
    void on_cancelButton_clicked();
    void on_playerSettingsTabGyroXAxisSensitivitySlider_valueChanged(int value);
    void on_playerSettingsTabGyroXAxisSensitivitySpinbox_valueChanged(double value);
    void on_playerSettingsTabGyroYAxisSensitivitySlider_valueChanged(int value);
    void on_playerSettingsTabGyroYAxisSensitivitySpinbox_valueChanged(double value);



public:
    ~ConfigDialog() override;
    explicit ConfigDialog(QDialog *parent = 0);
    static bool GetConfigDialogState();


    void _loadProfileIntoUI(PROFILE &profile);

    void _loadProfileIntoUI(PROFILE &profile, Assignment &asgn);
};


#endif //INC_1964_INPUT_JOYSHOCKCPP_CONFIGDIALOG_H
