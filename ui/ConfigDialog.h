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
#include <memory>
#include <future>
#include "../settings/Settings.h"
#include "../input/SdlDriver.h"
#include "ui_1964_config.h"



namespace Ui {
    class ConfigDialog;
}

class ConfigDialog : public QDialog {
    Q_OBJECT
private:
    const QString _cnststr_fov_locked = "FOV controls are disabled while a ROM is running.<br>Stop the ROM to change this setting.";

    Ui::ConfigDialog* _baseDialog;

    QList<QPushButton*> _mappingButtonListPrimary;
    QList<QPushButton*> _mappingButtonListSecondary;

    Settings* _settingsptr = Settings::GetInstance();
    SdlDriver* _jsdriver = SdlDriver::getInstance();
    js_settings::PROFILE _localprofiles[4]{};
    Assignment _localassignments[4]{};

    bool _ge_crosshair;
    int _aspect_x;
    int _aspect_y;
    int fov;

    int _deviceindex[2][4];

    std::vector<std::shared_ptr<SDLDevice>> _loadedfull;
    std::vector<SDLDevice> _loadedjoyconprimary;
    std::vector<SDLDevice> _loadedjoyconsecondary;

    bool _locked = false;
    static inline bool _configdialogisopen = false;
    static inline bool _guibusy = false;
    PLAYERS _selectedplayer;
    void _loadDevicesIntoDeviceBox(CONTROLLERMODE mode);
    void _loadMappingsIntoUi(js_settings::PROFILE &profile, Assignment &asgn);
    void _mapButtonToCommand(CONTROLLERENUM command, bool isSecondary);
    void _getCurrentConfigState();
    static QString _getNameFromButtonIndex(CONTROLLERENUM index);
    void _createPrimaryButtonLayouts();
    void _commitAssignments();
    void _commitProfiles();
    void _commitGlobals();
    void _loadProfileSettingsIntoUi(const js_settings::PROFILE profile);
    static Color _getColorFromInt(int color);
    int _getIntFromColor(Color color);
    void _setPlayerColorAndDefaultNumber(const js_settings::PROFILE& prf, Assignment asgn);

signals:
    void primaryClicked(int value);
    void secondaryClicked(int value);

private slots:
    void _processPrimaryLayout(int value);
    void _processSecondaryLayout(int value);

    void on_cancelButton_clicked();
    void on_okButton_clicked();
    void on_reconnectControllers_clicked();
    void on_playerSettingsTabGyroLocalSpaceYAxisBox_activated(int index);
    void on_playerSettingsTabGyroGyroSpaceBox_activated(int index);


    void on_playerSettingsTabGyroXAxisSensitivitySlider_sliderMoved(int value);
    void on_playerSettingsTabGyroXAxisSensitivitySpinbox_valueChanged(double value);
    void on_playerSettingsTabGyroYAxisSensitivitySlider_sliderMoved(int value);
    void on_playerSettingsTabGyroYAxisSensitivitySpinbox_valueChanged(double value);
    void on_playerSettingsTabStickAimSensitivityXSlider_sliderMoved(int value);
    void on_playerSettingsTabStickAimSensitivityXSpinbox_valueChanged(double value);
    void on_playerSettingsTabStickAimSensitivityYSlider_sliderMoved(int value);
    void on_playerSettingsTabStickAimSensitivityYSpinbox_valueChanged(double value);
    void on_playerSettingsTabStickAimDeadzoneXSlider_sliderMoved(int value);
    void on_playerSettingsTabStickAimDeadzoneYSpinbox_valueChanged(double value);
    void on_playerSettingsTabStickAimDeadzoneYSlider_sliderMoved(int value);
    void on_playerSettingsTabStickAimDeadzoneXSpinbox_valueChanged(double value);
    void on_playerSettingsTabStickMoveDeadzoneXSlider_sliderMoved(int value);
    void on_playerSettingsTabStickMoveDeadzoneXSpinbox_valueChanged(double value);
    void on_playerSettingsTabStickMoveDeadzoneYSlider_sliderMoved(int value);
    void on_playerSettingsTabStickMoveDeadzoneYSpinbox_valueChanged(double value);
    void on_controllerModeBox_activated(int index);
    void on_primaryDeviceBox_activated(int index);
    void on_secondaryDeviceBox_activated(int index);
    void on_globalFovSpinbox_valueChanged(int value);
    void on_globalFovSlider_valueChanged(int value);
    void on_playerSettingsTabGyroAimingStyleBox_activated(int index);
    void on_playerSelectionButtonGroup_buttonClicked(QAbstractButton* button);
    void on_playerSettingsTabStickAimStickModeBox_activated(int index);
    void on_playerSettingsTabStickInvertAimPitchCheckbox_stateChanged(int state);
    void on_playerSettingsTabGyroInvertCheckbox_stateChanged(int state);
    void on_playerSettingsTabOtherCursorAimingGoldeneyeCheckbox_stateChanged(int state);
    void on_playerSettingsTabOtherCursorAimingPerfectDarkCheckbox_stateChanged(int state);
    void on_playerSettingsTabOtherAllowAimingUsingStickCheckbox_stateChanged(int state);
    void on_playerSettingsTabOtherDS4SpinboxRed_valueChanged(int value);
    void on_playerSettingsTabOtherDS4SpinboxGreen_valueChanged(int value);
    void on_playerSettingsTabOtherDS4SpinboxBlue_valueChanged(int value);
    void on_playerSettingsTabGyroUseSeperateAimSensCheckbox_stateChanged(int state);
    void on_playerSettingsTabGyroXAxisSensitivityAimSpinbox_valueChanged(double value);
    void on_playerSettingsTabGyroYAxisSensitivityAimSpinbox_valueChanged(double value);
    void on_playerSettingsTabGyroXAxisSensitivityAimSlider_sliderMoved(int value);
    void on_playerSettingsTabGyroYAxisSensitivityAimSlider_sliderMoved(int value);
    void on_playerSettingsTabOtherTriggerThresholdL_valueChanged(double value);
    void on_playerSettingsTabOtherTriggerThresholdR_valueChanged(double value);







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
