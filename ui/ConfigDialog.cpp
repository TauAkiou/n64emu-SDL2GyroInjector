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

void ConfigDialog::_loadDevicesIntoDeviceBox(JSD_ControllerType type) {
    if(type == SwitchPro || type == Dualshock_4) {
        // Disable and clear secondary device box.
        _configform->secondaryDeviceBox->clear();
        _configform->secondaryDeviceBox->setDisabled(true);

        _configform->primaryDeviceBox->clear();

        int deviceno = 0;
        for(JSDevice dev : _loadedfull) {
            auto strbuild = std::stringstream();
            deviceno++;
            strbuild << deviceno << ": " << _jsdriver->GetNameOfDevice(dev) << std::endl;
            _configform->primaryDeviceBox->addItem(
                    QString::fromStdString(strbuild.str()));
        }

    }
}


void ConfigDialog::_refresh() {
    _guibusy = false;
}

ConfigDialog::ConfigDialog(QDialog *parent) : QDialog(parent), _configform(new Ui::ConfigDialog) {
    _configform->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    _loadedfull = _jsdriver->GetConnectedFullControllers();
    _loadedjoyconprimary = _jsdriver->GetConnectedLeftJoycons();
    _loadedjoyconsecondary = _jsdriver->GetConnectedRightJoycons();

    for(int player = PLAYER1; player < ALLPLAYERS; player++) {
        _localprofiles[player] = _settingsptr->GetProfileForPlayer(static_cast<PLAYERS>(player));
        _localassignments[player] = _settingsptr->GetAssignmentForPlayer(static_cast<PLAYERS>(player));
    }

    _loadProfileIntoUI(_localprofiles[PLAYER1], _localassignments[PLAYER1]);
}

void ConfigDialog::_loadProfileIntoUI(PROFILE &profile, Assignment &asgn) {

    // Mappings

    // Only use the primary device. We merge the joycon input into one mask anyway, so it doesn't matter what controller
    // we 'read' from.

    // Fire
    _configform->mappingFirePrimaryButton->setText(
            QString::fromStdString(
                    _jsdriver->GetButtonLabelForController(asgn.PrimaryDevice, profile.BUTTONSEC[FIRE])));
    _configform->mappingFireSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[FIRE])));

    // Aim
    _configform->mappingAimPrimaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice, profile.BUTTONSEC[AIM])));
    _configform->mappingAimSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[AIM])));
    // Next Weapon/Accept
    _configform->mappingNextWeaponAcceptPrimaryButton->setText(
            QString::fromStdString(
                    _jsdriver->GetButtonLabelForController(asgn.PrimaryDevice, profile.BUTTONSEC[ACCEPT])));
    _configform->mappingNextWeaponAcceptSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[ACCEPT])));
    // Reload/Cancel
    _configform->mappingReloadUseCancelPrimaryButton->setText(
            QString::fromStdString(
                    _jsdriver->GetButtonLabelForController(asgn.PrimaryDevice, profile.BUTTONSEC[CANCEL])));
    _configform->mappingReloadUseCancelSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[CANCEL])));
    // Start
    _configform->mappingStartPrimaryButton->setText(
            QString::fromStdString(
                    _jsdriver->GetButtonLabelForController(asgn.PrimaryDevice, profile.BUTTONSEC[START])));
    _configform->mappingStartSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[START])));
    // Crouch
    _configform->mappingCrouchPrimaryButton->setText(
            QString::fromStdString(
                    _jsdriver->GetButtonLabelForController(asgn.PrimaryDevice, profile.BUTTONSEC[CROUCH])));
    _configform->mappingCrouchSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[CROUCH])));
    // Kneel
    _configform->mappingPDKneelPrimaryButton->setText(
            QString::fromStdString(
                    _jsdriver->GetButtonLabelForController(asgn.PrimaryDevice, profile.BUTTONSEC[KNEEL])));
    _configform->mappingPDKneelSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[KNEEL])));

    // Forwards
    _configform->mappingForwardPrimaryButton->setText(
            QString::fromStdString(
                    _jsdriver->GetButtonLabelForController(asgn.PrimaryDevice, profile.BUTTONSEC[FORWARDS])));
    _configform->mappingForwardSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[FORWARDS])));
    // Backwards
    _configform->mappingBackwardsPrimaryButton->setText(
            QString::fromStdString(
                    _jsdriver->GetButtonLabelForController(asgn.PrimaryDevice, profile.BUTTONSEC[BACKWARDS])));
    _configform->mappingBackwardsSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[BACKWARDS])));

    // Strafe Left
    _configform->mappingStrafeLeftPrimaryButton->setText(
            QString::fromStdString(
                    _jsdriver->GetButtonLabelForController(asgn.PrimaryDevice, profile.BUTTONSEC[STRAFELEFT])));
    _configform->mappingStrafeLeftSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[STRAFERIGHT])));
    // Strafe Right
    _configform->mappingStrafeRightPrimaryButton->setText(
            QString::fromStdString(
                    _jsdriver->GetButtonLabelForController(asgn.PrimaryDevice, profile.BUTTONSEC[STRAFERIGHT])));
    _configform->mappingStrafeRightSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[STRAFERIGHT])));

    // Analog Up
    _configform->mappingAnalogUpPrimaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice, profile.BUTTONSEC[UP])));
    _configform->mappingAnalogUpSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[UP])));

    // Analog Down
    _configform->mappingAnalogDownPrimaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice, profile.BUTTONSEC[DOWN])));
    _configform->mappingAnalogDownSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[DOWN])));

    // Analog Left
    _configform->mappingAnalogLeftPrimaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,profile.BUTTONSEC[LEFT])));
    _configform->mappingAnalogLeftSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[LEFT])));

    // Analog Right
    _configform->mappingAnalogRightPrimaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,profile.BUTTONSEC[RIGHT])));
    _configform->mappingAnalogRightSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[RIGHT])));

    // Next Weapon
    _configform->mappingNextWeaponPrimaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,profile.BUTTONSEC[NEXTWEAPON])));
    _configform->mappingNextWeaponSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[NEXTWEAPON])));
    // Previous Weapon
    _configform->mappingPreviousWeaponPrimaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,profile.BUTTONSEC[PREVIOUSWEAPON])));
    _configform->mappingPreviousWeaponSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[PREVIOUSWEAPON])));
    // Toggle Gyroscope
    _configform->mappingToggleGyroscopePrimaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,profile.BUTTONSEC[TOGGLEGYRO])));
    _configform->mappingAimSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[TOGGLEGYRO])));

    _configform->mappingCenterAimPrimaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,profile.BUTTONSEC[RESETGYRO])));
    _configform->mappingCenterAimSecondaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,
                                                                          profile.BUTTONSEC[RESETGYRO])));

    _configform->mappingCalibrateGyroscopePrimaryButton->setText(
            QString::fromStdString(_jsdriver->GetButtonLabelForController(asgn.PrimaryDevice,profile.BUTTONSEC[CALIBRATEGYRO])));
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

// insane boilerplate for linking all of the sliders to their respective textboxes...
void ConfigDialog::on_playerSettingsTabGyroXAxisSensitivitySlider_valueChanged(int value) {
    _configform->playerSettingsTabGyroXAxisSensitivitySpinbox->setValue(value / 100.0);
}

void ConfigDialog::on_playerSettingsTabGyroXAxisSensitivitySpinbox_valueChanged(double value) {
    _configform->playerSettingsTabGyroXAxisSensitivitySlider->setValue(value * 100);
}

void ConfigDialog::on_playerSettingsTabGyroYAxisSensitivitySlider_valueChanged(int value) {
    _configform->playerSettingsTabGyroYAxisSensitivitySpinbox->setValue(value / 100.0);
}

void ConfigDialog::on_playerSettingsTabGyroYAxisSensitivitySpinbox_valueChanged(double value) {
    _configform->playerSettingsTabGyroYAxisSensitivitySlider->setValue(value * 100);
}



