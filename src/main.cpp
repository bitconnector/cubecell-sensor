#include "LoRaWan_APP.h"
#include "Arduino.h"
#include "config.hpp"

#include "DHT.h"
#define DHTPIN GPIO5
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// The interrupt pin is attached to USER_KEY
#define INT_PIN USER_KEY

bool accelWoke = false;

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

uint16_t batteryVoltage, batteryLevel;
uint8_t batData;
void readBat()
{
  batteryVoltage = getBatteryVoltage();
  batteryLevel = (BoardGetBatteryLevel() / 254) * 100;

  batData = (batteryVoltage / 10) - 250;
  Serial.printf("batData:%08x\n", batData);

  Serial.printf("Bat:%u, %u%\n", batteryVoltage, batteryLevel);
}

float temperature, humidity;
uint32_t dhtData;
void readTemp()
{
  digitalWrite(Vext, LOW);
  // Wait a few seconds between measurements.
  delay(800);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  temperature = dht.readTemperature();

  digitalWrite(Vext, HIGH);

  dhtData = ((uint16_t)((temperature + 40) * 10)) << 12 | ((uint16_t)(humidity * 10));
  Serial.printf("dhtHum:%06x\n", ((uint16_t)((temperature + 40) * 10)));
  Serial.printf("dhtData:%06x\n", dhtData);

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
void prepareTxFrame()
{
  readTemp();
  readBat();

  appPort = 1;
  isTxConfirmed = LORAWAN_UPLINKMODE;

  appDataSize = 4;

  appData[0] = dhtData >> 16;
  appData[1] = dhtData >> 8;
  appData[2] = dhtData;

  appData[3] = batData;

  Serial.print("appData:");
  for (int i = 0; i < appDataSize; i++)
  {
    Serial.printf(" %02x", appData[i]);
  }
  Serial.println();
}

void accelWakeup()
{
  delay(10);
  if (digitalRead(INT_PIN) == HIGH)
  {
    accelWoke = true;
  }
}

void setup()
{
  pinMode(Vext, OUTPUT);
  dht.begin();
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
    readTemp();
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
    prepareTxFrame(); // sending according to appTxDutyCycle
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
      prepareTxFrame();
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
