# 1964-plugin-joyshock

This is a modified/rewritten C++ version of the Keyboard/Mouse Injector Plugin written by Carnivorous.

This plugin works with the custom, overclocked version of the 1964 Emulator released by Stolen and Carnivorous, which can be found here: [link](http://www.shootersforever.com/forums_message_boards/viewtopic.php?t=7045)

While this entire plugin was effectively rewritten, the code was primarily copied and readapted from Stolen and Carnivorous' work. A majority of the credit for this project goes to them and the ShootersForever community.

The code has some strange, unusual bugs which will be detailed in the Issue tracker, and is still very incomplete. Build and use at your own risk. Pull requests and suggestions to improve the code would be greatly appreciated.

This branch is the JoyShockMapper version and is kept for archival purposes. It will not be updated.

## Features:
* Mouse-style Gyro aiming for the following controllers:
    - Dualshock 4
    - DualSense
    - Switch Pro Controller
    - Joycon Pairs
* Player Identification (Color bar for DS4, Player Lights for SPC/Joycon, both for DS5)    
* Extensive input customization.
* Support for up to 4 players.
* Some Gyro-Exclusive Features:
    - Free Aim (Always-Active Aim Mode - experimental)
    - [Flick Stick](http://gyrowiki.jibbsmart.com/blog:good-gyro-controls-part-2:the-flick-stick)
  
## Potential Future Features
* A less buggy Input library (JoyShockLibrary is great and very easy to work with, but has some issues.)
* Force Feedback (Potentially possible if we can poke the moments the game is supposed to have rumble.)
* Analog Movement (Also requires poking the game's player struct for info.)

## Dependencies & Requirements
This plugin only works on Stolen/Carnivorous' GE/PD 1964 hack. Use the link above to find it.
- MinGW64 (32-bit, x86)
- QT Framework 5.15.2 (preferably statically built)
- JoyShockLibrary (JoyShockLibrary.dll - preferably built from source right now as it has the Wireless DS4 and DS5 support)

Currently, you will have to configure ```cmakelist.txt``` and your build environment yourself.

# Bugs

Bugs are present in the issue tracker.
