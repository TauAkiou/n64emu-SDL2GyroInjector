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

bool ConfigDialog::GetConfigDialogState() {
    return _configdialogisopen;
}

void ConfigDialog::_loadDevicesIntoDeviceBox(CONTROLLERMODE mode) {
    int deviceno = 0;
    switch(mode) {
        case FULLCONTROLLER:
        // Disable and clear secondary device box.
        _configform->secondaryDeviceBox->clear();
        _configform->secondaryDeviceBox->setDisabled(true);

        _configform->primaryDeviceBox->clear();
        _configform->primaryDeviceBox->setEnabled(true);

        for(JSDevice dev : _loadedfull) {
            auto strbuild = std::stringstream();
            deviceno++;
            strbuild << deviceno << ": " << _jsdriver->GetNameOfDevice(dev);
            _configform->primaryDeviceBox->addItem(
                    QString::fromStdString(strbuild.str()));
        }
        break;
        case JOYCONS:
            // Disable and clear secondary device box.
            _configform->secondaryDeviceBox->clear();
            _configform->secondaryDeviceBox->setEnabled(true);

            _configform->primaryDeviceBox->clear();
            _configform->primaryDeviceBox->setEnabled(true);

            for(JSDevice dev : _loadedjoyconprimary) {
                auto strbuild = std::stringstream();
                deviceno++;
                strbuild << deviceno << ": " << _jsdriver->GetNameOfDevice(dev);
                _configform->primaryDeviceBox->addItem(
                        QString::fromStdString(strbuild.str()));
            }

            deviceno = 0;
            for(JSDevice dev : _loadedjoyconsecondary) {
                auto strbuild = std::stringstream();
                deviceno++;
                strbuild << deviceno << ": " << _jsdriver->GetNameOfDevice(dev);
                _configform->secondaryDeviceBox->addItem(
                        QString::fromStdString(strbuild.str()));
            }
            break;
        case DISCONNECTED:
        default:
            _configform->secondaryDeviceBox->clear();
            _configform->secondaryDeviceBox->setEnabled(false);

            _configform->primaryDeviceBox->clear();
            _configform->primaryDeviceBox->setEnabled(false);
    }
}

void ConfigDialog::_selectDeviceFromAssignment() {
    for(int players = 0; players < ALLPLAYERS; players++) {

    }
}

ConfigDialog::ConfigDialog(QDialog *parent) : QDialog(parent), _configform(new Ui::ConfigDialog) {
    _configform->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    _getCurrentConfigState();

    _loadedfull = _jsdriver->GetConnectedFullControllers();
    _loadedjoyconprimary = _jsdriver->GetConnectedLeftJoycons();
    _loadedjoyconsecondary = _jsdriver->GetConnectedRightJoycons();
    _selectedplayer = PLAYER1;

    _configform->controllerModeBox->setCurrentIndex(_localassignments[_selectedplayer].ControllerMode);

    _loadDevicesIntoDeviceBox(_localassignments[_selectedplayer].ControllerMode);
    _loadMappingsIntoUi(_localprofiles[_selectedplayer], _localassignments[_selectedplayer]);
}

void ConfigDialog::_getCurrentConfigState() {
    for(int player = PLAYER1; player < ALLPLAYERS; player++) {
        _localprofiles[player] = _settingsptr->GetProfileForPlayer(static_cast<PLAYERS>(player));
        _localassignments[player] = _settingsptr->GetAssignmentForPlayer(static_cast<PLAYERS>(player));
    }
}

void ConfigDialog::_loadMappingsIntoUi(PROFILE &profile, Assignment &asgn) {

    // Mappings

    // Only use the primary device. We merge the joycon input into one mask anyway, so it doesn't matter what controller
    // we 'read' from.

    // Fire
    _configform->mappingFirePrimaryButton->setText(
            QString::fromStdString(
                    _jsdriver->GetButtonLabelForController(asgn.PrimaryDevice, profile.BUTTONPRIM[FIRE])));
    _configform->mappingFireSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[FIRE])));

    // Aim
    _configform->mappingAimPrimaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice, profile.BUTTONPRIM[AIM])));
    _configform->mappingAimSecondaryButton->setText(
            QString::fromStdString(JoyShockDriver::GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[AIM])));
    // Next Weapon/Accept
    _configform->mappingNextWeaponAcceptPrimaryButton->setText(
            QString::fromStdString(
                    JoyShockDriver::GetButtonLabelForController(asgn.PrimaryDevice, profile.BUTTONPRIM[ACCEPT])));
    _configform->mappingNextWeaponAcceptSecondaryButton->setText(
            QString::fromStdString(JoyShockDriver::GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[ACCEPT])));
    // Reload/Cancel
    _configform->mappingReloadUseCancelPrimaryButton->setText(
            QString::fromStdString(
                    JoyShockDriver::GetButtonLabelForController(asgn.PrimaryDevice, profile.BUTTONPRIM[CANCEL])));
    _configform->mappingReloadUseCancelSecondaryButton->setText(
            QString::fromStdString(JoyShockDriver::GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[CANCEL])));
    // Start
    _configform->mappingStartPrimaryButton->setText(
            QString::fromStdString(
                    _jsdriver->GetButtonLabelForController(asgn.PrimaryDevice, profile.BUTTONPRIM[START])));
    _configform->mappingStartSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[START])));
    // Crouch
    _configform->mappingCrouchPrimaryButton->setText(
            QString::fromStdString(
                    _jsdriver->GetButtonLabelForController(asgn.PrimaryDevice, profile.BUTTONPRIM[CROUCH])));
    _configform->mappingCrouchSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[CROUCH])));
    // Kneel
    _configform->mappingPDKneelPrimaryButton->setText(
            QString::fromStdString(
                    _jsdriver->GetButtonLabelForController(asgn.PrimaryDevice, profile.BUTTONPRIM[KNEEL])));
    _configform->mappingPDKneelSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[KNEEL])));

    // Forwards
    _configform->mappingForwardPrimaryButton->setText(
            QString::fromStdString(
                    _jsdriver->GetButtonLabelForController(asgn.PrimaryDevice, profile.BUTTONPRIM[FORWARDS])));
    _configform->mappingForwardSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[FORWARDS])));
    // Backwards
    _configform->mappingBackwardsPrimaryButton->setText(
            QString::fromStdString(
                    _jsdriver->GetButtonLabelForController(asgn.PrimaryDevice, profile.BUTTONPRIM[BACKWARDS])));
    _configform->mappingBackwardsSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[BACKWARDS])));

    // Strafe Left
    _configform->mappingStrafeLeftPrimaryButton->setText(
            QString::fromStdString(
                    _jsdriver->GetButtonLabelForController(asgn.PrimaryDevice, profile.BUTTONPRIM[STRAFELEFT])));
    _configform->mappingStrafeLeftSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[STRAFERIGHT])));
    // Strafe Right
    _configform->mappingStrafeRightPrimaryButton->setText(
            QString::fromStdString(
                    _jsdriver->GetButtonLabelForController(asgn.PrimaryDevice, profile.BUTTONPRIM[STRAFERIGHT])));
    _configform->mappingStrafeRightSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[STRAFERIGHT])));

    // Analog Up
    _configform->mappingAnalogUpPrimaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice, profile.BUTTONPRIM[UP])));
    _configform->mappingAnalogUpSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[UP])));

    // Analog Down
    _configform->mappingAnalogDownPrimaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice, profile.BUTTONPRIM[DOWN])));
    _configform->mappingAnalogDownSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[DOWN])));

    // Analog Left
    _configform->mappingAnalogLeftPrimaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,profile.BUTTONPRIM[LEFT])));
    _configform->mappingAnalogLeftSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[LEFT])));

    // Analog Right
    _configform->mappingAnalogRightPrimaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,profile.BUTTONPRIM[RIGHT])));
    _configform->mappingAnalogRightSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[RIGHT])));

    // Next Weapon
    _configform->mappingNextWeaponPrimaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,profile.BUTTONPRIM[NEXTWEAPON])));
    _configform->mappingNextWeaponSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[NEXTWEAPON])));
    // Previous Weapon
    _configform->mappingPreviousWeaponPrimaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,profile.BUTTONPRIM[PREVIOUSWEAPON])));
    _configform->mappingPreviousWeaponSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[PREVIOUSWEAPON])));
    // Toggle Gyroscope
    _configform->mappingToggleGyroscopePrimaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,profile.BUTTONPRIM[TOGGLEGYRO])));
    _configform->mappingToggleGyroscopeSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[TOGGLEGYRO])));

    _configform->mappingCenterAimPrimaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,profile.BUTTONPRIM[RESETGYRO])));
    _configform->mappingCenterAimSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[RESETGYRO])));

    _configform->mappingCalibrateGyroscopePrimaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,profile.BUTTONPRIM[CALIBRATEGYRO])));
    _configform->mappingCalibrateGyroscopeSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[CALIBRATEGYRO])));
}

ConfigDialog::~ConfigDialog() {
    delete _configform;
}


void ConfigDialog::on_cancelButton_clicked() {
    this->close();
}

// -----------------------------------------------------------------------------------------------
// Slots & Signals
// -----------------------------------------------------------------------------------------------

void ConfigDialog::_mapButtonToPrimaryCommand() {

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
    switch(index) {
        default:
        case DISCONNECTED:
            _loadDevicesIntoDeviceBox(DISCONNECTED);
            _localassignments[_selectedplayer].ControllerMode = JOYCONS;
            _localassignments[_selectedplayer].PrimaryDevice = {-1, None};
            _localassignments[_selectedplayer].SecondaryDevice = {-1, None};
            _loadMappingsIntoUi(_localprofiles[_selectedplayer],  _localassignments[_selectedplayer]);

            break;
        case FULLCONTROLLER:
            _loadDevicesIntoDeviceBox(FULLCONTROLLER);
            _localassignments[_selectedplayer].ControllerMode = FULLCONTROLLER;
            _localassignments[_selectedplayer].PrimaryDevice = _loadedfull.front(); // Always select the first con.
            _loadMappingsIntoUi(_localprofiles[_selectedplayer],  _localassignments[_selectedplayer]);
            break;
        case JOYCONS:
            _loadDevicesIntoDeviceBox(JOYCONS);
            _localassignments[_selectedplayer].ControllerMode = JOYCONS;
            _localassignments[_selectedplayer].PrimaryDevice = _loadedjoyconprimary.front(); // Always select the first con.
            _localassignments[_selectedplayer].SecondaryDevice = _loadedjoyconsecondary.front();
            _loadMappingsIntoUi(_localprofiles[_selectedplayer],  _localassignments[_selectedplayer]);
            break;
    }

}

// insane boilerplate for linking all of the sliders to their respective textboxes...
void ConfigDialog::on_playerSettingsTabGyroXAxisSensitivitySlider_valueChanged(int value) {
    _configform->playerSettingsTabGyroXAxisSensitivitySpinbox->setValue(value / 100.0);
    _localprofiles[_selectedplayer].GyroscopeSensitivity.x = (value / 100.0);
}

void ConfigDialog::on_playerSettingsTabGyroXAxisSensitivitySpinbox_valueChanged(double value) {
    _configform->playerSettingsTabGyroXAxisSensitivitySlider->setValue(value * 100);
    _localprofiles[_selectedplayer].GyroscopeSensitivity.x = (float)value;

}

void ConfigDialog::on_playerSettingsTabGyroYAxisSensitivitySlider_valueChanged(int value) {
    _configform->playerSettingsTabGyroYAxisSensitivitySpinbox->setValue(value / 100.0);
}

void ConfigDialog::on_playerSettingsTabGyroYAxisSensitivitySpinbox_valueChanged(double value) {
    _configform->playerSettingsTabGyroYAxisSensitivitySlider->setValue(value * 100);
}

void ConfigDialog::on_reconnectControllers_clicked() {
    _jsdriver->ReconnectControllers();
    _loadedfull = _jsdriver->GetConnectedFullControllers();
    _loadedjoyconprimary = _jsdriver->GetConnectedLeftJoycons();
    _loadedjoyconsecondary = _jsdriver->GetConnectedRightJoycons();
    _getCurrentConfigState();

    on_primaryDeviceBox_currentIndexChanged(0);
    on_secondaryDeviceBox_currentIndexChanged(0);


}



