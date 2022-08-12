#include "LoRaWan_APP.h"
#include "Arduino.h"
#include "config.hpp"

#define USE_BME
//#define USE_DHT

#include "dht.hpp"
#include "bme.hpp"

// The interrupt pin is attached to USER_KEY
#define INT_PIN USER_KEY

/*the application data transmission duty cycle.  value in [ms].*/
/*For this example, this is the frequency of the device status packets */
uint32_t appTxDutyCycle = (5 * 60 * 1000); // 5min;

/*LoraWan region, select in arduino IDE tools*/
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;

/*LoraWan channelsmask, default channels 0-7*/
uint16_t userChannelsMask[6] = {0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};

/*LoraWan Class, Class A and Class C are supported*/
DeviceClass_t loraWanClass = LORAWAN_CLASS;

/*ADR enable*/
bool loraWanAdr = LORAWAN_ADR;

/* set LORAWAN_Net_Reserve ON, the node could save the network info to flash, when node reset not need to join again */
bool keepNet = LORAWAN_NET_RESERVE;

/* Indicates if the node is sending confirmed or unconfirmed messages */
bool isTxConfirmed = LORAWAN_UPLINKMODE;

/* Application port */
uint8_t appPort = 1;

uint16_t batteryVoltage;
void readBat()
{
  batteryVoltage = getBatteryVoltage();
  // Serial.printf("Bat:%u, %u%\n", batteryVoltage, batteryLevel);
}

void packBat(uint8_t *appData, uint8_t *appDataSize)
{
  readBat();
  appData[*appDataSize] = (batteryVoltage / 10) - 250;
  *appDataSize += 1;
}

bool accelWoke = false;
void accelWakeup()
{
  accelWoke = true;
}

void packPin(uint8_t *appData, uint8_t *appDataSize)
{
  appData[*appDataSize] = accelWoke;
  *appDataSize += 1;
}

void pepareStatusFrame()
{
  appPort = 1;
  appDataSize = 0;
  packBat(appData, &appDataSize);
  packPin(appData, &appDataSize);
}

void pepareDataFrame()
{
  appDataSize = 0;
#ifdef USE_DHT
  appPort = 2;
  packBat(appData, &appDataSize);
  dht::packDhtData(appData, &appDataSize);
#endif
#ifdef USE_BME
  appPort = 3;
  packBat(appData, &appDataSize);
  bme::packBmeData(appData, &appDataSize);
#endif
}

void setup()
{
  pinMode(Vext, OUTPUT);
  Serial.begin(115200);
  delay(2000);
  Serial.println("test1");

  deviceState = DEVICE_STATE_INIT;
  LoRaWAN.ifskipjoin();

  accelWoke = false;
  pinMode(INT_PIN, INPUT);
  attachInterrupt(INT_PIN, accelWakeup, RISING);
  Serial.println("Interrupts attached");
}

void loop()
{
  while (false)
  {
    // readDht();
    readBat();
    delay(2000);
  }

  if (accelWoke)
  {
    uint32_t now = TimerGetCurrentTime();
    Serial.print(now);
    Serial.println("accel woke");
  }

  switch (deviceState)
  {
  case DEVICE_STATE_INIT:
  {
    // LoRaWAN.generateDeveuiByChipID();
    // printDevParam();
    LoRaWAN.init(loraWanClass, loraWanRegion);
    deviceState = DEVICE_STATE_JOIN;
    break;
  }
  case DEVICE_STATE_JOIN:
  {
    LoRaWAN.join();
    break;
  }
  case DEVICE_STATE_SEND:
  {
    pepareDataFrame(); // sending according to appTxDutyCycle
    LoRaWAN.send();
    deviceState = DEVICE_STATE_CYCLE;
    break;
  }
  case DEVICE_STATE_CYCLE:
  {
    txDutyCycleTime = appTxDutyCycle + randr(0, APP_TX_DUTYCYCLE_RND); // Schedule next packet transmission
    LoRaWAN.cycle(txDutyCycleTime);
    deviceState = DEVICE_STATE_SLEEP;
    break;
  }
  case DEVICE_STATE_SLEEP:
  {
    if (accelWoke && IsLoRaMacNetworkJoined) // button pressed
    {
      pepareStatusFrame();
      LoRaWAN.send();
    }
    accelWoke = false;
    LoRaWAN.sleep();
    break;
  }
  default:
  {
    deviceState = DEVICE_STATE_INIT;
    break;
  }
  }
}
