#include <Arduino.h>
#include "DHTesp.h"
#include <Ticker.h>

#ifndef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP32 ONLY!)
#error Select ESP32 board.
#endif

struct TempReadResult {
    bool ok = false;
    TempAndHumidity tempAndHumidity;
};

namespace TempSensor {
    DHTesp dht;

    void setup(int pin) {
        dht.setup(pin, DHTesp::DHT22);
    }

    TempReadResult readTemperatureAndHumidity() {
        TempReadResult result;
        result.ok = false;
        auto tempHumid = dht.getTempAndHumidity();
        if (dht.getStatusString() == "OK") {
            result.ok = true;
            result.tempAndHumidity = tempHumid;
            Serial.print("\t");
            Serial.print(tempHumid.humidity, 1);
            Serial.print(" %hum\t");
            Serial.print(tempHumid.temperature, 1);
            Serial.print(" °C \n");
        } 
        return result;
    }
}