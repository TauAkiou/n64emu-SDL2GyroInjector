//
// Created by Robin on 1/18/2021.
//

#ifndef INC_1964_INPUT_JOYSHOCKCPP_PERFECTDARK_H
#define INC_1964_INPUT_JOYSHOCKCPP_PERFECTDARK_H

#define GUNAIMLIMIT 14.12940025 // 0x41621206
#define CROSSHAIRLIMIT 18.76135635 // 0x41961742
#define GUNRECOILXLIMIT 756.1247559 // 0x443D07FC
#define GUNRECOILYLIMIT 57.63883972 // 0x42668E2C
#define BIKEXROTATIONLIMIT 6.282184601 // 0x40C907A8
#define BIKEROLLLIMIT 0.7852724195 // 0xBF49079D/0x3F49079D
#define PI 3.1415927 // 0x40490FDB
// PERFECT DARK ADDRESSES - OFFSET ADDRESSES BELOW (REQUIRES PLAYERBASE/BIKEBASE TO USE)
#define PD_stanceflag 0x801BB74C - 0x801BB6A0
#define PD_deathflag 0x801BB778 - 0x801BB6A0
#define PD_camx 0x801BB7E4 - 0x801BB6A0
#define PD_camy 0x801BB7F4 - 0x801BB6A0
#define PD_fov 0x801BCEE8 - 0x801BB6A0
#define PD_crosshairx 0x801BCD08 - 0x801BB6A0
#define PD_crosshairy 0x801BCD0C - 0x801BB6A0
#define PD_grabflag 0x801BB850 - 0x801BB6A0
#define PD_bikeptr 0x801BD10C - 0x801BB6A0
#define PD_thirdperson 0x801BB6A0 - 0x801BB6A0
#define PD_gunrx 0x801BC374 - 0x801BB6A0
#define PD_gunry 0x801BC378 - 0x801BB6A0
#define PD_gunlx 0x801BCB18 - 0x801BB6A0
#define PD_gunly 0x801BCB1C - 0x801BB6A0
#define PD_aimingflag 0x801BB7C0 - 0x801BB6A0
#define PD_gunrstate 0x801BC2DC - 0x801BB6A0
#define PD_gunlstate 0x801BCA80 - 0x801BB6A0
#define PD_gunrxrecoil 0x801BBE98 - 0x801BB6A0
#define PD_gunryrecoil 0x801BBE9C - 0x801BB6A0
#define PD_gunlxrecoil 0x801BC63C - 0x801BB6A0
#define PD_gunlyrecoil 0x801BC640 - 0x801BB6A0
#define PD_currentweapon 0x801BCC28 - 0x801BB6A0
#define PD_bikebase 0x805142C4 - 0x805142C0
#define PD_bikeyaw 0x804B1AB4 - 0x804B1A48
#define PD_bikeroll 0x804B1B04 - 0x804B1A48
// STATIC ADDRESSES BELOW
#define JOANNADATA(X) (unsigned int)EMU_ReadInt(0x8009A024 + (X * 0x4)) // player pointer address (0x4 offset for each players)
#define PD_menu(X) 0x80070750 + (X * 0x4) // player menu flag (0 = PD is in menu) (0x4 offset for each players)
#define PD_camera 0x8009A26C // camera flag (1 = gameplay, 2 & 3 = ???, 4 = multiplayer sweep, 5 = gameover screen, 6 = cutscene mode, 7 = force player to move: extraction's dark room)
#define PD_pause 0x80084014 // menu flag (1 = PD is paused)
#define PD_stageid 0x800624E4 // stage id
#define PD_debugtext 0x803C79E0 // debug text (used to check if PD is running)
#define PD_mppause 0x800ACBA6 // used to check if multiplayer match is paused
#define PD_defaultratio 0x803CD680 // 16:9 ratio default
#define PD_defaultfov 0x802EAA5C // field of view default
#define PD_defaultfovzoom 0x802EACFC // field of view default for zoom
#define PD_defaultzoominspeed 0x802DA8F8 // default zoom in speed
#define PD_defaultzoomoutspeed 0x802DA924 // default zoom out speed
#define PD_introcounter 0x800624C4 // counter for intro
#define PD_controlstyle 0x80372728 // instruction reads the current controller style
#define PD_reversepitch 0x803727A0 // instruction reads the current reverse pitch option
#define PD_pickupyaxisthreshold 0x803CAE78 // y axis threshold on picking up weapons
#define PD_weapontable 0x8006FF1C // weapon pointer table, used to change view model positions
#define PD_radialmenutimer 0x802EA2BC // time instruction for radial menu to appear (15 ticks)
#define PD_radialmenualphainit 0x803D2CDC // initial alpha value for all menus
#define PD_blurfix 0x802DB68C // nop gap on chr function to store our blur fix

#endif //INC_1964_INPUT_JOYSHOCKCPP_PERFECTDARK_H
