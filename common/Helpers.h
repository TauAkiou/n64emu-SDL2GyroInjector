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

/*
 * helper.h - Common static helper functions that perform frequently used operations.
 */


#ifndef INC_1964_INPUT_JOYSHOCKCPP_HELPERS_H
#define INC_1964_INPUT_JOYSHOCKCPP_HELPERS_H

class PluginHelpers {
    public:
        static float ClampFloat(float value, float min, float max);
        static int ClampInt(int value, int min, int max);

    // https://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c/10133700

    template <typename T> static int sign(T val) {
            return (val > T(0)) - (val < T(0));
        }
};

#endif //INC_1964_INPUT_JOYSHOCKCPP_HELPERS_H
