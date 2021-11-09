//
// Created by ladmin on 11/8/2021.
//

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
        j.at("stickmode").get_to(p.StickMode);
        j.at("ds4color").get_to(p.DS4Color);
        j.at("gyroscopesensitivity").at("x").get_to(p.GyroscopeSensitivity.x);
        j.at("gyroscopesensitivity").at("y").get_to(p.AimStickSensitivity.y);
        j.at("aimsticksensitivity").at("x").get_to(p.AimStickSensitivity.x);
        j.at("aimsticksensitivity").at("y").get_to(p.AimStickSensitivity.y);
        j.at("aimstickdeadzone").at("x").get_to(p.AimstickDeadzone.x);
        j.at("aimstickdeadzone").at("y").get_to(p.AimstickDeadzone.y);
        j.at("movestickdeadzone").at("x").get_to(p.MoveStickDeadzone.x);
        j.at("movestickdeadzone").at("y").get_to(p.MoveStickDeadzone.x);
        j.at("crosshair").get_to(p.Crosshair);
        j.at("gyropitchinverted").get_to(p.GyroPitchInverted);
        j.at("stickpitchinverted").get_to(p.StickPitchInverted);
        j.at("crouchtoggle").get_to(p.CrouchToggle);
        j.at("goldeneyeaimmode").get_to(p.GoldeneyeAimMode);
        j.at("perfectdarkaimmode").get_to(p.PerfectDarkAimMode);
        j.at("freeaim").get_to(p.FreeAiming);
        j.at("aimstick").get_to(p.AimStick);

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
                        {"usesticktoaim", p.UseStickToAim},
                        {"freeaim", p.FreeAiming},
                        {"aimstick", p.AimStick},
                        {"button_primary", p.BUTTONPRIM},
                        {"button_secondary", p.BUTTONSEC} });
    };