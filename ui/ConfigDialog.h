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
#include <iostream>
#include <chrono>
#include <future>
#include "../settings/Settings.h"
#include "ui_1964_config.h"
#include "../input/JoyShockDriver.h"

namespace Ui {
    class ConfigDialog;
}

class ConfigDialog : public QDialog {
    Q_OBJECT
private:
    Ui::ConfigDialog* _baseDialog;

    QList<QPushButton*> _mappingButtonListPrimary;
    QList<QPushButton*> _mappingButtonListSecondary;

    Settings* _settingsptr = Settings::GetInstance();
    JoyShockDriver* _jsdriver = JoyShockDriver::getInstance();
    PROFILE _localprofiles[4]{};
    Assignment _localassignments[4]{};

    std::vector<JSDevice> _loadedfull;
    std::vector<JSDevice> _loadedjoyconprimary;
    std::vector<JSDevice> _loadedjoyconsecondary;

    bool _locked = false;
    static inline bool _configdialogisopen = false;
    static inline bool _guibusy = false;
    PLAYERS _selectedplayer;
    void _loadDevicesIntoDeviceBox(CONTROLLERMODE mode);
    void _loadMappingsIntoUi(PROFILE &profile, Assignment &asgn);
    void _mapButtonToCommand(CONTROLLERENUM command, bool isSecondary);
    void _selectDeviceFromAssignment();
    void _getCurrentConfigState();
    static QString _getNameFromButtonIndex(CONTROLLERENUM index);
    void _createPrimaryButtonLayouts();
    void _commitAssignments();
    void _commitProfiles();
    void _commitGlobals();
    void _loadProfileSettingsIntoUi(PROFILE profile);
    static Color _getColorFromInt(int color);
    int _getIntFromColor(Color color);
    void _setPlayerColorAndDefaultNumber(PROFILE prf, Assignment asgn);

signals:
    void primaryClicked(int value);
    void secondaryClicked(int value);

private slots:
    void _processPrimaryLayout(int value);
    void _processSecondaryLayout(int value);

    void on_cancelButton_clicked();
    void on_okButton_clicked();
    void on_reconnectControllers_clicked();
    void on_playerSettingsTabGyroXAxisSensitivitySlider_valueChanged(int value);
    void on_playerSettingsTabGyroXAxisSensitivitySpinbox_valueChanged(double value);
    void on_playerSettingsTabGyroYAxisSensitivitySlider_valueChanged(int value);
    void on_playerSettingsTabGyroYAxisSensitivitySpinbox_valueChanged(double value);
    void on_playerSettingsTabStickAimSensitivityXSlider_valueChanged(int value);
    void on_playerSettingsTabStickAimSensitivityXSpinbox_valueChanged(double value);
    void on_playerSettingsTabStickAimSensitivityYSlider_valueChanged(int value);
    void on_playerSettingsTabStickAimSensitivityYSpinbox_valueChanged(double value);
    void on_playerSettingsTabStickAimDeadzoneXSlider_valueChanged(int value);
    void on_playerSettingsTabStickAimDeadzoneYSpinbox_valueChanged(double value);
    void on_playerSettingsTabStickAimDeadzoneYSlider_valueChanged(int value);
    void on_playerSettingsTabStickAimDeadzoneXSpinbox_valueChanged(double value);
    void on_playerSettingsTabStickMoveDeadzoneXSlider_valueChanged(int value);
    void on_playerSettingsTabStickMoveDeadzoneXSpinbox_valueChanged(double value);
    void on_playerSettingsTabStickMoveDeadzoneYSlider_valueChanged(int value);
    void on_playerSettingsTabStickMoveDeadzoneYSpinbox_valueChanged(double value);
    void on_controllerModeBox_currentIndexChanged(int index);
    void on_primaryDeviceBox_currentIndexChanged(int index);
    void on_secondaryDeviceBox_currentIndexChanged(int index);
    void on_globalFovSpinbox_valueChanged(int value);
    void on_globalFovSlider_valueChanged(int value);
    void on_playerSettingsTabGyroAimingStyleBox(int index);
    void on_playerSelectionButtonGroup_buttonClicked(QAbstractButton* button);


    // Why can't qt designer have an easy way of assigning large groups of buttons in a way that is straightforward and works...
    // unlike the following...
    //
    // - Subclassing, which causes issues in the editor & extra programming overhead
    // - ID Extraction, which turns any edits to the button chain into a tiresome manual nightmare of adjusting numbers
    // - ButtonGroups, which have no way of assigning the ID or relative order in designer, because reasons?
    // - Manually creating 44(!) on_<whatever>_clicked() functions and having them call a function
    // - Dynamically creating the buttons and linking them myself, which defeats the point of the designer

public:
    ~ConfigDialog() override;
    explicit ConfigDialog(QDialog *parent = 0);
    static bool GetConfigDialogState();



};


#endif //INC_1964_INPUT_JOYSHOCKCPP_CONFIGDIALOG_H
