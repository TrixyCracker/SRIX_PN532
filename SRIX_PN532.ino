#include "SRIX_PN532_SPI.h"

#define CS 5
SRIX_PN532_SPI pn532(CS);

/*
#include "SRIX_PN532_I2C.h"

#define RST 2
#define IRQ 3
SRIX_PN532_I2C pn532(IRQ, RST);
*/

#define BLOCK_SIZE 4
#define BLOCK_NUMBER 128

char buffer[32] = "";

void setup()
{
    Serial.begin(115200);
    
    while(!Serial);

    Serial.println("\nHello World!\n");

    pn532.begin();

    uint32_t versiondata = pn532.getFirmwareVersion();
    if (!versiondata)
    {
        Serial.println("Didn't find PN53x board");
        while (1);
    }

    Serial.print("Found chip PN5");
    Serial.println((versiondata >> 24) & 0xFF, HEX);
    Serial.print("Firmware ver. ");
    Serial.print((versiondata >> 16) & 0xFF, DEC);
    Serial.print(".");
    Serial.println((versiondata >> 8) & 0xFF, DEC);
    Serial.println("Supported functionalities:");
    if (versiondata & 0b1)
    {
        Serial.println("\t-> ISO/IEC 14443 TypeA");
    }
    if (versiondata & 0b10)
    {
        Serial.println("\t-> ISO/IEC 14443 TypeB");
    }
    if (versiondata & 0b100)
    {
        Serial.println("\t-> ISO 18092");
    }

    pn532.SAMConfiguration();
    pn532.ISO14443BInit();
    pn532.deselectTag();

    while (!pn532.srixInitiate())
    {
        delay(500);
    }
    
    uint8_t UID[8];
    pn532.readUID(UID);

    sprintf(buffer, "UID: 0x%02X%02X%02X%02X%02X%02X%02X%02X", UID[0], UID[1], UID[2], UID[3], UID[4], UID[5], UID[6], UID[7]);
    Serial.println(buffer);

    uint8_t block[BLOCK_SIZE];
    for (int i = 0; i < BLOCK_NUMBER; ++i)
    {
        pn532.readBlock(block, i);

        sprintf(buffer, "[%02X]: %02X %02X %02X %02X", i, block[0], block[1], block[2], block[3]);
        Serial.println(buffer);
    }

}

void loop()
{}