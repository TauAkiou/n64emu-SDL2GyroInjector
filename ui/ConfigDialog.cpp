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

#include "ConfigDialog.h"

/*
 * ConfigDialog::ConfigDialog(QDialog *parent) : QDialog(parent), _baseDialog(new Ui::ConfigDialog)
 *
 * Constructor for configuration dialog.
 *
 */
ConfigDialog::ConfigDialog(QDialog *parent) : QDialog(parent), _baseDialog(new Ui::ConfigDialog) {
    _baseDialog->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    _baseDialog->playerSelectionButtonGroup->addButton(_baseDialog->player1Radio, 0);
    _baseDialog->playerSelectionButtonGroup->addButton(_baseDialog->player2Radio, 1);
    _baseDialog->playerSelectionButtonGroup->addButton(_baseDialog->player3Radio, 2);
    _baseDialog->playerSelectionButtonGroup->addButton(_baseDialog->player4Radio, 3);

    _getCurrentConfigState();

    // Dynamically generate all mapper buttons and assign them to the ScrollBox
    auto *mapperPrimary = new QSignalMapper(this);
    auto *mapperSecondary = new QSignalMapper(this);
    connect(mapperPrimary, SIGNAL(mapped(int)), this, SIGNAL(primaryClicked(int)));
    connect(mapperSecondary, SIGNAL(mapped(int)), this, SIGNAL(secondaryClicked(int)));

    for(int index = 0; index < TOTALBUTTONS; index++) {
        auto primaryButton = new QPushButton(this);
        auto secondaryButton = new QPushButton(this);
        primaryButton->setText(QString::fromStdString("None"));
        secondaryButton->setText(QString::fromStdString("None"));
        _mappingButtonListPrimary.append(primaryButton);
        _mappingButtonListSecondary.append(secondaryButton);
        mapperPrimary->setMapping(primaryButton, index);
        mapperSecondary->setMapping(secondaryButton, index);

        connect(primaryButton, SIGNAL(clicked()), mapperPrimary, SLOT(map()));
        connect(secondaryButton, SIGNAL(clicked()), mapperSecondary, SLOT(map()));

    }

    connect(this, SIGNAL(primaryClicked(int)), this, SLOT(_processPrimaryLayout(int)));
    connect(this, SIGNAL(secondaryClicked(int)), this, SLOT(_processSecondaryLayout(int)));

    _createPrimaryButtonLayouts();

    _loadedfull = _jsdriver->GetConnectedFullControllers();
    _loadedjoyconprimary = _jsdriver->GetConnectedLeftJoycons();
    _loadedjoyconsecondary = _jsdriver->GetConnectedRightJoycons();
    _selectedplayer = PLAYER1;

    _baseDialog->controllerModeBox->setCurrentIndex(_localassignments[_selectedplayer].ControllerMode);

    _loadProfileSettingsIntoUi(_localprofiles[_selectedplayer]);
    _loadDevicesIntoDeviceBox(_localassignments[_selectedplayer].ControllerMode);
    _loadMappingsIntoUi(_localprofiles[_selectedplayer], _localassignments[_selectedplayer]);
}

void ConfigDialog::_setPlayerColorAndDefaultNumber(PROFILE prf, Assignment asgn) {
    switch(asgn.ControllerMode) {
        default:
        case DISCONNECTED:
            return;
        case JOYCONS:
            // JSL is smart enough to ignore a line for controllers that don't have a particular feature.
            _jsdriver->SetDS4Color(asgn.SecondaryDevice, 15);
        case FULLCONTROLLER:
            _jsdriver->SetSPCJCNumber(asgn.PrimaryDevice, 15);
            _jsdriver->SetDS4Color(asgn.PrimaryDevice, prf.DS4Color);
            return;
    }
}

void ConfigDialog::_getCurrentConfigState() {
    // Set up our profiles and global states here.
    for(int player = PLAYER1; player < ALLPLAYERS; player++) {
        _localprofiles[player] = _settingsptr->GetProfileForPlayer(static_cast<PLAYERS>(player));
        _localassignments[player] = _settingsptr->GetAssignmentForPlayer(static_cast<PLAYERS>(player));
    }
    // I hate ternaries, but this is a place where they are actually quite useful.
    _baseDialog->globalGoldeneyeCrosshairCheckbox->setCheckState
    (_settingsptr->GetShowGoldeneyeCrosshair() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    auto gratio = _settingsptr->GetOverrideRatio();
    _baseDialog->globalRatioHorizontalSpinbox->setValue(gratio.x);
    _baseDialog->globalRatioVerticalSpinbox->setValue(gratio.y);
    _baseDialog->globalFovSlider->setValue(_settingsptr->GetFovOverride());
}

Color ConfigDialog::_getColorFromInt(int color) {
    // https://math.stackexchange.com/questions/1635999/algorithm-to-convert-integer-to-3-variables-rgb
    Color ret;
    ret.r = color / 0x10000;
    ret.g = (color / 0x100);
    ret.b = color % 0x100;
    return ret;
}

int ConfigDialog::_getIntFromColor(Color color) {
    https://stackoverflow.com/questions/4801366/convert-rgb-values-to-integer
    int rgb = color.r;
    rgb = (rgb << 8) + color.g;
    return (rgb << 8) + color.b;
}

void ConfigDialog::_loadProfileSettingsIntoUi(PROFILE profile) {
    //
    _baseDialog->playerSettingsTabStickAimStickLayoutBox->setCurrentIndex(profile.AimStick);
    _baseDialog->playerSettingsTabStickAimStickModeBox->setCurrentIndex(profile.StickMode);
    _baseDialog->playerSettingsTabOtherReverseGyroPitchCheckbox->setChecked(profile.GyroPitchInverted);
    _baseDialog->playerSettingsTabOtherReverseAimStickPitchCheckbox->setChecked(profile.StickPitchInverted);
    _baseDialog->playerSettingsTabOtherCursorAimingPDCheckbox->setChecked(profile.PerfectDarkAimMode);
    _baseDialog->playerSettingsTabOtherCursorAimingGeCheckbox->setChecked(profile.GoldeneyeAimMode);

    auto ds4color = _getColorFromInt(profile.DS4Color);
    _baseDialog->playerrSettingsTabOtherDS4RedSpinbox->setValue(ds4color.r);
    _baseDialog->playerrSettingsTabOtherDS4GreenSpinbox->setValue(ds4color.g);
    _baseDialog->playerrSettingsTabOtherDS4BlueSpinbox->setValue(ds4color.b);

    //auto colorrole = _baseDialog->colorwidget.

    _baseDialog->playerSettingsTabGyroXAxisSensitivitySpinbox->setValue(profile.AimStickSensitivity.x);
    _baseDialog->playerSettingsTabGyroXAxisSensitivitySlider->setValue(profile.AimStickSensitivity.x * 100);

    _baseDialog->playerSettingsTabGyroYAxisSensitivitySpinbox->setValue(profile.GyroscopeSensitivity.y);
    _baseDialog->playerSettingsTabGyroYAxisSensitivitySlider->setValue(profile.GyroscopeSensitivity.y * 100);

    _baseDialog->playerSettingsTabStickAimSensitivityXSpinbox->setValue(profile.AimStickSensitivity.x);
    _baseDialog->playerSettingsTabStickAimSensitivityXSlider->setValue(profile.AimStickSensitivity.x * 100);

    _baseDialog->playerSettingsTabStickAimSensitivityYSpinbox->setValue(profile.AimStickSensitivity.y);
    _baseDialog->playerSettingsTabStickAimSensitivityYSlider->setValue(profile.AimStickSensitivity.y * 100);

    _baseDialog->playerSettingsTabStickAimDeadzoneXSpinbox->setValue(profile.AimstickDeadzone.x);
    _baseDialog->playerSettingsTabStickAimDeadzoneXSlider->setValue(profile.AimstickDeadzone.x * 100);

    _baseDialog->playerSettingsTabStickAimDeadzoneYSpinbox->setValue(profile.AimstickDeadzone.y);
    _baseDialog->playerSettingsTabStickAimDeadzoneYSlider->setValue(profile.AimstickDeadzone.y * 100);

    _baseDialog->playerSettingsTabStickMoveDeadzoneXSpinbox->setValue(profile.MoveStickDeadzone.x);
    _baseDialog->playerSettingsTabStickMoveDeadzoneXSlider->setValue(profile.MoveStickDeadzone.x * 100);

    _baseDialog->playerSettingsTabStickMoveDeadzoneYSpinbox->setValue(profile.MoveStickDeadzone.y);
    _baseDialog->playerSettingsTabStickMoveDeadzoneYSlider->setValue(profile.MoveStickDeadzone.y * 100);

}

void ConfigDialog::_processPrimaryLayout(int value) {
    if (!_locked) {
        _locked = true;
        _mapButtonToCommand(static_cast<CONTROLLERENUM>(value), false);
        _locked = false;
    }
}

void ConfigDialog::_processSecondaryLayout(int value) {
    if (!_locked) {
        _locked = true;
        _mapButtonToCommand(static_cast<CONTROLLERENUM>(value), true);
        _locked = false;
    }

}

void ConfigDialog::_createPrimaryButtonLayouts() {
    auto vlayout = new QVBoxLayout(_baseDialog->scrollAreaWidgetContents);
    _baseDialog->scrollAreaWidgetContents->setLayout(vlayout);

    for(int entry = 0; entry < TOTALBUTTONS; entry++) {
        auto layout = new QHBoxLayout();
        layout->setSpacing(6);
        auto label = new QLabel(_getNameFromButtonIndex(static_cast<CONTROLLERENUM>(entry)));
        auto sizepolicy = QSizePolicy();
        sizepolicy.setVerticalPolicy(QSizePolicy::Preferred);
        sizepolicy.setHorizontalPolicy(QSizePolicy::Minimum);
        label->setSizePolicy(sizepolicy);
        label->setMinimumWidth(120);
        layout->addWidget(label);
        layout->addWidget(_mappingButtonListPrimary[entry]);
        layout->addWidget(_mappingButtonListSecondary[entry]);
        vlayout->addLayout(layout);
    }
}

QString ConfigDialog::_getNameFromButtonIndex(CONTROLLERENUM index) {
    switch(index) {
        case FIRE:
            return QString::fromStdString("Fire");
        case AIM:
            return QString::fromStdString("Aim");
        case ACCEPT:
            return QString::fromStdString("Next Weapon/Accept");
        case CANCEL:
            return QString::fromStdString("Reload/Use/Cancel");
        case START:
            return QString::fromStdString("Start");
        case CROUCH:
            return QString::fromStdString("Crouch");
        case KNEEL:
            return QString::fromStdString("Kneel (Perfect Dark)");
        case PREVIOUSWEAPON:
            return QString::fromStdString("Previous Weapon");
        case NEXTWEAPON:
            return QString::fromStdString("Next Weapon");
        case FORWARDS:
            return QString::fromStdString("Forwards");
        case BACKWARDS:
            return QString::fromStdString("Backwards");
        case STRAFELEFT:
            return QString::fromStdString("Strafe Left");
        case STRAFERIGHT:
            return QString::fromStdString("Strafe Right");
        case UP:
            return QString::fromStdString("Analog Up");
        case DOWN:
            return QString::fromStdString("Analog Down");
        case LEFT:
            return QString::fromStdString("Analog Left");
        case RIGHT:
            return QString::fromStdString("Analog Right");
        case RESETGYRO:
            return QString::fromStdString("Center Viewpoint");
        case TOGGLEGYRO:
            return QString::fromStdString("Toggle Gyroscope");
        case CALIBRATEGYRO:
            return QString::fromStdString("Calibrate Gyroscope");
        case TOTALBUTTONS:
        default:
            return QString::fromStdString("Invalid");
    }
}

bool ConfigDialog::GetConfigDialogState() {
    return _configdialogisopen;
}

void ConfigDialog::_loadDevicesIntoDeviceBox(CONTROLLERMODE mode) {
    int deviceno = 0;
    switch(mode) {
        case FULLCONTROLLER:
        // Disable and clear secondary device box.
        _baseDialog->secondaryDeviceBox->clear();
        _baseDialog->secondaryDeviceBox->setDisabled(true);

        _baseDialog->primaryDeviceBox->clear();
        _baseDialog->primaryDeviceBox->setEnabled(true);

        for(JSDevice dev : _loadedfull) {
            auto strbuild = std::stringstream();
            deviceno++;
            strbuild << deviceno << ": " << _jsdriver->GetNameOfDevice(dev);
            _baseDialog->primaryDeviceBox->addItem(
                    QString::fromStdString(strbuild.str()));
        }
        break;
        case JOYCONS:
            // Disable and clear secondary device box.
            _baseDialog->secondaryDeviceBox->clear();
            _baseDialog->secondaryDeviceBox->setEnabled(true);

            _baseDialog->primaryDeviceBox->clear();
            _baseDialog->primaryDeviceBox->setEnabled(true);

            for(JSDevice dev : _loadedjoyconprimary) {
                auto strbuild = std::stringstream();
                deviceno++;
                strbuild << deviceno << ": " << _jsdriver->GetNameOfDevice(dev);
                _baseDialog->primaryDeviceBox->addItem(
                        QString::fromStdString(strbuild.str()));
            }

            deviceno = 0;
            for(JSDevice dev : _loadedjoyconsecondary) {
                auto strbuild = std::stringstream();
                deviceno++;
                strbuild << deviceno << ": " << _jsdriver->GetNameOfDevice(dev);
                _baseDialog->secondaryDeviceBox->addItem(
                        QString::fromStdString(strbuild.str()));
            }
            break;
        case DISCONNECTED:
        default:
            _baseDialog->secondaryDeviceBox->clear();
            _baseDialog->secondaryDeviceBox->setEnabled(false);

            _baseDialog->primaryDeviceBox->clear();
            _baseDialog->primaryDeviceBox->setEnabled(false);
    }
}

void ConfigDialog::_loadMappingsIntoUi(PROFILE &profile, Assignment &asgn) {

    // Only use the primary device. We merge the joycon input into one mask anyway, so it doesn't matter what controller
    // we 'read' from.

    for(int index = 0; index < TOTALBUTTONS; index++) {
        _mappingButtonListPrimary[index]->setText(QString::fromStdString(JoyShockDriver::GetButtonLabelForController(asgn.PrimaryDevice, profile.BUTTONPRIM[index])));
        _mappingButtonListSecondary[index]->setText(QString::fromStdString(JoyShockDriver::GetButtonLabelForController(asgn.SecondaryDevice, profile.BUTTONSEC[index])));
    }
}

//void ConfigDialog::_disconnectDevices()

ConfigDialog::~ConfigDialog() {
    delete _baseDialog;
}

// -----------------------------------------------------------------------------------------------
// Slots & Signals
// -----------------------------------------------------------------------------------------------

void ConfigDialog::on_cancelButton_clicked() {
    this->close();
}

void ConfigDialog::on_okButton_clicked() {
    _commitAssignments();
    _commitProfiles();
    _commitGlobals();
    this->close();
}

void ConfigDialog::on_primaryDeviceBox_currentIndexChanged(int index) {
    auto cmode = _localassignments[_selectedplayer].ControllerMode;

    // bounds checking
    switch(cmode) {
        case DISCONNECTED:
        default:
            return;
        case FULLCONTROLLER:
            if (index >= _loadedfull.size() || index < 0) {
                return;
            }
            _localassignments[_selectedplayer].PrimaryDevice = _loadedfull[index];
            _loadMappingsIntoUi(_localprofiles[_selectedplayer], _localassignments[_selectedplayer]);
            break;
        case JOYCONS:
            if (index >= _loadedjoyconprimary.size() || index < 0) {
                return;
            }
            _localassignments[_selectedplayer].PrimaryDevice = _loadedjoyconprimary[index];
            _loadMappingsIntoUi(_localprofiles[_selectedplayer], _localassignments[_selectedplayer]);
            break;
    }
}

void ConfigDialog::on_secondaryDeviceBox_currentIndexChanged(int index) {
    auto cmode = _localassignments[_selectedplayer].ControllerMode;

    // bounds checking
    switch(cmode) {
        case DISCONNECTED:
        case FULLCONTROLLER:
        default:
            return;
        case JOYCONS:
            if (index >= _loadedjoyconsecondary.size() || index < 0)  {
                return;
            }
            _localassignments[_selectedplayer].SecondaryDevice = _loadedjoyconsecondary[index];
            _loadMappingsIntoUi(_localprofiles[_selectedplayer], _localassignments[_selectedplayer]);
            break;
    }
}

void ConfigDialog::on_controllerModeBox_currentIndexChanged(int index) {
    // jumping to default switch statements on conditions is one of the only valid uses for goto.
    // simply, you cannot have a player attached to a device if they do not have any controllers
    // of that class connected. Treat an empty controller list as though it was DISCONNECTED.
    // TODO: Consider refactoring this single, horrible decision out.
    switch(index) {
        case FULLCONTROLLER:
            if (_loadedfull.empty())
                goto ListEmpty; // no full controllers; treat as DISCONNECTED.
            _loadDevicesIntoDeviceBox(FULLCONTROLLER);
            _localassignments[_selectedplayer].ControllerMode = FULLCONTROLLER;
            _localassignments[_selectedplayer].PrimaryDevice = _loadedfull.front(); // Always select the first con.
            _loadMappingsIntoUi(_localprofiles[_selectedplayer], _localassignments[_selectedplayer]);
            break;
        case JOYCONS:
            if(_loadedjoyconprimary.empty() || _loadedjoyconsecondary.empty())
                goto ListEmpty; // no joycons; treat as DISCONNECTED.
            _loadDevicesIntoDeviceBox(JOYCONS);
            _localassignments[_selectedplayer].ControllerMode = JOYCONS;
            _localassignments[_selectedplayer].PrimaryDevice = _loadedjoyconprimary.front(); // Always select the first con.
            _localassignments[_selectedplayer].SecondaryDevice = _loadedjoyconsecondary.front();
            _loadMappingsIntoUi(_localprofiles[_selectedplayer],  _localassignments[_selectedplayer]);
            break;
        default:
        case DISCONNECTED:
        ListEmpty:
            _loadDevicesIntoDeviceBox(DISCONNECTED);
            _localassignments[_selectedplayer].ControllerMode = DISCONNECTED;
            _localassignments[_selectedplayer].PrimaryDevice = {-1, None};
            _localassignments[_selectedplayer].SecondaryDevice = {-1, None};
            _loadMappingsIntoUi(_localprofiles[_selectedplayer], _localassignments[_selectedplayer]);
            break;
    }
}

// insane boilerplate for linking all of the sliders to their respective textboxes...

// Gyro Aim X
void ConfigDialog::on_playerSettingsTabGyroXAxisSensitivitySlider_valueChanged(int value) {
    _baseDialog->playerSettingsTabGyroXAxisSensitivitySpinbox->setValue(value / 100.0);
    _localprofiles[_selectedplayer].GyroscopeSensitivity.x = (value / 100.0);
}

void ConfigDialog::on_playerSettingsTabGyroXAxisSensitivitySpinbox_valueChanged(double value) {
    _baseDialog->playerSettingsTabGyroXAxisSensitivitySlider->setValue(value * 100.0);
    _localprofiles[_selectedplayer].GyroscopeSensitivity.x = (float)value;
}

// Y
void ConfigDialog::on_playerSettingsTabGyroYAxisSensitivitySlider_valueChanged(int value) {
    _baseDialog->playerSettingsTabGyroYAxisSensitivitySpinbox->setValue(value / 100.0);
    _localprofiles[_selectedplayer].GyroscopeSensitivity.y = value / 100.0;
}

void ConfigDialog::on_playerSettingsTabGyroYAxisSensitivitySpinbox_valueChanged(double value) {
    _baseDialog->playerSettingsTabGyroYAxisSensitivitySlider->setValue(value * 100.0);
    _localprofiles[_selectedplayer].GyroscopeSensitivity.y = (float)value;
}

// Stick Aim X
void ConfigDialog::on_playerSettingsTabStickAimSensitivityXSlider_valueChanged(int value) {
    _baseDialog->playerSettingsTabStickAimSensitivityXSpinbox->setValue(value / 100.0);
    _localprofiles[_selectedplayer].AimStickSensitivity.x = (value / 100.0);
}

void ConfigDialog::on_playerSettingsTabStickAimSensitivityXSpinbox_valueChanged(double value) {
    _baseDialog->playerSettingsTabStickAimSensitivityXSlider->setValue(value * 100.0);
    _localprofiles[_selectedplayer].AimStickSensitivity.x = (float) value;
}

// Y
void ConfigDialog::on_playerSettingsTabStickAimSensitivityYSlider_valueChanged(int value) {
    _baseDialog->playerSettingsTabStickAimSensitivityYSpinbox->setValue(value / 100.0);
    _localprofiles[_selectedplayer].AimStickSensitivity.y = (value / 100.0f);
}

void ConfigDialog::on_playerSettingsTabStickAimSensitivityYSpinbox_valueChanged(double value) {
    _baseDialog->playerSettingsTabStickAimSensitivityYSlider->setValue(value * 100.0);
    _localprofiles[_selectedplayer].AimStickSensitivity.y = (float) value;
}

// Stick Aim Deadzone X

void ConfigDialog::on_playerSettingsTabStickAimDeadzoneXSlider_valueChanged(int value) {
    _baseDialog->playerSettingsTabStickAimDeadzoneXSpinbox->setValue(value / 100.0);
    _localprofiles[_selectedplayer].AimstickDeadzone.x = (value / 100.0f);
}

void ConfigDialog::on_playerSettingsTabStickAimDeadzoneXSpinbox_valueChanged(double value) {
    _baseDialog->playerSettingsTabStickAimDeadzoneXSlider->setValue(value * 100.0);
    _localprofiles[_selectedplayer].AimstickDeadzone.x = (float) value;
}

// Y
void ConfigDialog::on_playerSettingsTabStickAimDeadzoneYSlider_valueChanged(int value) {
    _baseDialog->playerSettingsTabStickAimDeadzoneYSpinbox->setValue(value / 100.0);
    _localprofiles[_selectedplayer].AimstickDeadzone.y = (value / 100.0f);
}

void ConfigDialog::on_playerSettingsTabStickAimDeadzoneYSpinbox_valueChanged(double value) {
    _baseDialog->playerSettingsTabStickAimDeadzoneYSlider->setValue(value * 100.0);
    _localprofiles[_selectedplayer].AimstickDeadzone.y = (float) value;
}

// Stick Move X
void ConfigDialog::on_playerSettingsTabStickMoveDeadzoneXSlider_valueChanged(int value) {
    _baseDialog->playerSettingsTabStickMoveDeadzoneXSpinbox->setValue(value / 100.0);
    _localprofiles[_selectedplayer].MoveStickDeadzone.x = (value / 100.0f);
}

void ConfigDialog::on_playerSettingsTabStickMoveDeadzoneXSpinbox_valueChanged(double value) {
    _baseDialog->playerSettingsTabStickMoveDeadzoneXSlider->setValue(value * 100.0);
    _localprofiles[_selectedplayer].MoveStickDeadzone.x = (float) value;
}

// Y
void ConfigDialog::on_playerSettingsTabStickMoveDeadzoneYSlider_valueChanged(int value) {
    _baseDialog->playerSettingsTabStickMoveDeadzoneYSpinbox->setValue(value / 100.0);
    _localprofiles[_selectedplayer].MoveStickDeadzone.y = (value / 100.0f);
}

void ConfigDialog::on_playerSettingsTabStickMoveDeadzoneYSpinbox_valueChanged(double value) {
    _baseDialog->playerSettingsTabStickMoveDeadzoneYSlider->setValue(value * 100.0);
    _localprofiles[_selectedplayer].MoveStickDeadzone.y = (float) value;
}

void ConfigDialog::on_globalFovSpinbox_valueChanged(int value) {
    _baseDialog->globalFovSlider->setValue(value);
    _settingsptr->SetFovOverride(value);
}

void ConfigDialog::on_globalFovSlider_valueChanged(int value) {
    _baseDialog->globalFovSpinbox->setValue(value);
    _settingsptr->SetFovOverride(value);
}

void ConfigDialog::on_playerSettingsTabGyroAimingStyleBox(int index) {
    switch(index) {
        default:
        case 0:
            _localprofiles->FreeAiming = true;
            break;
        case 1:
            _localprofiles->FreeAiming = false;
            break;
    }
}

void ConfigDialog::on_reconnectControllers_clicked() {
    _jsdriver->ReconnectControllers();
    _loadedfull = _jsdriver->GetConnectedFullControllers();
    _loadedjoyconprimary = _jsdriver->GetConnectedLeftJoycons();
    _loadedjoyconsecondary = _jsdriver->GetConnectedRightJoycons();

    // Reset all controllers to disconnected.
    _loadDevicesIntoDeviceBox(DISCONNECTED);
    for(int ctrl = PLAYER1; ctrl < ALLPLAYERS; ctrl++) {
        _localassignments[_selectedplayer].ControllerMode = DISCONNECTED;
        _localassignments[_selectedplayer].PrimaryDevice = {-1, None};
        _localassignments[_selectedplayer].SecondaryDevice = {-1, None};
    }
    _loadMappingsIntoUi(_localprofiles[_selectedplayer], _localassignments[_selectedplayer]);
    _loadDevicesIntoDeviceBox(DISCONNECTED);
    _baseDialog->controllerModeBox->setCurrentIndex(DISCONNECTED);

    // Commit assignments as all original assignments may now be invalid.
    _commitAssignments();
}

void ConfigDialog::on_playerSelectionButtonGroup_buttonClicked(QAbstractButton* button) {
    switch(_baseDialog->playerSelectionButtonGroup->id(button)) {
        case 0:
            _selectedplayer = PLAYER1;
            break;
        case 1:
            _selectedplayer = PLAYER2;
            break;
        case 2:
            _selectedplayer = PLAYER3;
            break;
        case 3:
            _selectedplayer = PLAYER4;
            break;
    }


    _loadProfileSettingsIntoUi(_localprofiles[_selectedplayer]);
    _loadDevicesIntoDeviceBox(_localassignments[_selectedplayer].ControllerMode);
    _loadMappingsIntoUi(_localprofiles[_selectedplayer], _localassignments[_selectedplayer]);
}

// ---------------------------------------------------------------------------------------------------------

void ConfigDialog::_commitAssignments() {
    for(int ctrl = PLAYER1; ctrl < ALLPLAYERS; ctrl++) {
        _settingsptr->SetAssignmentForPlayer(static_cast<PLAYERS>(ctrl), _localassignments[ctrl]);
    }
}

void ConfigDialog::_commitProfiles() {
    for(int ctrl = PLAYER1; ctrl < ALLPLAYERS; ctrl++) {
        _settingsptr->SetProfileForPlayer(static_cast<PLAYERS>(ctrl), _localprofiles[ctrl]);
    }
}

void ConfigDialog::_commitGlobals() {
    _settingsptr->SetShowGoldeneyeCrosshair(_baseDialog->globalGoldeneyeCrosshairCheckbox->isChecked());
    _settingsptr->SetOverrideRatio(vec2<int>{_baseDialog->globalRatioHorizontalSpinbox->value(), _baseDialog->globalRatioVerticalSpinbox->value() });
    _settingsptr->SetFovOverride(_baseDialog->globalFovSpinbox->value());
}

void ConfigDialog::_mapButtonToCommand(CONTROLLERENUM command, bool isSecondary) {
    _locked = true;
    auto asgn = _localassignments[_selectedplayer];
    auto start = std::chrono::steady_clock::now();
    auto now = start;
    QList<QPushButton*> buttonlist;

    if(!isSecondary)
        buttonlist = _mappingButtonListPrimary;
    else
        buttonlist = _mappingButtonListSecondary;

    if (asgn.ControllerMode == DISCONNECTED) {
        return;
    }

    std::chrono::duration<float> dur = start - now;
    buttonlist[command]->setText(QString::fromStdString("5..."));
    while (dur.count() < 5.0f) {
        QApplication::processEvents();
        if (asgn.ControllerMode == FULLCONTROLLER) {
            auto btn = JoyShockDriver::GetFirstButtonFromDevice(asgn.PrimaryDevice);
            if (btn != 0) {

                if(isSecondary) _localprofiles[_selectedplayer].BUTTONSEC[command] = btn;
                else _localprofiles[_selectedplayer].BUTTONPRIM[command] = btn;

                buttonlist[command]->setText(QString::fromStdString(
                        JoyShockDriver::GetButtonLabelForController(asgn.PrimaryDevice, btn)));
                return;
            }
        }
        else if(asgn.ControllerMode == JOYCONS) {
            auto btn_primary = JoyShockDriver::GetFirstButtonFromDevice(asgn.PrimaryDevice);
            auto btn_secondary = JoyShockDriver::GetFirstButtonFromDevice(asgn.SecondaryDevice);
            auto btn = btn_primary & btn_secondary;

            if (btn != 0) {

                if(isSecondary) _localprofiles[_selectedplayer].BUTTONSEC[command] = btn;
                else _localprofiles[_selectedplayer].BUTTONPRIM[command] = btn;

                buttonlist[command]->setText(QString::fromStdString(
                        JoyShockDriver::GetButtonLabelForController(asgn.PrimaryDevice, btn)));
                return;
            }
        }
            if (dur.count() > 4.0f)
                buttonlist[command]->setText(QString::fromStdString("1..."));
            else if (dur.count() > 3.0f)
                buttonlist[command]->setText(QString::fromStdString("2..."));
            else if (dur.count() > 2.0f)
                buttonlist[command]->setText(QString::fromStdString("3..."));
            else if (dur.count() > 1.0f)
                buttonlist[command]->setText(QString::fromStdString("4..."));

            now = std::chrono::steady_clock::now();
            dur = now - start;
    }
    if(isSecondary) _localprofiles[_selectedplayer].BUTTONSEC[command] = 0;
    else _localprofiles[_selectedplayer].BUTTONPRIM[command] = 0;
    buttonlist[command]->setText(
            QString::fromStdString(JoyShockDriver::GetButtonLabelForController(asgn.PrimaryDevice, -1)));
}