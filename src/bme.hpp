#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

namespace bme
{
    extern float temperature, humidity, pressure;

    bool readBme();
    void packBmeData(uint8_t *appData, uint8_t *appDataSize);
}
