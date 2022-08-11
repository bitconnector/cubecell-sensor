# possibilitys per bit

| bits | possibilitys |
| ---- | ------------ |
| 1    | 2            |
| 2    | 4            |
| 3    | 8            |
| 4    | 16           |
| 5    | 32           |
| 6    | 64           |
| 7    | 128          |
| 8    | 256          |
| 9    | 512          |
| 10   | 1024         |
| 11   | 2048         |
| 12   | 4096         |
| 13   | 8192         |
| 14   | 16384        |
| 15   | 32768        |
| 16   | 65536        |


# batVoltage

range: 4.3V - 2.5V = 1.8V

step: 1.8V / 0.01V = 180 -> 8bit

limit: 2.5V + (0.01V * 256) = 5,06V

# DHT22

## Humidity

range: 100% - 0% = 100%

step: 100% / 0.1% = 1000 -> 10bit

limit: 0% + (0.1% * 1024) = 102.4%


## Temperature

range: 80C - -40C = 120C

step: 120C / 0.1C = 1200 -> 11bit

limit: -40C + (0.1C * 2048) = 164.8C

