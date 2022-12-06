#include "SRIX_PN532.h"

#define DEBUG

bool SRIX_PN532::SAMConfiguration()
{
    pn532_buffer[0] = PN532_SAMConfiguration;
    pn532_buffer[1] = 0x01; // Normal mode, no SAM used
    pn532_buffer[2] = 0x00; // No timeout
    pn532_buffer[3] = 0x01; // Use IRQ pin

    if (!sendCommandCheckAck(pn532_buffer, 4))
    {
        #ifdef DEBUG
        Serial.println("[!] ERROR : Failed to set PN532 to SAM configuration!");
        #endif
        return false;
    }

    return true;
}

bool SRIX_PN532::ISO14443BInit()
{

    // Set all retries to 0 because we need only REQB
    pn532_buffer[0] = PN532_RFConfiguration;
    pn532_buffer[1] = 0x05; // Config item (max retries)
    pn532_buffer[2] = 0x00; // MxRtyATR
    pn532_buffer[3] = 0x00; // MxRtyPSL
    pn532_buffer[4] = 0x00; // MxRtyPassiveActivation

    if (!sendCommandCheckAck(pn532_buffer, 5))
    {
        #ifdef DEBUG
        Serial.println("[!] ERROR : Failed to set retries!");
        #endif
        return false;
    }

    // Set ISO14443-3B Modulation
    pn532_buffer[0] = PN532_InListPassiveTarget;
    pn532_buffer[1] = 0x01; // Max target
    pn532_buffer[2] = 0x03; // Modulation type
    pn532_buffer[3] = 0x00; // Timeslot approach

    if (!sendCommandCheckAck(pn532_buffer, 4))
    {
        #ifdef DEBUG
        Serial.println("[!] ERROR : Failed to set ISO14443-3B modulation!");
        #endif
        return false;
    }

    return true;
}

uint32_t SRIX_PN532::getFirmwareVersion()
{
    pn532_buffer[0] = PN532_GetFirmwareVersion;

    if (!sendCommandCheckAck(pn532_buffer, 1))
    {
        #ifdef DEBUG
        Serial.println("[!] ERROR : Failed to get firmware version!");
        #endif
        return 0;
    }

    readData(pn532_buffer, 12);

    uint32_t version;
    version = pn532_buffer[7]; // IC
    version <<= 8;
    version |= pn532_buffer[8]; // Version
    version <<= 8;
    version |= pn532_buffer[9]; // Revision
    version <<= 8;
    version |= pn532_buffer[10]; // Support

    return version;
}

bool SRIX_PN532::sendCommandCheckAck(const uint8_t * cmdbuff, const uint8_t buflen)
{
    sendCommand(cmdbuff, buflen);

    uint8_t ack[6];
    readData(ack, 6);

    // Check integrity of ack response
    if (memcmp(ack, pn532ack, 6) != 0)
        return false;

    return true;
}

// SRIX FUNCTIONS

bool SRIX_PN532::srixInitiate()
{
    
    pn532_buffer[0] = PN532_InCommunicateThru;
    pn532_buffer[1] = SRIX_Initiate1;
    pn532_buffer[2] = SRIX_Initiate2;

    if (!sendCommandCheckAck(pn532_buffer, 3))
    {
        #ifdef DEBUG
        Serial.println("[!] ERROR : Init srix4k failed!");
        #endif
        return false;
    }

    readData(pn532_buffer, 12);

    if (pn532_buffer[7] != SRIX_SOF)
    {
        #ifdef DEBUG
        Serial.println("[!] ERROR : SOF missing in initiate response!");
        #endif
        return false;
    }

    uint8_t chip_id = pn532_buffer[8];

    pn532_buffer[0] = PN532_InCommunicateThru;
    pn532_buffer[1] = SRIX_Select;
    pn532_buffer[2] = chip_id;

    if (!sendCommandCheckAck(pn532_buffer, 3))
    {
        #ifdef DEBUG
        Serial.println("[!] ERROR : Select srix4k failed!");
        #endif
        return false;
    }
    
    readData(pn532_buffer, 12);

    if (pn532_buffer[7] != SRIX_SOF)
    {
        #ifdef DEBUG
        Serial.println("[!] ERROR : SOF missing in select response!");
        #endif
        return false;
    }

    return true;
}

bool SRIX_PN532::readBlock(uint8_t block_buffer[4], const int block)
{

    pn532_buffer[0] = PN532_InCommunicateThru;
    pn532_buffer[1] = SRIX_ReadBlock;
    pn532_buffer[2] = (uint8_t)block;

    if (!sendCommandCheckAck(pn532_buffer, 3))
    {
        #ifdef DEBUG
        Serial.println("[!] ERROR : Command read block failed!");
        #endif
        return false;
    }

    readData(pn532_buffer, 14);

    if (pn532_buffer[7] != SRIX_SOF)
    {
        #ifdef DEBUG
        Serial.println("[!] ERROR : SOF missing in read block response!");
        #endif
        return false;
    }

    block_buffer[0] = pn532_buffer[8];
    block_buffer[1] = pn532_buffer[9];
    block_buffer[2] = pn532_buffer[10];
    block_buffer[3] = pn532_buffer[11];

    return true;
}

bool SRIX_PN532::writeBlock(const uint8_t block_buffer[4], const int block)
{

    pn532_buffer[0] = PN532_InCommunicateThru;
    pn532_buffer[1] = SRIX_WriteBlock;
    pn532_buffer[2] = (uint8_t)block;
    pn532_buffer[3] = block_buffer[0];
    pn532_buffer[4] = block_buffer[1];
    pn532_buffer[5] = block_buffer[2];
    pn532_buffer[6] = block_buffer[3];

    if (!sendCommandCheckAck(pn532_buffer, 7))
    {
        #ifdef DEBUG
        Serial.println("[!] ERROR : Command write block failed!");
        #endif
        return false;
    }

    // Tprog from datasheet
    delay(2);

    return true;
}

bool SRIX_PN532::readUID(uint8_t block_buffer[8])
{

    pn532_buffer[0] = PN532_InCommunicateThru;
    pn532_buffer[1] = SRIX_ReadUID;

    if (!sendCommandCheckAck(pn532_buffer, 2))
    {
        #ifdef DEBUG
        Serial.println("[!] ERROR : Command read uid failed!");
        #endif
        return false;
    }

    readData(pn532_buffer, 18);

    if (pn532_buffer[7] != SRIX_SOF)
    {
        #ifdef DEBUG
        Serial.println("[!] ERROR : SOF missing in read uid response!");
        #endif
        return false;
    }

    block_buffer[7] = pn532_buffer[8];
    block_buffer[6] = pn532_buffer[9];
    block_buffer[5] = pn532_buffer[10];
    block_buffer[4] = pn532_buffer[11];
    block_buffer[3] = pn532_buffer[12];
    block_buffer[2] = pn532_buffer[13];
    block_buffer[1] = pn532_buffer[14];
    block_buffer[0] = pn532_buffer[15];

    return true;
}

bool SRIX_PN532::deselectTag()
{
    // RF field off
    pn532_buffer[0] = PN532_RFConfiguration;
    pn532_buffer[1] = 0x01; // Config item (Field)
    pn532_buffer[2] = 0x00; // Switch off

    if (!sendCommandCheckAck(pn532_buffer, 3))
    {
        #ifdef DEBUG
        Serial.println("[!] ERROR : Failed to deactivate the RF field!");
        #endif
        return false;
    }

    // This delay i dont know why is required, so see DATASHEETRRRGIREOGBB
    delay(10);

    // RF field on
    pn532_buffer[0] = PN532_RFConfiguration;
    pn532_buffer[1] = 0x01; // Config item (Field)
    pn532_buffer[2] = 0x01; // Switch on

    if (!sendCommandCheckAck(pn532_buffer, 3))
    {
        #ifdef DEBUG
        Serial.println("[!] ERROR : Failed to activate the RF field!");
        #endif
        return false;
    }

    return true;
}
