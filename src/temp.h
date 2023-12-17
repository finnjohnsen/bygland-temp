#include <Arduino.h>
#include "DHTesp.h"
#include <Ticker.h>

#ifndef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP32 ONLY!)
#error Select ESP32 board.
#endif

namespace Temp {
    DHTesp dht;
    struct TempAndHumidity lastTempAndHumidity = {0, 0};

    TempAndHumidity getLastRead() {
        return lastTempAndHumidity;
    }

    void setup(int pin) {
        dht.setup(pin, DHTesp::DHT22);
  
    }

    void loop() {
        lastTempAndHumidity = dht.getTempAndHumidity();
        
        Serial.print(dht.getStatusString());
        Serial.print("\t");
        Serial.print(lastTempAndHumidity.humidity, 1);
        Serial.print(" %hum\t");
        Serial.print(lastTempAndHumidity.temperature, 1);
        Serial.print(" °C \n");
        //Serial.print(dht.computeHeatIndex(temperature, humidity, false), 1);
        //Serial.print("\n");

    }
}