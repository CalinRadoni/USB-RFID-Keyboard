/* Copyright (C) 2016 Calin Radoni

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LedStringH
#define LedStringH

#include "defines.h"

#include <FastLED.h>

#define LS_NUM_LEDS 3
#define LS_DATA_PIN 0

class LedString {
private:
    CRGB leds[LS_NUM_LEDS];

    uint32_t timeS, timeE, onTime;

public:
    LedString(void);

    void Initialize(void);

    void Blank(void);
    void PlayHueShow(void);

    void SetColor(uint32_t);
    void SetColor(uint8_t, uint8_t, uint8_t);
    void SetPixelColor(uint8_t, uint8_t, uint8_t, uint8_t);
    void SetStringColors(byte*);

    void SetOnTime(uint32_t);
    void Pulse(void);
};

extern LedString ledString;

#endif
