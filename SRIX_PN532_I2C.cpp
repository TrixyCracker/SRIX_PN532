#include "SRIX_PN532_I2C.h"

/**
 * @brief Construct a new srix
 * 
 * @param irq Pin of pn532 irq
 * @param reset Pin of pn532 reset
 */
SRIX_PN532_I2C::SRIX_PN532_I2C(uint8_t irq, uint8_t reset)
{
    _irq = irq;

    _reset = reset;
}


/**
 * @brief Inizialize pn532
 * 
 */
void SRIX_PN532_I2C::begin()
{
    Wire.begin();

    delay(1000);

    //Da verificare
    pinMode(_reset, OUTPUT);
    pinMode(_irq, INPUT_PULLUP);

    digitalWrite(_reset, HIGH);
    digitalWrite(_reset, LOW);
    delay(400);
    digitalWrite(_reset, HIGH);
    delay(100);

}

/**
 * @brief Wait for pn532 get ready
 * 
 * @param timeout Timeout value
 * @return true Ready
 * @return false Timeout
 */
bool SRIX_PN532_I2C::waitReady(int timeout)
{
    unsigned long start = millis();

    while(millis() - start < timeout)
    {
        // Check if pn532 is ready
        if (digitalRead(_irq) == LOW) 
        {
            return true;
        }
    }

    return false;
}

/**
 * @brief Read data from pn532
 * 
 * @param databuff Buffer where the bytes get stored
 * @param datalen Number of bytes to read
 */
void SRIX_PN532_I2C::readData(uint8_t * databuff, const uint8_t datalen)
{
    if (!waitReady(TIMEOUT))
    {
        #ifdef DEBUG
        Serial.println("[!] ERROR : TIMEOUT!");
        #endif
        //return false;
    }

    Wire.requestFrom(0x24, datalen + 2);

    Wire.read();

    for (int i = 0; i < datalen; ++i)
    {
        databuff[i] = Wire.read();
    }

}

/**
 * @brief Send command to pn532
 * 
 * @param buff Buffer where stored the data to send
 * @param buflen Length of buffer
 */
void SRIX_PN532_I2C::sendCommand(const uint8_t * buff, const uint8_t buflen)
{
    uint8_t checksum;
    Wire.beginTransmission(0x24);

    Wire.write(PN532_PREAMBLE);
    Wire.write(PN532_STARTCODE1);
    Wire.write(PN532_STARTCODE2);

    checksum = buflen + 1;
    Wire.write(checksum);
    Wire.write(~checksum + 1);

    checksum = PN532_HOSTTOPN532;
    Wire.write(PN532_HOSTTOPN532);

    for (int i = 0; i < buflen; ++i)
    {
        Wire.write(buff[i]);
        checksum += buff[i];
    }

    Wire.write(~checksum + 1);

    Wire.write(PN532_POSTAMBLE);

    Wire.endTransmission();
}
