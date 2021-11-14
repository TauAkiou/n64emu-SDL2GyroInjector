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

    if(_jsdriver->IsThreadRunning()) {
        // Lock the FoV controls if the plugin is running.
        _baseDialog->globalFovSlider->setEnabled(false);
        _baseDialog->globalFovSlider->setToolTip(_cnststr_fov_locked);
        _baseDialog->globalFovSpinbox->setEnabled(false);
        _baseDialog->globalFovSpinbox->setToolTip(_cnststr_fov_locked);
        _baseDialog->globalFovTopResetFovButton->setEnabled(false);
        _baseDialog->globalFovTopResetFovButton->setToolTip(_cnststr_fov_locked);
    }

    _createPrimaryButtonLayouts();

    _loadedfull = _jsdriver->GetConnectedControllers();
    _loadedjoyconprimary = _jsdriver->GetConnectedLeftJoycons();
    _loadedjoyconsecondary = _jsdriver->GetConnectedRightJoycons();
    _selectedplayer = PLAYER1;

    _baseDialog->controllerModeBox->setCurrentIndex(_localassignments[_selectedplayer].ControllerMode);

    _loadProfileSettingsIntoUi(_localprofiles[_selectedplayer]);
    _loadDevicesIntoDeviceBox(_localassignments[_selectedplayer].ControllerMode);
    _loadMappingsIntoUi(_localprofiles[_selectedplayer], _localassignments[_selectedplayer]);
}

void ConfigDialog::_setPlayerColorAndDefaultNumber(const js_settings::PROFILE& prf, Assignment asgn) {
    switch(asgn.ControllerMode) {
        default:
        case DISCONNECTED:
            return;
        case JOYCONS:
            // JSL is smart enough to ignore a line for controllers that don't have a particular feature.
            //_jsdriver->SetSPCJCNumber(asgn.SecondaryDevice, 15);
        case FULLCONTROLLER:
            //_jsdriver->SetSPCJCNumber(asgn.PrimaryDevice, 15);
            //_jsdriver->SetDS4Color(asgn.PrimaryDevice, prf.DS4Color);
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

void ConfigDialog::_loadProfileSettingsIntoUi(const js_settings::PROFILE profile) {
    //
    _baseDialog->playerSettingsTabStickAimStickLayoutBox->setCurrentIndex(profile.AimStick);
    _baseDialog->playerSettingsTabStickAimStickModeBox->setCurrentIndex(profile.StickMode);
    _baseDialog->playerSettingsTabGyroAimingStyleBox->setCurrentIndex(profile.FreeAiming);
    _baseDialog->playerSettingsTabGyroInvertCheckbox->setChecked(profile.GyroPitchInverted);
    _baseDialog->playerSettingsTabStickInvertAimPitchCheckbox->setChecked(profile.StickPitchInverted);
    _baseDialog->playerSettingsTabOtherCursorAimingPerfectDarkCheckbox->setChecked(profile.PerfectDarkAimMode);
    _baseDialog->playerSettingsTabOtherCursorAimingGoldeneyeCheckbox->setChecked(profile.GoldeneyeAimMode);
    _baseDialog->playerSettingsTabOtherAllowAimingUsingStickCheckbox->setChecked(profile.AllowStickInAimMode);

    auto ds4color = _getColorFromInt(profile.DS4Color);
    _baseDialog->playerSettingsTabOtherDS4SpinboxRed->setValue(ds4color.r);
    _baseDialog->playerSettingsTabOtherDS4SpinboxGreen->setValue(ds4color.g);
    _baseDialog->playerSettingsTabOtherDS4SpinboxBlue->setValue(ds4color.b);

    //auto colorrole = _baseDialog->colorwidget.

    _baseDialog->playerSettingsTabGyroXAxisSensitivitySpinbox->setValue(profile.GyroscopeSensitivity.x);
    _baseDialog->playerSettingsTabGyroXAxisSensitivitySlider->setValue(profile.GyroscopeSensitivity.x * 100);

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

        // Item is unselected
        _baseDialog->primaryDeviceBox->addItem(QString::fromStdString("---"));

        for(auto & dev : _loadedfull) {
            auto strbuild = std::stringstream();

            strbuild << deviceno+1 << ": " << dev->GetControllerName();
            _baseDialog->primaryDeviceBox->addItem(QString::fromStdString(strbuild.str()));

            if(dev == _localassignments[_selectedplayer].PrimaryDevice) {
                _baseDialog->primaryDeviceBox->setCurrentIndex(deviceno+1);
            }

            deviceno++;
        }

        if(_localassignments[_selectedplayer].PrimaryDevice == nullptr) {
            _baseDialog->primaryDeviceBox->setCurrentIndex(0);
        }

        // Remember to set the devices appropriately.

        break;
        case JOYCONS:
            /* SDL2 does not yet have proper support for Joycon pairs.
            _baseDialog->secondaryDeviceBox->clear();
            _baseDialog->secondaryDeviceBox->setEnabled(true);

            _baseDialog->primaryDeviceBox->clear();
            _baseDialog->primaryDeviceBox->setEnabled(true);

            for(int dev = 0; dev < _loadedjoyconprimary.size(); dev++) {
                auto strbuild = std::stringstream();
                deviceno++;
                strbuild << deviceno << ": " << _jsdriver->GetNameOfDevice(_loadedjoyconprimary[dev]);
                _baseDialog->primaryDeviceBox->addItem(
                        QString::fromStdString(strbuild.str()));
            }

            for(int dev = 0; dev < _loadedjoyconprimary.size(); dev++) {
                if(_localassignments[_selectedplayer].PrimaryDevice.Handle == _loadedjoyconprimary[dev].Handle) {
                    _baseDialog->primaryDeviceBox->setCurrentIndex(dev);

                }
            }

            deviceno = 0;
            for(int dev = 0; dev < _loadedjoyconsecondary.size(); dev++) {
                auto strbuild = std::stringstream();
                deviceno++;
                strbuild << deviceno << ": " << _jsdriver->GetNameOfDevice(_loadedjoyconsecondary[dev]);
                _baseDialog->secondaryDeviceBox->addItem(
                        QString::fromStdString(strbuild.str()));
            }

            for(int dev = 0; dev < _loadedjoyconprimary.size(); dev++) {
                if(_localassignments[_selectedplayer].SecondaryDevice.Handle == _loadedjoyconprimary[dev].Handle) {
                    _baseDialog->secondaryDeviceBox->setCurrentIndex(dev);

                }
            }
            break;
             */
        case DISCONNECTED:
        default:
            _baseDialog->secondaryDeviceBox->clear();
            _baseDialog->secondaryDeviceBox->setEnabled(false);

            _baseDialog->primaryDeviceBox->clear();
            _baseDialog->primaryDeviceBox->setEnabled(false);
    }
}

void ConfigDialog::_loadMappingsIntoUi(js_settings::PROFILE&profile, Assignment &asgn) {
    if(asgn.PrimaryDevice != nullptr) {
        for (int index = 0; index < TOTALBUTTONS; index++) {
            _mappingButtonListPrimary[index]->setText(
                    QString::fromStdString(asgn.PrimaryDevice->GetButtonNameFromBitmask(profile.BUTTONPRIM[index])));
            _mappingButtonListSecondary[index]->setText(
                    QString::fromStdString(asgn.PrimaryDevice->GetButtonNameFromBitmask(profile.BUTTONSEC[index])));
        }
    }
    else {
        for (int index = 0; index < TOTALBUTTONS; index++) {
            _mappingButtonListPrimary[index]->setText(
                    QString::fromStdString(SDLDevice::GetGenericButtonNameFromBitmask(profile.BUTTONPRIM[index])));
            _mappingButtonListSecondary[index]->setText(
                    QString::fromStdString(SDLDevice::GetGenericButtonNameFromBitmask(profile.BUTTONSEC[index])));
        }
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
    _settingsptr->SaveConfigFile();
    this->close();
}

void ConfigDialog::on_primaryDeviceBox_activated(int index) {
    auto cmode = _localassignments[_selectedplayer].ControllerMode;

    // bounds checking
    switch(cmode) {
        case DISCONNECTED:
        case JOYCONS:
        default:
            return;
        case FULLCONTROLLER:

            // Checking.
            if (!index) {
            _localassignments[_selectedplayer].PrimaryDevice = nullptr;
                _baseDialog->primaryDeviceBox->setCurrentIndex(-1);
                return;
            }

                for(int player = PLAYER1; player < ALLPLAYERS; player++) {
                    if (_loadedfull[index-1] == _localassignments[player].PrimaryDevice && player != _selectedplayer) {
                        // Controller is already selected. Null the primarydevice.
                        _localassignments[_selectedplayer].PrimaryDevice = nullptr;
                        _baseDialog->primaryDeviceBox->setCurrentIndex(-1);
                        _loadMappingsIntoUi(_localprofiles[_selectedplayer], _localassignments[_selectedplayer]);
                        return;
                    }
                }

            _localassignments[_selectedplayer].PrimaryDevice = _loadedfull[index-1];
            _localassignments[_selectedplayer].PrimaryDevice->AssignPlayerIndex(_selectedplayer);
            _loadMappingsIntoUi(_localprofiles[_selectedplayer], _localassignments[_selectedplayer]);
            // set index.
            _localassignments[_selectedplayer].PrimaryDevice->AssignPlayerIndex(_selectedplayer+1);

            break;
            /*
        case JOYCONS:
            if (index >= _loadedjoyconprimary.size() || index < 0) {
                return;
            }
            _localassignments[_selectedplayer].PrimaryDevice = _loadedjoyconprimary[index];
            _loadMappingsIntoUi(_localprofiles[_selectedplayer], _localassignments[_selectedplayer]);
            break;
             */
    }
}

void ConfigDialog::on_secondaryDeviceBox_activated(int index) {
    auto cmode = _localassignments[_selectedplayer].ControllerMode;

    // bounds checking
    switch(cmode) {
        case DISCONNECTED:
        case FULLCONTROLLER:
        default:
        case JOYCONS:
            return;
            /*
            if (index >= _loadedjoyconsecondary.size() || index < 0)  {
                return;
            }
            _localassignments[_selectedplayer].SecondaryDevice = _loadedjoyconsecondary[index];
            _loadMappingsIntoUi(_localprofiles[_selectedplayer], _localassignments[_selectedplayer]);
            break;
             */
    }
}

void ConfigDialog::on_controllerModeBox_activated(int index) {
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
            _localassignments[_selectedplayer].PrimaryDevice = nullptr;
            _loadMappingsIntoUi(_localprofiles[_selectedplayer], _localassignments[_selectedplayer]);
            break;
        case JOYCONS:
            /*
            if(_loadedjoyconprimary.empty() || _loadedjoyconsecondary.empty())
                goto ListEmpty; // no joycons; treat as DISCONNECTED.
            _loadDevicesIntoDeviceBox(JOYCONS);
            _localassignments[_selectedplayer].ControllerMode = JOYCONS;
            _localassignments[_selectedplayer].PrimaryDevice = _loadedjoyconprimary.front(); // Always select the first con.
            _localassignments[_selectedplayer].SecondaryDevice = _loadedjoyconsecondary.front();
            _loadMappingsIntoUi(_localprofiles[_selectedplayer],  _localassignments[_selectedplayer]);
            break;
             */
        default:
        case DISCONNECTED:
        ListEmpty:
            _loadDevicesIntoDeviceBox(DISCONNECTED);
            _localassignments[_selectedplayer].ControllerMode = DISCONNECTED;
            _localassignments[_selectedplayer].PrimaryDevice = nullptr;
            _localassignments[_selectedplayer].SecondaryDevice = nullptr;
            _loadMappingsIntoUi(_localprofiles[_selectedplayer], _localassignments[_selectedplayer]);
            break;
    }
}

// insane boilerplate for linking all of the sliders to their respective textboxes...

// Gyro Aim X
void ConfigDialog::on_playerSettingsTabGyroXAxisSensitivitySlider_sliderMoved(int value) {
    _baseDialog->playerSettingsTabGyroXAxisSensitivitySpinbox->setValue(value / 100.0);
    _localprofiles[_selectedplayer].GyroscopeSensitivity.x = roundf((float)value) / 100.0f;
}

void ConfigDialog::on_playerSettingsTabGyroXAxisSensitivitySpinbox_valueChanged(double value) {
    _baseDialog->playerSettingsTabGyroXAxisSensitivitySlider->setValue((int)(value * 100.0f));
    _localprofiles[_selectedplayer].GyroscopeSensitivity.x = roundf((float)value * 100 / 100);
}

// Y
void ConfigDialog::on_playerSettingsTabGyroYAxisSensitivitySlider_sliderMoved(int value) {
    _baseDialog->playerSettingsTabGyroYAxisSensitivitySpinbox->setValue(value / 100.0);
    //_localprofiles[_selectedplayer].GyroscopeSensitivity.y = value / 100.0;
}

void ConfigDialog::on_playerSettingsTabGyroYAxisSensitivitySpinbox_valueChanged(double value) {
    _baseDialog->playerSettingsTabGyroYAxisSensitivitySlider->setValue(value * 100.0);
    _localprofiles[_selectedplayer].GyroscopeSensitivity.y = roundf((float)value * 100 / 100);
}

// Stick Aim X
void ConfigDialog::on_playerSettingsTabStickAimSensitivityXSlider_sliderMoved(int value) {
    _baseDialog->playerSettingsTabStickAimSensitivityXSpinbox->setValue(value / 100.0);
    _localprofiles[_selectedplayer].AimStickSensitivity.x = (value / 100.0);
}

void ConfigDialog::on_playerSettingsTabStickAimSensitivityXSpinbox_valueChanged(double value) {
    _baseDialog->playerSettingsTabStickAimSensitivityXSlider->setValue(value * 100.0);
    _localprofiles[_selectedplayer].AimStickSensitivity.x = (float) value;
}

// Y
void ConfigDialog::on_playerSettingsTabStickAimSensitivityYSlider_sliderMoved(int value) {
    _baseDialog->playerSettingsTabStickAimSensitivityYSpinbox->setValue(value / 100.0);
    _localprofiles[_selectedplayer].AimStickSensitivity.y = (value / 100.0f);
}

void ConfigDialog::on_playerSettingsTabStickAimSensitivityYSpinbox_valueChanged(double value) {
    _baseDialog->playerSettingsTabStickAimSensitivityYSlider->setValue(value * 100.0);
    _localprofiles[_selectedplayer].AimStickSensitivity.y = (float) value;
}

// Stick Aim Deadzone X

void ConfigDialog::on_playerSettingsTabStickAimDeadzoneXSlider_sliderMoved(int value) {
    _baseDialog->playerSettingsTabStickAimDeadzoneXSpinbox->setValue(value / 100.0);
    _localprofiles[_selectedplayer].AimstickDeadzone.x = (value / 100.0f);
}

void ConfigDialog::on_playerSettingsTabStickAimDeadzoneXSpinbox_valueChanged(double value) {
    _baseDialog->playerSettingsTabStickAimDeadzoneXSlider->setValue(value * 100.0);
    _localprofiles[_selectedplayer].AimstickDeadzone.x = (float) value;
}

// Y
void ConfigDialog::on_playerSettingsTabStickAimDeadzoneYSlider_sliderMoved(int value) {
    _baseDialog->playerSettingsTabStickAimDeadzoneYSpinbox->setValue(value / 100.0);
    _localprofiles[_selectedplayer].AimstickDeadzone.y = (value / 100.0f);
}

void ConfigDialog::on_playerSettingsTabStickAimDeadzoneYSpinbox_valueChanged(double value) {
    _baseDialog->playerSettingsTabStickAimDeadzoneYSlider->setValue(value * 100.0);
    _localprofiles[_selectedplayer].AimstickDeadzone.y = (float) value;
}

// Stick Move X
void ConfigDialog::on_playerSettingsTabStickMoveDeadzoneXSlider_sliderMoved(int value) {
    _baseDialog->playerSettingsTabStickMoveDeadzoneXSpinbox->setValue(value / 100.0);
    _localprofiles[_selectedplayer].MoveStickDeadzone.x = (value / 100.0f);
}

void ConfigDialog::on_playerSettingsTabStickMoveDeadzoneXSpinbox_valueChanged(double value) {
    _baseDialog->playerSettingsTabStickMoveDeadzoneXSlider->setValue(value * 100.0);
    _localprofiles[_selectedplayer].MoveStickDeadzone.x = (float) value;
}

// Y
void ConfigDialog::on_playerSettingsTabStickMoveDeadzoneYSlider_sliderMoved(int value) {
    _baseDialog->playerSettingsTabStickMoveDeadzoneYSpinbox->setValue(value / 100.0);
    _localprofiles[_selectedplayer].MoveStickDeadzone.y = ((float)value / 100.0f);
}

void ConfigDialog::on_playerSettingsTabStickMoveDeadzoneYSpinbox_valueChanged(double value) {
    _baseDialog->playerSettingsTabStickMoveDeadzoneYSlider->setValue(value * 100.0);
    _localprofiles[_selectedplayer].MoveStickDeadzone.y = (float) value;
}

void ConfigDialog::on_globalFovSpinbox_valueChanged(int value) {
    _baseDialog->globalFovSlider->setValue(value);
    _settingsptr->SetFovOverride(value);

    if(_settingsptr->GetFovOverride() < 60)
        _baseDialog->globalFovBottomLabel->setText("Below Default");
    else if(_settingsptr->GetFovOverride() == 60)
        _baseDialog->globalFovBottomLabel->setText("Default FOV");
    else if(_settingsptr->GetFovOverride() <= 80)
        _baseDialog->globalFovBottomLabel->setText("Above Default");
    else if(_settingsptr->GetFovOverride() <= 90)
        _baseDialog->globalFovBottomLabel->setText("Breaks Viewmodels");
    else
        _baseDialog->globalFovBottomLabel->setText("Breaks Viewmodels\\LOD");

}

void ConfigDialog::on_globalFovSlider_valueChanged(int value) {
    _baseDialog->globalFovSpinbox->setValue(value);
    _settingsptr->SetFovOverride(value);
}

void ConfigDialog::on_playerSettingsTabGyroAimingStyleBox_activated(int index) {
    switch(index) {
        default:
        case 0:
            _localprofiles[_selectedplayer].FreeAiming = STANDARD;
            break;
        case 1:
            _localprofiles[_selectedplayer].FreeAiming = SPLATOON;
            break;
        case 2:
            _localprofiles[_selectedplayer].FreeAiming = FREE;
            break;
    }
}

void ConfigDialog::on_playerSettingsTabStickAimStickModeBox_activated(int index) {
    switch(index) {
        default:
        case FULLSTICK:
            _localprofiles[_selectedplayer].StickMode = FULLSTICK;
            break;
        case XONLY:
            _localprofiles[_selectedplayer].StickMode = XONLY;
            break;
        case FLICK:
            _localprofiles[_selectedplayer].StickMode = FLICK;
            break;
    }
}

void ConfigDialog::on_reconnectControllers_clicked() {
    /*
    _jsdriver->ReconnectControllers();
    _loadedfull = _jsdriver->GetConnectedControllers();
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
     */
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

    _baseDialog->controllerModeBox->setCurrentIndex(_localassignments[_selectedplayer].ControllerMode);

    _loadProfileSettingsIntoUi(_localprofiles[_selectedplayer]);
    _loadDevicesIntoDeviceBox(_localassignments[_selectedplayer].ControllerMode);
    _loadMappingsIntoUi(_localprofiles[_selectedplayer], _localassignments[_selectedplayer]);
}

void ConfigDialog::on_playerSettingsTabStickInvertAimPitchCheckbox_stateChanged(int state) {
    _localprofiles[_selectedplayer].StickPitchInverted = (bool)state;
}

void ConfigDialog::on_playerSettingsTabGyroInvertCheckbox_stateChanged(int state) {
    _localprofiles[_selectedplayer].GyroPitchInverted = (bool)state;
}

void ConfigDialog::on_playerSettingsTabOtherCursorAimingGoldeneyeCheckbox_stateChanged(int state) {
    _localprofiles[_selectedplayer].GoldeneyeAimMode = (bool)state;
}

void ConfigDialog::on_playerSettingsTabOtherCursorAimingPerfectDarkCheckbox_stateChanged(int state) {
    _localprofiles[_selectedplayer].PerfectDarkAimMode = (bool)state;
}

void ConfigDialog::on_playerSettingsTabOtherAllowAimingUsingStickCheckbox_stateChanged(int state) {
    _localprofiles[_selectedplayer].AllowStickInAimMode = (bool)state;
}

void ConfigDialog::on_playerSettingsTabOtherDS4SpinboxRed_valueChanged(int value) {
    Color ds4_new_color = Color();
    ds4_new_color.r = value;
    ds4_new_color.g = _baseDialog->playerSettingsTabOtherDS4SpinboxGreen->value();
    ds4_new_color.b = _baseDialog->playerSettingsTabOtherDS4SpinboxBlue->value();

    _localprofiles[_selectedplayer].DS4Color = _getIntFromColor(ds4_new_color);

    //_localassignments[_selectedplayer].PrimaryDevice->SetLightbarColor(_localprofiles[_selectedplayer].DS4Color);
}

void ConfigDialog::on_playerSettingsTabOtherDS4SpinboxGreen_valueChanged(int value) {
    Color ds4_new_color = Color();
    ds4_new_color.r = _baseDialog->playerSettingsTabOtherDS4SpinboxRed->value();
    ds4_new_color.g = value;
    ds4_new_color.b = _baseDialog->playerSettingsTabOtherDS4SpinboxBlue->value();

    _localprofiles[_selectedplayer].DS4Color = _getIntFromColor(ds4_new_color);
    //_localassignments[_selectedplayer].PrimaryDevice->SetLightbarColor(_localprofiles[_selectedplayer].DS4Color);

}

void ConfigDialog::on_playerSettingsTabOtherDS4SpinboxBlue_valueChanged(int value) {
    Color ds4_new_color = Color();
    ds4_new_color.r = _baseDialog->playerSettingsTabOtherDS4SpinboxRed->value();
    ds4_new_color.g = _baseDialog->playerSettingsTabOtherDS4SpinboxGreen->value();
    ds4_new_color.b = value;

    _localprofiles[_selectedplayer].DS4Color = _getIntFromColor(ds4_new_color);
    //_localassignments[_selectedplayer].PrimaryDevice->SetLightbarColor(_localprofiles[_selectedplayer].DS4Color);

}

// ---------------------------------------------------------------------------------------------------------

void ConfigDialog::_commitAssignments() {
    for(int ctrl = PLAYER1; ctrl < ALLPLAYERS; ctrl++) {
        if(_localassignments[ctrl].PrimaryDevice == nullptr) {
            _localassignments[ctrl].ControllerMode = DISCONNECTED;
        }
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
            auto btn = asgn.PrimaryDevice->GetFirstPressedButton();
            if (btn != 0) {

                if(isSecondary) _localprofiles[_selectedplayer].BUTTONSEC[command] = btn;
                else _localprofiles[_selectedplayer].BUTTONPRIM[command] = btn;

                buttonlist[command]->setText(QString::fromStdString(asgn.PrimaryDevice->GetButtonNameFromBitmask(btn)));
                return;
            }
        }
        /*
        else if(asgn.ControllerMode == JOYCONS) {
            auto btn_primary = SdlDriver::GetFirstButtonFromDevice(asgn.PrimaryDevice);
            auto btn_secondary = SdlDriver::GetFirstButtonFromDevice(asgn.SecondaryDevice);
            auto btn = btn_primary & btn_secondary;

            if (btn != 0) {

                if(isSecondary) _localprofiles[_selectedplayer].BUTTONSEC[command] = btn;
                else _localprofiles[_selectedplayer].BUTTONPRIM[command] = btn;

                buttonlist[command]->setText(QString::fromStdString(
                        SdlDriver::GetButtonLabelForController(asgn.PrimaryDevice, btn)));
                return;
            }
        }
        */
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
            QString::fromStdString("None"));
}