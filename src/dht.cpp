#include "dht.hpp"

float temperature, humidity;

void readDht()
{
    DHT dht(DHTPIN, DHTTYPE);
    dht.begin();

    digitalWrite(Vext, LOW);
    // Wait a few seconds between measurements.
    delay(800);

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    humidity = dht.readHumidity();
    // Read temperature as Celsius (the default)
    temperature = dht.readTemperature();

    digitalWrite(Vext, HIGH);

    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temperature))
    {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
    }

    Serial.print(F("Humidity: "));
    Serial.print(humidity, 2);
    Serial.print(F("%  Temperature: "));
    Serial.print(temperature, 2);
    Serial.println(F("°C "));
}

/* Prepares the payload of the frame */
uint8_t packDhtData(uint8_t *appData)
{
    readTemp();

    uint32_t dhtData = ((uint16_t)((temperature + 40) * 10)) << 12 | ((uint16_t)(humidity * 10));
    Serial.printf("dhtHum:%06x\n", ((uint16_t)((temperature + 40) * 10)));
    Serial.printf("dhtData:%06x\n", dhtData);

    // readBat();

    //*appPort = 1;
    // isTxConfirmed = LORAWAN_UPLINKMODE;

    // appDataSize = 4;

    appData[0] = dhtData >> 16;
    appData[1] = dhtData >> 8;
    appData[2] = dhtData;

    // appData[3] = batData;

    Serial.print("appData:");
    for (int i = 0; i < 4; i++)
    {
        Serial.printf(" %02x", appData[i]);
    }
    Serial.println();
    return 3;
}
