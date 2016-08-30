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

#include "defines.h"

#include "ledString.h"
#include "SimpleShell.h"
#include "rfIDmodule.h"

void setup() {
    Keyboard.begin();

    Serial.begin(115200);   // Initialize serial communications with the PC

    modRFID.Initialize();

    ledString.Initialize();
    ledString.PlayHueShow();
    ledString.Blank();
}

void SendKeyboardKey(uint16_t key){
    Keyboard.set_modifier(0);
    Keyboard.set_key1(key);
    Keyboard.send_now();
    delay(50);
    Keyboard.set_key1(0);
    Keyboard.send_now();
    delay(50);
}

void SendKeyboardCombinationWINr(void){
    Keyboard.set_modifier((uint8_t)(MODIFIERKEY_GUI & 0xFF));
    Keyboard.set_key1((uint8_t)(KEY_R & 0xFF));
    Keyboard.send_now();
    delay(50);
    Keyboard.set_modifier(0);
    Keyboard.set_key1(0);
    Keyboard.send_now();
    delay(50);
}

void loop() {
    bool escapeChar;
    byte ch;

    ledString.Pulse();

    if(Serial.available()){
        simpleShell.AddData(Serial.read());
        simpleShell.ProcessData();
    }

    if(modRFID.ReadCard()){
        ledString.SetColor(modRFID.readColor);
        ledString.SetOnTime(2000);
        if(modRFID.readBufLen > 0){
            escapeChar = false;
            for(uint16_t i = 0; i < modRFID.readBufLen; i++){
                ch = modRFID.readBuffer[i];
                if(escapeChar){
                    switch(ch){
                        case '#':  Keyboard.write('#'); break;
                        case 't':  SendKeyboardKey(KEY_TAB); break;
                        case 'n':  SendKeyboardKey(KEY_ENTER); break;
                        case 'e':  SendKeyboardKey(KEY_ESC); break;
                        case 'U':  SendKeyboardKey(KEY_UP); break;
                        case 'D':  SendKeyboardKey(KEY_DOWN); break;
                        case 'L':  SendKeyboardKey(KEY_LEFT); break;
                        case 'R':  SendKeyboardKey(KEY_RIGHT); break;
                        case 'x':  SendKeyboardCombinationWINr(); break;
                        case '0':  delay(50); break;
                        case '1':  delay(100); break;
                        case '2':  delay(200); break;
                        case '3':  delay(300); break;
                        case '4':  delay(400); break;
                        case '5':  delay(500); break;
                        case '6':  delay(600); break;
                        case '7':  delay(700); break;
                        case '8':  delay(800); break;
                        case '9':  delay(900); break;
                        default:   SendKeyboardKey(KEY_PERIOD); break;
                    }
                    escapeChar = false;
                }
                else{
                    if(ch == '#') escapeChar = true;
                    else Keyboard.write(ch);
                }
            }
            modRFID.readBufLen = 0;
        }
    }
}
