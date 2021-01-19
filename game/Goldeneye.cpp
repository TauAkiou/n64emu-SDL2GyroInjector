//
// Created by Robin on 1/18/2021.
//

#include "Goldeneye.h"

int Goldeneye::Status() {
    const int ge_camera = _link->ReadInt(GE_camera), ge_page = _link->ReadInt(GE_menupage), ge_pause = _link->ReadInt(GE_pause), ge_exit = _link->ReadInt(GE_exit);
    const float ge_crosshairx = _link->ReadFloat(GE_menux), ge_crosshairy = _link->ReadFloat(GE_menuy);
    return (ge_camera >= 0 && ge_camera <= 10 && ge_page >= -1 && ge_page <= 25 && ge_pause >= 0 && ge_pause <= 1 && ge_exit >= 0 && ge_exit <= 1 && ge_crosshairx >= 20 && ge_crosshairx <= 420 && ge_crosshairy >= 20 && ge_crosshairy <= 310); // if GoldenEye 007 is current game
}

void Goldeneye::Inject() {
/*    if(EMU_ReadInt(GE_menupage) < 1) // hacks can only be injected at boot sequence before code blocks are cached, so inject until the main menu
        GE_InjectHacks();
    const int camera = EMU_ReadInt(GE_camera);
    const int exit = EMU_ReadInt(GE_exit);
    const int pause = EMU_ReadInt(GE_pause);
    const int menupage = EMU_ReadInt(GE_menupage);
    const int tankflag = EMU_ReadInt(GE_tankflag);
    const int mproundend = EMU_ReadInt(GE_matchended);
    for(int player = PLAYER1; player < ALLPLAYERS; player++)
    {
        if(PROFILE[player].SETTINGS[CONFIG] == DISABLED) // bypass disabled players
            continue;
        playerbase[player] = BONDDATA(player);
        const int dead = EMU_ReadInt(playerbase[player] + GE_deathflag);
        const int watch = EMU_ReadInt(playerbase[player] + GE_watch);
        const int aimingflag = EMU_ReadInt(playerbase[player] + GE_aimingflag);
        const int mppausemenu = EMU_ReadInt(playerbase[player] + GE_multipausemenu);
        const int cursoraimingflag = PROFILE[player].SETTINGS[GEAIMMODE] && aimingflag;
        const float fov = EMU_ReadFloat(playerbase[player] + GE_fov);
        const float basefov = fov > 60.0f ? (float)OVERRIDEFOV : 60.0f;
        //const float mouseaccel = PROFILE[player].SETTINGS[ACCELERATION] ? sqrt(DEVICE[player].XPOS * DEVICE[player].XPOS + DEVICE[player].YPOS * DEVICE[player].YPOS) / TICKRATE / 12.0f * PROFILE[player].SETTINGS[ACCELERATION] : 0;
        const float sensitivity_stick_x = PROFILE[player].SETTINGS[STICKSENSITIVITYX] / 40.0f;// * fmax(mouseaccel, 1);
        const float sensitivity_stick_y = PROFILE[player].SETTINGS[STICKSENSITIVITYY] / 40.0f; // * fmax(mouseaccel, 1);
        const float sensitivity_gyro_x = PROFILE[player].SETTINGS[GYROSENSITIVITYX] / 40.0f;
        const float sensitivity_gyro_y = PROFILE[player].SETTINGS[GYROSENSITIVITYY] / 40.0f; // fmax(mouseaccel, 1);

        const float gunsensitivity_stick_x = sensitivity_stick_x * (PROFILE[player].SETTINGS[CROSSHAIR] / 2.5f);
        const float gunsensitivity_stick_y = sensitivity_stick_y * (PROFILE[player].SETTINGS[CROSSHAIR] / 2.5f);
        const float gunsensitivity_gyro_x = sensitivity_gyro_x * (PROFILE[player].SETTINGS[CROSSHAIR] / 2.5f);
        const float gunsensitivity_gyro_y = sensitivity_gyro_y * (PROFILE[player].SETTINGS[CROSSHAIR] / 2.5f);

        float camx = EMU_ReadFloat(playerbase[player] + GE_camx), camy = EMU_ReadFloat(playerbase[player] + GE_camy);
        if(camx >= 0 && camx <= 360 && camy >= -90 && camy <= 90 && fov >= 1 && fov <= FOV_MAX && dead == 0 && watch == 0 && pause == 0 && (camera == 4 || camera == 0) && exit == 1 && menupage == 11 && !mproundend && !mppausemenu) // if safe to inject
        {
            GE_AimMode(player, cursoraimingflag, fov, basefov);
            if(!tankflag) // player is on foot
            {
                GE_Crouch(player); // only allow crouching if player is not in tank
                if(!cursoraimingflag) { // if not aiming (or geaimmode is off)
                    camx += DEVICE[player].RSTICKX / 10.0f * sensitivity_stick_x *
                            (fov / basefov); // regular mouselook calculation
                    camx += DEVICE[player].GYROX / 10.0f * sensitivity_gyro_x * DEVICE[player].DELTATIME * (fov / basefov);
                }
                else
                    camx += aimx[player] * (fov / basefov); // scroll screen with aimx/aimy
                while(camx < 0)
                    camx += 360;
                while(camx >= 360)
                    camx -= 360;
                EMU_WriteFloat(playerbase[player] + GE_camx, camx);
            }
            else // player is in tank
            {
                GE_ResetCrouchToggle(player); // reset crouch toggle if in tank
                float tankx = EMU_ReadFloat(GE_tankxrot);
                if(!cursoraimingflag || EMU_ReadInt(playerbase[player] + GE_currentweapon) == 32) { // if not aiming (or geaimmode is off) or player is driving tank with tank equipped as weapon, then use regular mouselook calculation
                    tankx += DEVICE[player].RSTICKX / 10.0f * sensitivity_stick_x / (360 / TANKXROTATIONLIMIT * 2.5) *
                             (fov / basefov);
                    tankx += DEVICE[player].GYROX / 10.0f * sensitivity_gyro_x / (360 / TANKXROTATIONLIMIT * 2.5) *
                             DEVICE[player].DELTATIME * (fov / basefov);
                }
                else
                    tankx += aimx[player] / (360 / TANKXROTATIONLIMIT * 2.5) * (fov / basefov);
                while(tankx < 0)
                    tankx += TANKXROTATIONLIMIT;
                while(tankx >= TANKXROTATIONLIMIT)
                    tankx -= TANKXROTATIONLIMIT;
                EMU_WriteFloat(GE_tankxrot, tankx);
            }
            if(!cursoraimingflag) {
                camy += (!PROFILE[player].SETTINGS[INVERTPITCH] ? -DEVICE[player].RSTICKY : DEVICE[player].RSTICKY) /
                        10.0f * sensitivity_stick_y * (fov / basefov);
                camy += (!PROFILE[player].SETTINGS[INVERTPITCH] ? -DEVICE[player].GYROY : DEVICE[player].GYROY) /
                        10.0f * sensitivity_gyro_y * DEVICE[player].DELTATIME * (fov / basefov);
            }
            else
                camy += -aimy[player] * (fov / basefov);
            camy = ClampFloat(camy, tankflag ? -20 : -90, 90); // tank limits player from looking down -20
            EMU_WriteFloat(playerbase[player] + GE_camy, camy);
            if(PROFILE[player].SETTINGS[CROSSHAIR] && !cursoraimingflag) // if crosshair movement is enabled and player isn't aiming (don't calculate weapon movement while the player is in aim mode)
            {
                if(!tankflag)
                {
                    float gunx = EMU_ReadFloat(playerbase[player] + GE_gunx), crosshairx = EMU_ReadFloat(playerbase[player] + GE_crosshairx); // after camera x and y have been calculated and injected, calculate the gun/crosshair movement
                    gunx += DEVICE[player].RSTICKX / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_stick_x * (fov / basefov) * 0.019f;
                    gunx += DEVICE[player].GYROX / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_gyro_x * DEVICE[player].DELTATIME * (fov / basefov) * 0.019f;
                    crosshairx += DEVICE[player].RSTICKX / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_stick_x * (fov / 4 / (basefov / 4)) * 0.01912f / RATIOFACTOR;
                    crosshairx += DEVICE[player].GYROX / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_gyro_x * (fov / 4 / (basefov / 4)) * DEVICE[player].DELTATIME * 0.01912f / RATIOFACTOR;
                    if(aimingflag) // emulate cursor moving back to the center
                        gunx /= emuoverclock ? 1.03f : 1.07f, crosshairx /= emuoverclock ? 1.03f : 1.07f;
                    gunx = ClampFloat(gunx, -GUNAIMLIMIT, GUNAIMLIMIT);
                    crosshairx = ClampFloat(crosshairx, -CROSSHAIRLIMIT, CROSSHAIRLIMIT);
                    EMU_WriteFloat(playerbase[player] + GE_gunx, gunx);
                    EMU_WriteFloat(playerbase[player] + GE_crosshairx, crosshairx);
                }
                if((!tankflag && camy > -90 || tankflag && camy > -20) && camy < 90) // only allow player's gun to pitch within a valid range
                {
                    float guny = EMU_ReadFloat(playerbase[player] + GE_guny), crosshairy = EMU_ReadFloat(playerbase[player] + GE_crosshairy);
                    guny += (!PROFILE[player].SETTINGS[INVERTPITCH] ? DEVICE[player].RSTICKY : -DEVICE[player].RSTICKY) / (!aimingflag ? 40.0f : 20.0f) * gunsensitivity_stick_y * (fov / basefov) * 0.025f;
                    guny += (!PROFILE[player].SETTINGS[INVERTPITCH] ? DEVICE[player].GYROY : -DEVICE[player].GYROY) / (!aimingflag ? 40.0f : 20.0f) * gunsensitivity_gyro_y * (fov / basefov) * 0.025f;
                    crosshairy += (!PROFILE[player].SETTINGS[INVERTPITCH] ? DEVICE[player].RSTICKY : -DEVICE[player].RSTICKY) / (!aimingflag ? 40.0f : 20.0f) * gunsensitivity_stick_y * (fov / 4 / (basefov / 4)) * 0.0225f;
                    crosshairy += (!PROFILE[player].SETTINGS[INVERTPITCH] ? DEVICE[player].RSTICKY : -DEVICE[player].RSTICKY) / (!aimingflag ? 40.0f : 20.0f) * gunsensitivity_gyro_y * DEVICE[player].DELTATIME * (fov / 4 / (basefov / 4)) * 0.0225f;
                    if(aimingflag)
                        guny /= emuoverclock ? 1.15f : 1.35f, crosshairy /= emuoverclock ? 1.15f : 1.35f;
                    guny = ClampFloat(guny, -GUNAIMLIMIT, GUNAIMLIMIT);
                    crosshairy = ClampFloat(crosshairy, -CROSSHAIRLIMIT, CROSSHAIRLIMIT);
                    EMU_WriteFloat(playerbase[player] + GE_guny, guny);
                    EMU_WriteFloat(playerbase[player] + GE_crosshairy, crosshairy);
                }
            }
        }
        else if(player == PLAYER1 && menupage != 11 && menupage != 23) // if user is in menu (only player 1 can control menu)
        {
            float menucrosshairx = EMU_ReadFloat(GE_menux), menucrosshairy = EMU_ReadFloat(GE_menuy);
            menucrosshairx += DEVICE[player].RSTICKX / 10.0f * sensitivity_stick_x * 6;
            menucrosshairx += DEVICE[player].GYROX / 10.0f * DEVICE[player].DELTATIME * sensitivity_gyro_x * 6;
            menucrosshairy += DEVICE[player].RSTICKY / 10.0f * sensitivity_stick_y * (400.0f / 290.0f * 6); // y is a little weaker then x in the menu so add more power to make it feel even with x axis
            menucrosshairy += DEVICE[player].GYROY / 10.0f * sensitivity_gyro_y * DEVICE[player].DELTATIME * (400.0f / 290.0f * 6); // y is a little weaker then x in the menu so add more power to make it feel even with x axis
            menucrosshairx = ClampFloat(menucrosshairx, 20, 420);
            menucrosshairy = ClampFloat(menucrosshairy, 20, 310);
            EMU_WriteFloat(GE_menux, menucrosshairx);
            EMU_WriteFloat(GE_menuy, menucrosshairy);
        }
        if(dead || menupage != 11) // if player is dead or in menu, reset crouch toggle
            GE_ResetCrouchToggle(player);
    }
    GE_Controller(); // set controller data
    GE_ResetGyro();
    */
}

void Goldeneye::Quit() {
    for(int player = PLAYER1; player < ALLPLAYERS; player++) {
        playerbase[player] = 0;
        GE_ResetCrouchToggle(player);
    }
}