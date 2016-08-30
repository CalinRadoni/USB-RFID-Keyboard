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

#include "ledString.h"

#include <FastLED.h>

// ---------------------------------------------------------------------------

LedString ledString;

// ---------------------------------------------------------------------------

LedString::LedString(void)
{
    onTime = 0;
}

void LedString::Initialize(void)
{
    FastLED.addLeds<WS2812B, LS_DATA_PIN, GRB>(leds, LS_NUM_LEDS);
}

// ---------------------------------------------------------------------------

void LedString::PlayHueShow()
{
    byte i, j;
    for(i = 0; i < 0xFF; i++){
        for(j = 0; j < LS_NUM_LEDS; j++)
            leds[j] = CHSV(i, 255, 128);
        FastLED.show();
        delay(10);
    }
}

// ---------------------------------------------------------------------------

void LedString::Blank()
{
    for(byte i = 0; i < LS_NUM_LEDS; i++)
        leds[i] = 0;
    FastLED.show();
}

void LedString::SetColor(uint32_t colorIn)
{
    for(byte i = 0; i < LS_NUM_LEDS; i++)
        leds[i] = colorIn;
    FastLED.show();
}
void LedString::SetColor(uint8_t rr, uint8_t gg, uint8_t bb)
{
    for(byte i = 0; i < LS_NUM_LEDS; i++)
        leds[i].setRGB(rr, gg, bb);
    FastLED.show();
}
void LedString::SetPixelColor(uint8_t ii, uint8_t rr, uint8_t gg, uint8_t bb)
{
    if(ii >= LS_NUM_LEDS) return;

    leds[ii].setRGB(rr, gg, bb);
    FastLED.show();
}
void LedString::SetStringColors(byte* buf)
{
    for(byte i = 0; i < LS_NUM_LEDS; i++)
        leds[i].setRGB(buf[3 * i], buf[3 * i + 1], buf[3 * i + 2]);
    FastLED.show();
}
// ---------------------------------------------------------------------------

void LedString::SetOnTime(uint32_t dIn)
{
    timeS = timeE = millis();
    onTime = dIn;
}

void LedString::Pulse(void)
{
    if(onTime > 0){
        timeE = millis();
        timeE -= timeS;
        if(timeE >= onTime){
            onTime = 0;
            Blank();
        }
    }
}

// ---------------------------------------------------------------------------
