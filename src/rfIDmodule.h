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

#ifndef RFIDmoduleH
#define RFIDmoduleH

#include "defines.h"

#define RFID_RST_PIN  9 // Configurable, see typical pin layout above
#define RFID_SS_PIN  10 // Configurable, see typical pin layout above

#define RFID_BUF_LEN 250

class RFIDModule{
protected:
    bool CheckBlockID(byte);
    bool IsSectorTrailerBlock(byte);
    bool ReadAndOutputBlock(byte);
    bool WriteABlock(byte, byte*);

    bool ReadCardData(void);
    void DisplayReadBuffer(void);

public:
    RFIDModule(void);

    void Initialize(void);
    byte GetModuleVersion(void);

    uint32_t cardID;
    uint8_t  sak;
    void CheckCardPresence(void);

    void ShowExtendedCardInfo(void);

    bool WriteCardData(byte*, uint16_t, uint32_t);

    bool ReadCard(void);
    uint8_t  readBuffer[RFID_BUF_LEN];
    uint16_t readBufLen;
    uint32_t readColor;
};

extern RFIDModule modRFID;

/*
MIFARE Classic Mini: 224 bytes  -  5 sectors
MIFARE Classic 1k: 752 bytes    - 16 sectors
MIFARE Classic 4k: 3440 bytes   - 32 sectors + 8 quadruple sectors

the very first 16 bytes contain the serial number of the card and certain other manufacturer data and are read only
16 bytes/sector reserved for the keys and access conditions

Sector 0:
    - block 0: READ-ONLY
        - byte 0 - 3: UID
        - byte 4: Check byte = byte0 ^ byte1 ^ byte2 ^ byte3
        - byte 5 - 15: manufacturer data
    - block 1 and 2: data
    - block 3: sector trailer
Sector !0:
    - block 0 - 2: data
    - block 3: sector trailer

Block 3 (sector trailer) of each sector:
    - bytes 0 - 5: key A
    - bytes 6 - 9: access conditions
    - bytes 10 - 15: key B

Data blocks: 1,2 / 4,5,6 / 8,9,10 / 12,13,14 / 16,17,18 / ...
Sector trailers: 3 / 7 / 11 / 15 / 19 / ...
*/
#endif
