#include "DHT.h"

#define DHTPIN GPIO5
#define DHTTYPE DHT22

namespace dht
{
    extern float temperature, humidity;

    void readDht();
    void packDhtData(uint8_t *appData, uint8_t *appDataSize);
}
