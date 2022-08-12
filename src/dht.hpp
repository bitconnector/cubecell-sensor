#include "DHT.h"

#define DHTPIN GPIO5
#define DHTTYPE DHT22

extern float temperature, humidity;

void readDht();
uint8_t packDhtData(uint8_t *appData);
