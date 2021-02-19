//
// Created by Robin on 2/6/2021.
//

#include "GamepadMotion.hpp"

GamepadMotion::GamepadMotion()
{
    Reset();
}

void GamepadMotion::Reset()
{
    GyroCalibration = {};
    Gyro = {};
    RawAccel = {};
    Motion.Reset();
}

void GamepadMotion::ProcessMotion(float gyroX, float gyroY, float gyroZ,
                                  float accelX, float accelY, float accelZ, float deltaTime)
{
    float accelMagnitude = sqrtf(accelX * accelX + accelY * accelY + accelZ * accelZ);

    if (IsCalibrating)
    {
        PushSensorSamples(gyroX, gyroY, gyroZ, accelMagnitude);
    }

    float gyroOffsetX = 0.0f, gyroOffsetY = 0.0f, gyroOffsetZ = 0.0f;
    GetCalibratedSensor(gyroOffsetX, gyroOffsetY, gyroOffsetZ, accelMagnitude);

    gyroX -= gyroOffsetX;
    gyroY -= gyroOffsetY;
    gyroZ -= gyroOffsetZ;

    Motion.Update(gyroX, gyroY, gyroZ, accelX, accelY, accelZ, accelMagnitude, deltaTime);

    Gyro.x = gyroX;
    Gyro.y = gyroY;
    Gyro.z = gyroZ;
    RawAccel.x = accelX;
    RawAccel.y = accelY;
    RawAccel.z = accelZ;
}

// reading the current state
void GamepadMotion::GetCalibratedGyro(float& x, float& y, float& z)
{
    x = Gyro.x;
    y = Gyro.y;
    z = Gyro.z;
}

void GamepadMotion::GetGravity(float& x, float& y, float& z)
{
    x = Motion.Grav.x;
    y = Motion.Grav.y;
    z = Motion.Grav.z;
}

void GamepadMotion::GetProcessedAcceleration(float& x, float& y, float& z)
{
    x = Motion.Accel.x;
    y = Motion.Accel.y;
    z = Motion.Accel.z;
}

void GamepadMotion::GetOrientation(float& w, float& x, float& y, float& z)
{
    w = Motion.Quaternion.w;
    x = Motion.Quaternion.x;
    y = Motion.Quaternion.y;
    z = Motion.Quaternion.z;
}

// gyro calibration functions
void GamepadMotion::StartContinuousCalibration()
{
    IsCalibrating = true;
}

void GamepadMotion::PauseContinuousCalibration()
{
    IsCalibrating = false;
}

void GamepadMotion::ResetContinuousCalibration()
{
    GyroCalibration = {};
}

void GamepadMotion::GetCalibrationOffset(float& xOffset, float& yOffset, float& zOffset)
{
    float accelMagnitude;
    GetCalibratedSensor(xOffset, yOffset, zOffset, accelMagnitude);
}

void GamepadMotion::SetCalibrationOffset(float xOffset, float yOffset, float zOffset, int weight)
{
    if (GyroCalibration.NumSamples > 1)
    {
        GyroCalibration.AccelMagnitude *= ((float)weight) / GyroCalibration.NumSamples;
    }
    else
    {
        GyroCalibration.AccelMagnitude = weight;
    }

    GyroCalibration.NumSamples = weight;
    GyroCalibration.X = xOffset * weight;
    GyroCalibration.Y = yOffset * weight;
    GyroCalibration.Z = zOffset * weight;
}

void GamepadMotion::ResetMotion()
{
    Motion.Reset();
}

// Private Methods

void GamepadMotion::PushSensorSamples(float gyroX, float gyroY, float gyroZ, float accelMagnitude)
{
    // accumulate
    GyroCalibration.NumSamples++;
    GyroCalibration.X += gyroX;
    GyroCalibration.Y += gyroY;
    GyroCalibration.Z += gyroZ;
    GyroCalibration.AccelMagnitude += accelMagnitude;
}

void GamepadMotion::GetCalibratedSensor(float& gyroOffsetX, float& gyroOffsetY, float& gyroOffsetZ, float& accelMagnitude)
{
    if (GyroCalibration.NumSamples <= 0) {
        gyroOffsetX = 0.f;
        gyroOffsetY = 0.f;
        gyroOffsetZ = 0.f;
        accelMagnitude = 0.f;
        return;
    }

    float inverseSamples = 1.f / GyroCalibration.NumSamples;
    gyroOffsetX = GyroCalibration.X * inverseSamples;
    gyroOffsetY = GyroCalibration.Y * inverseSamples;
    gyroOffsetZ = GyroCalibration.Z * inverseSamples;
    accelMagnitude = GyroCalibration.AccelMagnitude * inverseSamples;
}