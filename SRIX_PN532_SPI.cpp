#include "SRIX_PN532_SPI.h"

#define DEBUG

SRIX_PN532_SPI::SRIX_PN532_SPI(uint8_t cs)
{
    _cs = cs;
}

void SRIX_PN532_SPI::begin()
{
    SPI.begin();

    pinMode(_cs, OUTPUT); // Per sta linea non andava SPI sul ESP
    digitalWrite(_cs, HIGH);

    // Get PN532 synched
    getFirmwareVersion();

    delay(100);
}

bool SRIX_PN532_SPI::waitReady(int timeout)
{
    unsigned long start = millis();

    while(millis() - start < timeout)
    {
        digitalWrite(_cs, LOW);
        delay(SPI_DELAY);
        
        // Check if pn532 is ready
        if (SPI.transfer(PN532_SPI_GETSTATUS) == PN532_SPI_READY) 
        {
            digitalWrite(_cs, HIGH);
            return true;
        }

        digitalWrite(_cs, HIGH);
        delay(SPI_DELAY);
    }

    return false;
}

void SRIX_PN532_SPI::readData(uint8_t * databuff, const uint8_t datalen)
{
    if (!waitReady(TIMEOUT))
    {
        #ifdef DEBUG
        Serial.println("[!] ERROR : TIMEOUT!");
        #endif
        //return false;
    }

    digitalWrite(_cs, LOW);
    delay(SPI_DELAY);

    SPI.transfer(PN532_SPI_READ);

    for (int i = 0; i < datalen; ++i)
    {
        databuff[i] = SPI.transfer(PN532_SPI_READ);
    }

    digitalWrite(_cs, HIGH);
    delay(SPI_DELAY);
}

void SRIX_PN532_SPI::sendCommand(const uint8_t * buff, const uint8_t buflen)
{
    uint8_t checksum;

    SPI.beginTransaction(SPISettings(1000000L, LSBFIRST, SPI_MODE0));

    digitalWrite(_cs, LOW);
    delay(SPI_DELAY);
    
    SPI.transfer(PN532_SPI_WRITE);
    
    SPI.transfer(PN532_PREAMBLE);
    SPI.transfer(PN532_STARTCODE1);
    SPI.transfer(PN532_STARTCODE2);

    checksum = buflen + 1;
    SPI.transfer(checksum);
    SPI.transfer(~checksum + 1);

    checksum = PN532_HOSTTOPN532;
    SPI.transfer(PN532_HOSTTOPN532);

    for (int i = 0; i < buflen; ++i)
    {
        SPI.transfer(buff[i]);
        checksum += buff[i];
    }

    SPI.transfer(~checksum + 1);

    SPI.transfer(PN532_POSTAMBLE);

    SPI.endTransaction();

    digitalWrite(_cs, HIGH);
    delay(SPI_DELAY);
}
