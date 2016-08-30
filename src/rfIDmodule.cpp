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

#include "rfIDmodule.h"
#include "simpleTextOutput.h"

#include <SPI.h>
#include <MFRC522.h>

MFRC522 mfrc522(RFID_SS_PIN, RFID_RST_PIN);  // Create MFRC522 instance
MFRC522::MIFARE_Key key;

RFIDModule modRFID;

#define CARD_PRESENT_TIMEOUT 30000

RFIDModule::RFIDModule(void)
{
    readBufLen = 0;
    cardID = 0;
}

void RFIDModule::Initialize(void)
{
    SPI.begin();      // Init SPI bus
    mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
    mfrc522.PCD_Init();   // Init MFRC522
    mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
    // Prepare the key (used both as key A and as key B)
    // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
    for (byte i = 0; i < 6; i++)
        key.keyByte[i] = 0xFF;
}

byte RFIDModule::GetModuleVersion(void)
{
    return mfrc522.PCD_ReadRegister(MFRC522::PCD_Register::VersionReg);
}

void RFIDModule::CheckCardPresence(void)
{
    byte bufferATQA[2];
    byte bufferSize;
    MFRC522::StatusCode result;

    cardID = 0;

    bufferSize = sizeof(bufferATQA);
    result = mfrc522.PICC_RequestA(bufferATQA, &bufferSize); // Look for new cards
    if(result == MFRC522::StatusCode::STATUS_OK || result == MFRC522::StatusCode::STATUS_COLLISION){
        result = mfrc522.PICC_Select(&mfrc522.uid); // Select one of the cards
        if(result == MFRC522::StatusCode::STATUS_OK){
            cardID = mfrc522.uid.uidByte[0];
            cardID = cardID << 8; cardID |= mfrc522.uid.uidByte[1];
            cardID = cardID << 8; cardID |= mfrc522.uid.uidByte[2];
            cardID = cardID << 8; cardID |= mfrc522.uid.uidByte[3];
            sak = mfrc522.uid.sak;

            mfrc522.PICC_HaltA();
            mfrc522.PCD_StopCrypto1();
        }
    }
}

void RFIDModule::ShowExtendedCardInfo(void)
{
    uint32_t timeS, timeE, dt;
    uint16_t blockCnt, blockIdx;

    timeS = timeE = millis(); dt = 0;
    while((!mfrc522.PICC_IsNewCardPresent()) && (dt < CARD_PRESENT_TIMEOUT)){
        timeE = millis();
        dt = timeE - timeS;
    }
    if(dt >= CARD_PRESENT_TIMEOUT){
        simpleTextOutput.FormatAndOutputString("Timeout expired !\r\n");
        return;
    }

    MFRC522::StatusCode result = mfrc522.PICC_Select(&mfrc522.uid);
    if(result != MFRC522::STATUS_OK){
        simpleTextOutput.FormatAndOutputString("Failed to select card (Error is %s) ! Aborting.\r\n", mfrc522.GetStatusCodeName(result));
    }

    // if(!mfrc522.PICC_ReadCardSerial()){
    //     simpleTextOutput.FormatAndOutputString("Failed to read card UID ! Aborting.\r\n");
    //     return;
    // }

    // Show UID
    simpleTextOutput.FormatAndOutputString("Card UID:");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        if(mfrc522.uid.uidByte[i] < 0x10) simpleTextOutput.FormatAndOutputString(" 0");
        else                              simpleTextOutput.FormatAndOutputString(" ");
        simpleTextOutput.FormatAndOutputString("%x", mfrc522.uid.uidByte[i]);
    }
    // Show (suggested) PICC type and Card SAK
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    simpleTextOutput.FormatAndOutputString(" type: %s (SAK: %x)\r\n", mfrc522.PICC_GetTypeName(piccType), mfrc522.uid.sak);

    // Check card type
    switch(piccType){
        case MFRC522::PICC_TYPE_MIFARE_MINI: blockCnt =  20; break;
        case MFRC522::PICC_TYPE_MIFARE_1K  : blockCnt =  64; break;
        case MFRC522::PICC_TYPE_MIFARE_4K  : blockCnt = 256; break;
        default: blockCnt = 0; break;
    }
    if(blockCnt == 0){
        simpleTextOutput.FormatAndOutputString("Not a MIFARE Classic card ! Aborting.\r\n");
        return;
    }

    // display blockCnt blocks
    byte blockID;
    for(blockIdx = 0; blockIdx < blockCnt; blockIdx++){
        blockID = (byte)(blockIdx & 0x00FF);
        if(!ReadAndOutputBlock(blockID)){
            simpleTextOutput.FormatAndOutputString("Failed to read block %d ! Aborting.\r\n", blockID);
            mfrc522.PICC_HaltA();
            mfrc522.PCD_StopCrypto1();
            return;
        }
    }
}

bool RFIDModule::CheckBlockID(byte blockID)
{
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    switch(piccType){
        case MFRC522::PICC_Type::PICC_TYPE_MIFARE_MINI: return (blockID < 20);
        case MFRC522::PICC_Type::PICC_TYPE_MIFARE_1K:   return (blockID < 64);
        case MFRC522::PICC_Type::PICC_TYPE_MIFARE_4K:   return (blockID < 256); // beeing byte is ALLWAYS < 256 :)
        default: return false;
    }
}

bool RFIDModule::IsSectorTrailerBlock(byte blockID)
{
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    switch(piccType){
        case MFRC522::PICC_Type::PICC_TYPE_MIFARE_MINI:
        case MFRC522::PICC_Type::PICC_TYPE_MIFARE_1K:
            return ((blockID & 0x03) == 0x03);
        case MFRC522::PICC_Type::PICC_TYPE_MIFARE_4K:
            if(blockID < 128) return ((blockID & 0x03) == 0x03);
            else              return ((blockID & 0x0F) == 0x0F);
        default: return false;
    }
}

bool RFIDModule::ReadAndOutputBlock(byte blockID)
{
    MFRC522::StatusCode status;
    byte buffer[18];
    byte i, size;

    if(!CheckBlockID(blockID)) return false;

    status = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockID, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) return false;

    size = sizeof(buffer);
    status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(blockID, buffer, &size);
    if (status != MFRC522::STATUS_OK) return false;

    if(blockID < 10) simpleTextOutput.FormatAndOutputString("0");
    simpleTextOutput.FormatAndOutputString("%d:", blockID);
    for(i = 0; i < size; i++){
        if(buffer[i] < 16) simpleTextOutput.FormatAndOutputString(" 0%x", buffer[i]);
        else               simpleTextOutput.FormatAndOutputString(" %x", buffer[i]);
    }
    simpleTextOutput.FormatAndOutputString(" ");
    for(i = 0; i < size; i++){
        if(isprint(buffer[i])) simpleTextOutput.FormatAndOutputString("%c", buffer[i]);
        else                   simpleTextOutput.FormatAndOutputString(".");
    }
    simpleTextOutput.FormatAndOutputString("\r\n");

    return true;
}

bool RFIDModule::WriteABlock(byte blockID, byte *bufIn)
{
    MFRC522::StatusCode status;

    if(!CheckBlockID(blockID)) return false;
    // for protection I do not allow, yet, writting to the sector trailer blocks
    if(IsSectorTrailerBlock(blockID)) return false;

    status = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockID, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) return false;

    status = (MFRC522::StatusCode)mfrc522.MIFARE_Write(blockID, bufIn, 16);
    if (status != MFRC522::STATUS_OK) return false;

    return true;
}

bool RFIDModule::ReadCardData(void)
{
    MFRC522::StatusCode status;
    byte blockID;
    byte buffer[18];
    byte i, size;
    uint16_t idx, len;

    // read block 1
    blockID = 1;
    status = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockID, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) return false;

    size = sizeof(buffer);
    status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(blockID, buffer, &size);
    if (status != MFRC522::STATUS_OK) return false;

    i = buffer[0]; len  = i; len = len << 8;
    i = buffer[1]; len += i;
    if(len > RFID_BUF_LEN) len = RFID_BUF_LEN;
    i = buffer[2]; readColor  = i; readColor = readColor << 8;
    i = buffer[3]; readColor += i; readColor = readColor << 8;
    i = buffer[4]; readColor += i;
    idx = 0;
    for(i = 5; i < 16; i++){
        if(idx < len) readBuffer[idx] = buffer[i];
        else          readBuffer[idx] = 0;
        idx++;
    }

    // read the other blocks
    while(idx < len){
        blockID++;
        if(IsSectorTrailerBlock(blockID))
            blockID++;
        if(!CheckBlockID(blockID)) return false;

        status = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockID, &key, &(mfrc522.uid));
        if (status != MFRC522::STATUS_OK) return false;

        size = sizeof(buffer);
        status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(blockID, buffer, &size);
        if (status != MFRC522::STATUS_OK) return false;

        for(i = 0; i < 16; i++){
            if(idx < len) readBuffer[idx] = buffer[i];
            else
                if(idx < RFID_BUF_LEN)
                    readBuffer[idx] = 0;
            idx++;
        }
    }
    readBufLen = len;

    return true;
}

void RFIDModule::DisplayReadBuffer(void)
{
    simpleTextOutput.FormatAndOutputString("#");
    for(uint16_t i = 0; i < readBufLen; i++){
        if(isprint(readBuffer[i])) simpleTextOutput.FormatAndOutputString("%c",     readBuffer[i]);
        else                       simpleTextOutput.FormatAndOutputString("[0x%x]", readBuffer[i]);
    }
    simpleTextOutput.FormatAndOutputString("#\r\n");
}

bool RFIDModule::WriteCardData(byte *bufIn, uint16_t bufLen, uint32_t colorIn){
    uint32_t timeS, timeE, dt;
    uint16_t blockCnt;

    timeS = timeE = millis(); dt = 0;
    while((!mfrc522.PICC_IsNewCardPresent()) && (dt < CARD_PRESENT_TIMEOUT)){
        timeE = millis();
        dt = timeE - timeS;
    }
    if(dt >= CARD_PRESENT_TIMEOUT){
        simpleTextOutput.FormatAndOutputString("Timeout expired !\r\n");
        return false;
    }

    if(!mfrc522.PICC_ReadCardSerial()){
        simpleTextOutput.FormatAndOutputString("Failed to read card UID ! Aborting.\r\n");
        return false;
    }

    // Show UID
    simpleTextOutput.FormatAndOutputString("Card UID:");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        if(mfrc522.uid.uidByte[i] < 0x10) simpleTextOutput.FormatAndOutputString(" 0");
        else                              simpleTextOutput.FormatAndOutputString(" ");
        simpleTextOutput.FormatAndOutputString("%x", mfrc522.uid.uidByte[i]);
    }
    // Show (suggested) PICC type and Card SAK
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    simpleTextOutput.FormatAndOutputString(" type: %s (SAK: %x)\r\n", mfrc522.PICC_GetTypeName(piccType), mfrc522.uid.sak);

    // Check card type
    switch(piccType){
        case MFRC522::PICC_TYPE_MIFARE_MINI: blockCnt =  20; break;
        case MFRC522::PICC_TYPE_MIFARE_1K  : blockCnt =  64; break;
        case MFRC522::PICC_TYPE_MIFARE_4K  : blockCnt = 256; break;
        default: blockCnt = 0; break;
    }
    if(blockCnt == 0){
        simpleTextOutput.FormatAndOutputString("Not a MIFARE Classic card ! Aborting.\r\n");
        return false;
    }

    if(!ReadCardData()){
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();
        return false;
    }
    simpleTextOutput.FormatAndOutputString("Read color: %x\r\nRead data: ", readColor);
    DisplayReadBuffer();

    byte blockID;
    uint8_t  buffer[16], i;
    uint16_t idx;

    idx = 0;
    blockID = 1;

    // prepare and write block 1
    i = (uint8_t)(bufLen >> 8);            buffer[0] = i;
    i = (uint8_t)(bufLen & 0xFF);          buffer[1] = i;
    i = (uint8_t)((colorIn >> 16) & 0xFF); buffer[2] = i;
    i = (uint8_t)((colorIn >>  8) & 0xFF); buffer[3] = i;
    i = (uint8_t)( colorIn        & 0xFF); buffer[4] = i;
    for(i = 5; i < 16; i++){
        if(idx < bufLen) buffer[i] = bufIn[idx++];
        else             buffer[i] = 0x00;
    }
    if(!WriteABlock(blockID, buffer)){
        simpleTextOutput.FormatAndOutputString("Failed to write block %d ! Aborting.\r\n", blockID);
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();
        return false;
    }

    // prepare and write remaining data
    while(idx < bufLen){
        blockID++;
        if(IsSectorTrailerBlock(blockID))
            blockID++;
        if(!CheckBlockID(blockID)){
            simpleTextOutput.FormatAndOutputString("Not enough card memory ! Aborting.\r\n");
            mfrc522.PICC_HaltA();
            mfrc522.PCD_StopCrypto1();
            return false;
        }

        for(i = 0; i < 16; i++){
            if(idx < bufLen) buffer[i] = bufIn[idx++];
            else             buffer[i] = 0x00;
        }
        if(!WriteABlock(blockID, buffer)){
            simpleTextOutput.FormatAndOutputString("Failed to write block %d ! Aborting.\r\n", blockID);
            mfrc522.PICC_HaltA();
            mfrc522.PCD_StopCrypto1();
            return false;
        }

        // blockID overflow protection
        if(blockID == 0xFF) idx = bufLen;
    }

    // no errors. Done.
    simpleTextOutput.FormatAndOutputString("Write OK, %d blocks used.\r\n", blockID);

    if(!ReadCardData()){
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();
        return false;
    }
    simpleTextOutput.FormatAndOutputString("Read color: %x\r\nRead data: ", readColor);
    DisplayReadBuffer();
    simpleTextOutput.FormatAndOutputString("\r\n\r\n");

    mfrc522.PICC_HaltA(); // Halt PICC
    mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
    return true;
}

bool RFIDModule::ReadCard(void)
{
    if(!mfrc522.PICC_IsNewCardPresent()) return false;

    if(!mfrc522.PICC_ReadCardSerial()) return false;

    cardID = mfrc522.uid.uidByte[0];
    cardID = cardID << 8; cardID |= mfrc522.uid.uidByte[1];
    cardID = cardID << 8; cardID |= mfrc522.uid.uidByte[2];
    cardID = cardID << 8; cardID |= mfrc522.uid.uidByte[3];
    sak = mfrc522.uid.sak;

    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    // Check card type
    switch(piccType){
        case MFRC522::PICC_TYPE_MIFARE_MINI:
        case MFRC522::PICC_TYPE_MIFARE_1K  :
        case MFRC522::PICC_TYPE_MIFARE_4K  :
            break;
        default:
            mfrc522.PICC_HaltA();
            mfrc522.PCD_StopCrypto1();
            return false;
            break;
    }

    if(!ReadCardData()){
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();
        return false;
    }

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    return true;
}
