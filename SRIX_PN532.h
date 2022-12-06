#pragma once

#include <Arduino.h>
#include <Wire.h>

#include <stdint.h>
#include <string.h>

#define PN532_GetFirmwareVersion 0x02
#define PN532_SAMConfiguration 0x14
#define PN532_RFConfiguration 0x32
#define PN532_InCommunicateThru 0x42
#define PN532_InListPassiveTarget 0x4A

#define SRIX_Initiate1 0x06
#define SRIX_Initiate2 0x00
#define SRIX_Select 0x0E
#define SRIX_ReadBlock 0x08
#define SRIX_WriteBlock 0x09
#define SRIX_ReadUID 0x0B

#define SRIX_SOF 0x00 // Start Of Frame

#define TIMEOUT 1000

const uint8_t pn532ack[] = { 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00 };

class SRIX_PN532
{

    public:
        SRIX_PN532() {}

        virtual void begin();

        uint32_t getFirmwareVersion();

        bool SAMConfiguration();

        bool ISO14443BInit();

        bool srixInitiate();
        bool readBlock(uint8_t block_buffer[4], const int block);
        bool writeBlock(const uint8_t block_buffer[4], const int block);
        bool readUID(uint8_t block_buffer[8]);

        bool deselectTag();

    private:

    protected:
        uint8_t pn532_buffer[32];

        virtual void sendCommand(const uint8_t * buff, const uint8_t buflen);
        bool sendCommandCheckAck(const uint8_t * cmdbuff, const uint8_t buflen);

        virtual void readData(uint8_t * databuff, const uint8_t datalen);

        virtual bool waitReady(int timeout);

};