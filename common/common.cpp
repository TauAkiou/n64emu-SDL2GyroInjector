#include "common.h"

using nljs = nlohmann::json;

using namespace js_settings;

    void js_settings::to_json(nlohmann::json &j, const EMUSETTINGS &p) {
        j = nljs();

        j = nljs::object({ {"fov_override", p.FovOverride},
                               {"ratio_override", {{"x", p.RatioOverride.x},
                               {"y", p.RatioOverride.y}}},
                               {"show_goldeneye_crosshair", p.GoldeneyeCrosshair} });
    }

    void js_settings::from_json(const nlohmann::json &j, EMUSETTINGS &p) {
        j.at("fov_override").get_to(p.FovOverride);
        j.at("ratio_override").at("x").get_to(p.RatioOverride.x);
        j.at("ratio_override").at("y").get_to(p.RatioOverride.y);
        j.at("show_goldeneye_crosshair").get_to(p.GoldeneyeCrosshair);
    }

    void js_settings::from_json(const nlohmann::json &j, PROFILE &p) {
        p.StickMode = j.value("stickmode", FULLSTICK);
        p.DS4Color = j.value("ds4color", 0);
        //j.at("gyroscopesensitivity").at("x").get_to(p.GyroscopeSensitivity.x);
        p.AimStickSensitivity.x = j.at("gyroscopesensitivity").value("x", 1.0f);
        j.at("gyroscopesensitivity").at("y").get_to(p.AimStickSensitivity.y);
        j.at("aimsticksensitivity").at("x").get_to(p.AimStickSensitivity.x);
        j.at("aimsticksensitivity").at("y").get_to(p.AimStickSensitivity.y);
        j.at("aimstickdeadzone").at("x").get_to(p.AimstickDeadzone.x);
        j.at("aimstickdeadzone").at("y").get_to(p.AimstickDeadzone.y);
        j.at("movestickdeadzone").at("x").get_to(p.MoveStickDeadzone.x);
        j.at("movestickdeadzone").at("y").get_to(p.MoveStickDeadzone.y);
        j.at("crosshair").get_to(p.Crosshair);
        j.at("gyropitchinverted").get_to(p.GyroPitchInverted);
        j.at("stickpitchinverted").get_to(p.StickPitchInverted);
        j.at("crouchtoggle").get_to(p.CrouchToggle);
        j.at("goldeneyeaimmode").get_to(p.GoldeneyeAimMode);
        j.at("perfectdarkaimmode").get_to(p.PerfectDarkAimMode);
        p.AllowStickInAimMode = j.value("allow_stick_l_aiming", false);
        j.at("freeaim").get_to(p.FreeAiming);
        j.at("aimstick").get_to(p.AimStick);
        j.at("gyrospace").get_to(p.GyroscopeSpace);
        j.at("gyrolocalaxis").get_to(p.GyroscopeYAxis);
        j.at("gyro_use_separate_aiming_sensitivity").get_to(p.UseSeperateGyroAimSensitivity);
        j.at("trigger_thresholds").at("lt").get_to(p.TriggerThreshold.x);
        j.at("trigger_thresholds").at("rt").get_to(p.TriggerThreshold.y);
        j.at("gyro_aimmode_sensitivity").at("x").get_to(p.GyroscopeAimSensitivity.x);
        j.at("gyro_aimmode_sensitivity").at("y").get_to(p.GyroscopeAimSensitivity.y);

        // Load our button arrays now.
        auto primarybtn_json = j.at("button_primary");
        auto secondarybtn_json = j.at("button_secondary");

        for(int val = 0; val < TOTALBUTTONS; val++) {
            p.BUTTONPRIM[val] = primarybtn_json[val];
            p.BUTTONSEC[val] = secondarybtn_json[val];
        }
    }

    void js_settings::to_json(nlohmann::json &j, const PROFILE &p) {
        j = nljs::object({ {"stickmode", p.StickMode},
                        {"ds4color", p.DS4Color},
                        {"aimstickdeadzone", {{"x", p.AimstickDeadzone.x}, {"y", p.AimstickDeadzone.y}}},
                        {"movestickdeadzone", {{"x", p.MoveStickDeadzone.x}, {"y", p.MoveStickDeadzone.y}}},
                        {"gyroscopesensitivity", {{"x", p.GyroscopeSensitivity.x}, {"y", p.GyroscopeSensitivity.y}}},
                        {"aimsticksensitivity", {{"x", p.AimStickSensitivity.x}, {"y", p.AimStickSensitivity.y}}},
                        {"crosshair", p.Crosshair},
                        {"gyropitchinverted", p.GyroPitchInverted},
                        {"stickpitchinverted", p.StickPitchInverted},
                        {"crouchtoggle", p.CrouchToggle},
                        {"goldeneyeaimmode", p.GoldeneyeAimMode},
                        {"perfectdarkaimmode", p.PerfectDarkAimMode},
                        {"usesticktoaim", p.AllowStickInAimMode},
                        {"freeaim", p.FreeAiming},
                        {"aimstick", p.AimStick},
                        {"allow_stick_l_aiming", p.AllowStickInAimMode},
                        {"gyrospace", p.GyroscopeSpace},
                        {"gyrolocalaxis", p.GyroscopeYAxis},
                        {"button_primary", p.BUTTONPRIM},
                        {"button_secondary", p.BUTTONSEC},
                        {"gyro_use_separate_aiming_sensitivity", p.UseSeperateGyroAimSensitivity},
                        {"trigger_thresholds", {{"lt", p.TriggerThreshold.x}, {"rt", p.TriggerThreshold.y}}},
                        {"gyro_aimmode_sensitivity", {{"x", p.GyroscopeAimSensitivity.x}, {"y", p.GyroscopeAimSensitivity.y}}}
                });
    };