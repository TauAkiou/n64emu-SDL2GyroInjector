// Copyright (c) 2020-2021 Julian "Jibb" Smart
// Released under the MIT license. See https://github.com/JibbSmart/GamepadMotionHelpers/blob/main/LICENSE for more info
// Version 6

#pragma once
#define NOMINMAX

#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm> // std::min, std::max and std::clamp

// You don't need to look at these. These will just be used internally by the GamepadMotion class declared below.
// You can ignore anything in namespace GamepadMotionHelpers.
class GamepadMotionSettings;
class GamepadMotion;

namespace GamepadMotionHelpers
{
    struct GyroCalibration
    {
        float X;
        float Y;
        float Z;
        float AccelMagnitude;
        int NumSamples;
    };

    struct Quat
    {
        float w;
        float x;
        float y;
        float z;

        Quat();
        Quat(float inW, float inX, float inY, float inZ);
        void Set(float inW, float inX, float inY, float inZ);
        Quat& operator*=(const Quat& rhs);
        friend Quat operator*(Quat lhs, const Quat& rhs);
        void Normalize();
        Quat Normalized() const;
        void Invert();
        Quat Inverse() const;
    };

    struct Vec
    {
        float x;
        float y;
        float z;

        Vec();
        Vec(float inValue);
        Vec(float inX, float inY, float inZ);
        void Set(float inX, float inY, float inZ);
        float Length() const;
        float LengthSquared() const;
        void Normalize();
        Vec Normalized() const;
        float Dot(const Vec& other) const;
        Vec Cross(const Vec& other) const;
        Vec Min(const Vec& other) const;
        Vec Max(const Vec& other) const;
        Vec Abs() const;
        Vec Lerp(const Vec& other, float factor) const;
        Vec Lerp(const Vec& other, const Vec& factor) const;
        Vec& operator+=(const Vec& rhs);
        friend Vec operator+(Vec lhs, const Vec& rhs);
        Vec& operator-=(const Vec& rhs);
        friend Vec operator-(Vec lhs, const Vec& rhs);
        Vec& operator*=(const float rhs);
        friend Vec operator*(Vec lhs, const float rhs);
        Vec& operator/=(const float rhs);
        friend Vec operator/(Vec lhs, const float rhs);
        Vec& operator*=(const Quat& rhs);
        friend Vec operator*(Vec lhs, const Quat& rhs);
        Vec operator-() const;
    };

    struct SensorMinMaxWindow
    {
        Vec MinGyro;
        Vec MaxGyro;
        Vec MeanGyro;
        Vec MinAccel;
        Vec MaxAccel;
        Vec MeanAccel;
        Vec StartAccel;
        int NumSamples;
        float TimeSampled;

        SensorMinMaxWindow();
        void Reset(float remainder);
        void AddSample(const Vec& inGyro, const Vec& inAccel, float deltaTime);
        Vec GetMidGyro();
    };

    struct AutoCalibration
    {
        SensorMinMaxWindow MinMaxWindow;
        Vec SmoothedAngularVelocityGyro;
        Vec SmoothedAngularVelocityAccel;
        Vec SmoothedPreviousAccel;
        Vec PreviousAccel;

        AutoCalibration();
        bool AddSampleStillness(const Vec& inGyro, const Vec& inAccel, float deltaTime, bool doSensorFusion);
        void NoSampleStillness();
        bool AddSampleSensorFusion(const Vec& inGyro, const Vec& inAccel, float deltaTime);
        void NoSampleSensorFusion();
        void SetCalibrationData(GyroCalibration* calibrationData);
        void SetSettings(GamepadMotionSettings* settings);

    private:
        Vec MinDeltaGyro = Vec(10.f);
        Vec MinDeltaAccel = Vec(10.f);
        float RecalibrateThreshold = 1.f;
        float SensorFusionSkippedTime = 0.f;
        float TimeSteadySensorFusion = 0.f;
        float TimeSteadyStillness = 0.f;

        GyroCalibration* CalibrationData;
        GamepadMotionSettings* Settings;
    };

    struct Motion
    {
        Quat Quaternion;
        Vec Accel;
        Vec Grav;

        Vec SmoothAccel = Vec();
        float Shakiness = 0.f;
        const float ShortSteadinessHalfTime = 0.25f;
        const float LongSteadinessHalfTime = 1.f;

        Motion();
        void Reset();
        void Update(float inGyroX, float inGyroY, float inGyroZ, float inAccelX, float inAccelY, float inAccelZ, float gravityLength, float deltaTime);
        void SetSettings(GamepadMotionSettings* settings);

    private:
        GamepadMotionSettings* Settings;
    };

    enum CalibrationMode
    {
        Manual = 0,
        Stillness = 1,
        SensorFusion = 2,
    };

    // https://stackoverflow.com/a/1448478/1130520
    inline CalibrationMode operator|(CalibrationMode a, CalibrationMode b)
    {
        return static_cast<CalibrationMode>(static_cast<int>(a) | static_cast<int>(b));
    }

    inline CalibrationMode operator&(CalibrationMode a, CalibrationMode b)
    {
        return static_cast<CalibrationMode>(static_cast<int>(a) & static_cast<int>(b));
    }

    inline CalibrationMode operator~(CalibrationMode a)
    {
        return static_cast<CalibrationMode>(~static_cast<int>(a));
    }

    // https://stackoverflow.com/a/23152590/1130520
    inline CalibrationMode& operator|=(CalibrationMode& a, CalibrationMode b)
    {
        return (CalibrationMode&)((int&)(a) |= static_cast<int>(b));
    }

    inline CalibrationMode& operator&=(CalibrationMode& a, CalibrationMode b)
    {
        return (CalibrationMode&)((int&)(a) &= static_cast<int>(b));
    }
}

// Note that I'm using a Y-up coordinate system. This is to follow the convention set by the motion sensors in
// PlayStation controllers, which was what I was using when writing in this. But for the record, Z-up is
// better for most games (XY ground-plane in 3D games simplifies using 2D vectors in navigation, for example).

// Gyro units should be degrees per second. Accelerometer should be g-force (approx. 9.8 m/s^2 = 1 g). If you're using
// radians per second, meters per second squared, etc, conversion should be simple.

class GamepadMotionSettings
{
public:
    int MinStillnessSamples = 10;
    float MinStillnessCollectionTime = 0.5f;
    float MinStillnessCorrectionTime = 2.f;
    float MaxStillnessError = 1.25f;
    float StillnessSampleDeteriorationRate = 0.2f;
    float StillnessErrorClimbRate = 0.1f;
    float StillnessErrorDropOnRecalibrate = 0.1f;
    float StillnessCalibrationEaseInTime = 3.f;
    float StillnessCalibrationHalfTime = 0.1f;

    float StillnessGyroDelta = -1.f;
    float StillnessAccelDelta = -1.f;

    float SensorFusionCalibrationSmoothingStrength = 2.f;
    float SensorFusionAngularAccelerationThreshold = 20.f;
    float SensorFusionCalibrationEaseInTime = 3.f;
    float SensorFusionCalibrationHalfTime = 0.1f;

    float GravityCorrectionShakinessMaxThreshold = 0.4f;
    float GravityCorrectionShakinessMinThreshold = 0.01f;

    float GravityCorrectionStillSpeed = 1.f;
    float GravityCorrectionShakySpeed = 0.1f;

    float GravityCorrectionGyroFactor = 0.1f;
    float GravityCorrectionGyroMinThreshold = 0.05f;
    float GravityCorrectionGyroMaxThreshold = 0.25f;

    float GravityCorrectionMinimumSpeed = 0.01f;
};

class GamepadMotion
{
public:
    GamepadMotion();

    void Reset();

    void ProcessMotion(float gyroX, float gyroY, float gyroZ,
                       float accelX, float accelY, float accelZ, float deltaTime);

    // reading the current state
    void GetCalibratedGyro(float& x, float& y, float& z);
    void GetGravity(float& x, float& y, float& z);
    void GetProcessedAcceleration(float& x, float& y, float& z);
    void GetOrientation(float& w, float& x, float& y, float& z);

    // gyro calibration functions
    void StartContinuousCalibration();
    void PauseContinuousCalibration();
    void ResetContinuousCalibration();
    void GetCalibrationOffset(float& xOffset, float& yOffset, float& zOffset);
    void SetCalibrationOffset(float xOffset, float yOffset, float zOffset, int weight);

    GamepadMotionHelpers::CalibrationMode GetCalibrationMode();
    void SetCalibrationMode(GamepadMotionHelpers::CalibrationMode calibrationMode);

    void ResetMotion();

    GamepadMotionSettings Settings;

private:
    GamepadMotionHelpers::Vec Gyro;
    GamepadMotionHelpers::Vec RawAccel;
    GamepadMotionHelpers::Motion Motion;
    GamepadMotionHelpers::GyroCalibration GyroCalibration;
    GamepadMotionHelpers::AutoCalibration AutoCalibration;
    GamepadMotionHelpers::CalibrationMode CurrentCalibrationMode;

    bool IsCalibrating;
    void PushSensorSamples(float gyroX, float gyroY, float gyroZ, float accelMagnitude);
    void GetCalibratedSensor(float& gyroOffsetX, float& gyroOffsetY, float& gyroOffsetZ, float& accelMagnitude);
};

///////////// Everything below here are just implementation details /////////////

