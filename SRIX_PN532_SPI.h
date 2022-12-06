#pragma once

#include "SRIX_PN532.h"

#include <stdint.h>
#include <SPI.h>

#define PN532_SPI_WRITE 0x01
#define PN532_SPI_READ 0x03
#define PN532_SPI_READY 0x01
#define PN532_SPI_GETSTATUS 0x02

#define PN532_PREAMBLE 0x00
#define PN532_STARTCODE1 0x00
#define PN532_STARTCODE2 0xFF
#define PN532_HOSTTOPN532 0xD4
#define PN532_PN532TOHOST 0xD5
#define PN532_POSTAMBLE 0x00

#define SPI_DELAY 1
#define TIMEOUT 1000

class SRIX_PN532_SPI : public SRIX_PN532
{

    public:
        SRIX_PN532_SPI(uint8_t cs);

        void begin() override;
    
    private:
        uint8_t _cs;

        void sendCommand(const uint8_t * buff, const uint8_t buflen) override;
        void readData(uint8_t * databuff, const uint8_t datalen) override;

        bool waitReady(int timeout) override;

    protected:

};
