#include <Arduino.h>
#include "DHTesp.h"
#include <Ticker.h>

#ifndef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP32 ONLY!)
#error Select ESP32 board.
#endif

#ifndef TEMP_GPIO
#pragma message(TEMP_GPIO MUST BE SET)
#error Set -D TEMP_GPIO=xx pin in build_flag in platform.io
#endif

struct TempReadResult {
    bool ok = false;
    TempAndHumidity tempAndHumidity;
};

namespace TempSensor {
    DHTesp dht;

    void setup() {
        dht.setup(TEMP_GPIO, DHTesp::DHT22);
        Serial.print(String("Temp sensor started\n"));
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
        } else {
            Serial.println(String("Failed to read temp sensor ") + String(dht.getStatusString()));
        }
        return result;
    }
}