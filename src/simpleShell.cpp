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

#include "simpleShell.h"
#include "rfIDmodule.h"
#include "ledString.h"

// ---------------------------------------------------------------------------

SimpleShell simpleShell;

// ---------------------------------------------------------------------------

SimpleShell::SimpleShell(void)
{
    bufLen = 0;
}

// ---------------------------------------------------------------------------

byte SimpleShell::Hex2Bin(byte hh, byte ll)
{
    byte val;

    val = 0;
    if(hh >= '0' && hh <= '9') val =      hh - '0';
    if(hh >= 'A' && hh <= 'F') val = 10 + hh - 'A';
    if(hh >= 'a' && hh <= 'f') val = 10 + hh - 'a';
    val = val << 4;
    if(ll >= '0' && ll <= '9') val |=      ll - '0';
    if(ll >= 'A' && ll <= 'F') val |= 10 + ll - 'A';
    if(ll >= 'a' && ll <= 'f') val |= 10 + ll - 'a';
    return val;
}
uint32_t SimpleShell::HexColor2Bin(byte *buf)
{
    uint32_t res;
    byte val;

    val = Hex2Bin(buf[0], buf[1]); res  = val; res = res << 8;
    val = Hex2Bin(buf[2], buf[3]); res += val; res = res << 8;
    val = Hex2Bin(buf[4], buf[5]); res += val;
    return res;
}
uint16_t SimpleShell::Dec2Bin(byte *buf, byte len)
{
    byte i, val;
    uint16_t res;

    res = 0;
    for(i = 0; i < len; i++){
        val = buf[i];
        if((val >= '0') && (val <= '9')) val -= '0';
        else val = 0;
        res *= 10;
        res += val;
    }
    return res;
}

// ---------------------------------------------------------------------------

void SimpleShell::CleanupBuffer(void)
{
    bool done;
    uint16_t i;

    done = false;
    while(!done){
        if(bufLen == 0) done = true;
        else{
            // shift buffer left
            bufLen--;
            i = 0;
            while(i < bufLen){
                buffer[i] = buffer[i + 1];
                i++;
            }

            if(CheckHeader()) done = true;
        }
    }
}

void SimpleShell::ShiftAndCleanBuffer(uint16_t len)
{
    uint16_t i, cnt;

    if(bufLen <= len){
        bufLen = 0;
        return;
    }

    cnt = bufLen - len;
    for(i = 0; i < cnt; i++)
        buffer[i] = buffer[i + len];
}

bool SimpleShell::CheckHeader(void)
{
    switch(bufLen){
        case  0: return true;
        case  1: return  (buffer[0] == SS_HeaderID_0);
        case  2: return ((buffer[0] == SS_HeaderID_0) && (buffer[1] == SS_HeaderID_1));
        default: return ((buffer[0] == SS_HeaderID_0) && (buffer[1] == SS_HeaderID_1) && (buffer[2] == SS_HeaderID_2));
    }
}

void SimpleShell::AddData(byte data)
{
    if(bufLen < SS_BUFFER_LEN){
        buffer[bufLen] = data;
        bufLen++;

        if(bufLen == SS_HeaderLen){
            if(!CheckHeader())
                CleanupBuffer();
        }
    }
    else{
        CleanupBuffer();
    }
}

void SimpleShell::ProcessData(void)
{
    uint16_t dataLen, reqLen, fullLen;
    uint32_t colorIn;

    if(bufLen <= SS_HeaderLen) return;
    if(!CheckHeader()){
        CleanupBuffer();
        return;
    }

    dataLen = bufLen - SS_HeaderLen - 1;

    switch(buffer[SS_HeaderLen]){
        case SS_Cmd_Ver:        // NO dara
            CmdVersion();
            ShiftAndCleanBuffer(SS_HeaderLen + 1);
            break;
        case SS_Cmd_GetCardInfo:    // NO data
            simpleTextOutput.FormatAndOutputString("Show me a card\r\n");
            modRFID.ShowExtendedCardInfo();
            ShiftAndCleanBuffer(SS_HeaderLen + 1);
            break;
        case SS_Cmd_WriteCard:  // 6 bytes color(rrggbb[hex]) + 3 bytes length[dec] + data
            if(dataLen > 8){
                colorIn = HexColor2Bin(&buffer[SS_HeaderLen + 1]);
                reqLen = Dec2Bin(&buffer[SS_HeaderLen + 7], 3);
                fullLen = SS_HeaderLen + 1 + 9 + reqLen;
                if(fullLen >= SS_BUFFER_LEN){
                    simpleTextOutput.FormatAndOutputString("Maximum usable length is %d !\r\n", reqLen);
                    ShiftAndCleanBuffer(bufLen);
                }
                else{
                    if(dataLen >= (reqLen + 9)){
                        simpleTextOutput.FormatAndOutputString("\r\nReceived %d bytes: ", reqLen);
                        simpleTextOutput.OutputBuffer(&buffer[SS_HeaderLen + 10], reqLen < 0xFE ? reqLen : 0xFE);
                        simpleTextOutput.FormatAndOutputString("\r\nCard color: %x\r\n", colorIn);
                        simpleTextOutput.FormatAndOutputString("Show me a card\r\n");
                        modRFID.WriteCardData(&buffer[SS_HeaderLen + 10], reqLen, colorIn);
                        ShiftAndCleanBuffer(SS_HeaderLen + 1 + 9 + reqLen);
                    }
                }
            }
            break;

        case SS_CmdFullColorA:  // data length 6 bytes (rrggbb[HEX])
            if(dataLen >= 6){
                CmdSetColorA();
                ShiftAndCleanBuffer(SS_HeaderLen + 1 + 6);
            }
            break;
        case SS_CmdFullColor:   // data length 3 bytes (rgb)
            if(dataLen >= 3){
                ledString.SetColor(buffer[SS_HeaderLen + 1], buffer[SS_HeaderLen + 2], buffer[SS_HeaderLen + 3]);
                ShiftAndCleanBuffer(SS_HeaderLen + 1 + 3);
            }
            break;
        case SS_CmdSetPixelA:   // data length 8 bytes (pixelId[HEX] and rrggbb[HEX])
            if(dataLen >= 8){
                CmdSetPixelColorA();
                ShiftAndCleanBuffer(SS_HeaderLen + 1 + 8);
            }
            break;
        case SS_CmdSetPixel:    // data length 4 bytes (pixelId and rgb)
            if(dataLen >= 4){
                CmdSetPixelColorA();
                ShiftAndCleanBuffer(SS_HeaderLen + 1 + 4);
            }
            break;
        case SS_CmdSetFrame:    // data length LS_NUM_LEDS * 3 bytes
            if(dataLen >= 9){
                ledString.SetStringColors(&buffer[SS_HeaderLen + 1]);
                ShiftAndCleanBuffer(SS_HeaderLen + 1 + 9);
            }
            break;

        default:                // unknown/unimplemented command
            ShiftAndCleanBuffer(SS_HeaderLen + 1);
            break;
    }
}

// ---------------------------------------------------------------------------

void SimpleShell::CmdVersion(void)
{
    byte val;

    simpleTextOutput.FormatAndOutputString("\r\nPC Bling Controller firmware version %d.%d\r\n", VER_HI, VER_LO);
    simpleTextOutput.FormatAndOutputString("Build for Teensy LC with Teensyduino and PlatformIO (CLI 2.9.1, IDE 1.2.2)\r\n");
	val = modRFID.GetModuleVersion();
    if(val==0x00 || val==0xFF) simpleTextOutput.FormatAndOutputString("MFRC522 communication failure!\r\n");
    else{
        simpleTextOutput.FormatAndOutputString("MFRC522 firmware version ");
    	switch(val) {
    		case 0x88: simpleTextOutput.FormatAndOutputString("- clone"); break;
    		case 0x90: simpleTextOutput.FormatAndOutputString("0.0"); break;
    		case 0x91: simpleTextOutput.FormatAndOutputString("1.0"); break;
    		case 0x92: simpleTextOutput.FormatAndOutputString("2.0"); break;
    		default:   simpleTextOutput.FormatAndOutputString("- unknown, 0x%x", val); break;
    	}
        simpleTextOutput.FormatAndOutputString("\r\n");
    }
    simpleTextOutput.FormatAndOutputString("MFRC522 library v.1.1.8\r\n");
    simpleTextOutput.FormatAndOutputString("%d WS2812 LEDs connected to pin %d\r\n", LS_NUM_LEDS, LS_DATA_PIN);
    simpleTextOutput.FormatAndOutputString("FastLED library v.%d\r\n\r\n", FASTLED_VERSION);
    simpleTextOutput.Flush();
}

// ---------------------------------------------------------------------------

void SimpleShell::CmdSetColorA(void)
{
    byte rr, gg, bb;

    rr = Hex2Bin(buffer[SS_HeaderLen + 1], buffer[SS_HeaderLen + 2]);
    gg = Hex2Bin(buffer[SS_HeaderLen + 3], buffer[SS_HeaderLen + 4]);
    bb = Hex2Bin(buffer[SS_HeaderLen + 5], buffer[SS_HeaderLen + 6]);

    ledString.SetColor(rr, gg, bb);
}

void SimpleShell::CmdSetPixelColorA(void)
{
    byte ii, rr, gg, bb;

    ii = Hex2Bin(buffer[SS_HeaderLen + 1], buffer[SS_HeaderLen + 2]);
    rr = Hex2Bin(buffer[SS_HeaderLen + 3], buffer[SS_HeaderLen + 4]);
    gg = Hex2Bin(buffer[SS_HeaderLen + 5], buffer[SS_HeaderLen + 6]);
    bb = Hex2Bin(buffer[SS_HeaderLen + 7], buffer[SS_HeaderLen + 8]);

    ledString.SetPixelColor(ii, rr, gg, bb);
}

// ---------------------------------------------------------------------------
