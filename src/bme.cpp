#include "bme.hpp"

namespace bme
{

    float temperature, humidity, pressure;

    bool readBme()
    {
        bool success = 0;
        Adafruit_BME280 bme;
        digitalWrite(Vext, LOW);
        delay(10);
        Wire.begin();
        bme.begin(0x76);
        for (int i = 0; i < 15; i++)
        {
            success = bme.init();
            if (success)
                break;
            delay(10);
        }
        if (!success)
            return 0;

        temperature = bme.readTemperature();
        pressure = bme.readPressure() / 100.0F;
        humidity = bme.readHumidity();

        Wire.end();
        digitalWrite(Vext, HIGH);
        return 1;
    }

    /* Prepares the payload of the frame */
    void packBmeData(uint8_t *appData, uint8_t *appDataSize)
    {
        readBme();

        uint32_t temphum = ((uint16_t)((temperature + 40) * 10)) << 12 | ((uint16_t)(humidity * 10));
        // Serial.printf("dhtHum:%06x\n", ((uint16_t)((temperature + 40) * 10)));
        // Serial.printf("dhtData:%06x\n", dhtData);
        u_int16_t pres = (pressure - 300) * 50;

        appData[*appDataSize] = temphum >> 16;
        appData[*appDataSize + 1] = temphum >> 8;
        appData[*appDataSize + 2] = temphum;

        appData[*appDataSize + 3] = pres >> 8;
        appData[*appDataSize + 4] = pres;

        *appDataSize += 5;
    }
}