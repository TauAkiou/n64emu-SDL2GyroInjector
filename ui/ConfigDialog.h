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
#include <sstream>
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
    PROFILE _localprofiles[4]{};
    Assignment _localassignments[4]{};

    std::vector<JSDevice> _loadedfull;
    std::vector<JSDevice> _loadedjoyconprimary;
    std::vector<JSDevice> _loadedjoyconsecondary;

    static inline bool _configdialogisopen = false;
    static inline bool _guibusy = false;
    PLAYERS _selectedplayer;
    void _loadDevicesIntoDeviceBox(CONTROLLERMODE mode);
    void _loadMappingsIntoUi(PROFILE &profile, Assignment &asgn);


private slots:
    void on_cancelButton_clicked();
    void on_reconnectControllers_clicked();
    void on_playerSettingsTabGyroXAxisSensitivitySlider_valueChanged(int value);
    void on_playerSettingsTabGyroXAxisSensitivitySpinbox_valueChanged(double value);
    void on_playerSettingsTabGyroYAxisSensitivitySlider_valueChanged(int value);
    void on_playerSettingsTabGyroYAxisSensitivitySpinbox_valueChanged(double value);
    void on_controllerModeBox_currentIndexChanged(int index);
    void on_primaryDeviceBox_currentIndexChanged(int index);
    void on_secondaryDeviceBox_currentIndexChanged(int index);

public:
    ~ConfigDialog() override;
    explicit ConfigDialog(QDialog *parent = 0);
    static bool GetConfigDialogState();


    void _selectDeviceFromAssignment();

    void _getCurrentConfigState();
};


#endif //INC_1964_INPUT_JOYSHOCKCPP_CONFIGDIALOG_H
