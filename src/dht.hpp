#include "DHT.h"

#define USE_DHT

#define DHTPIN GPIO5
#define DHTTYPE DHT22

extern float temperature, humidity;

void readDht();
void packDhtData(uint8_t *appData, uint8_t *appDataSize);
