#include <Arduino.h>
#include <Ticker.h>
#include <Wire.h>
#include "ClosedCube_SHT31D.h"

struct TempReadResult {
    bool ok = false;
    SHT31D tempAndHumidity;
};

namespace TempSensor {
    ClosedCube_SHT31D sht3xd;

    void setup() {
        sht3xd.begin(0x44); // I2C address: 0x44 or 0x45
        Serial.println(String("Temp sensor started"));
    }

    TempReadResult readTemperatureAndHumidity() {
        TempReadResult result;
        result.ok = false;
        SHT31D tempHumid = sht3xd.readTempAndHumidity(SHT3XD_REPEATABILITY_MEDIUM, SHT3XD_MODE_POLLING, 50);
        if (tempHumid.error == SHT3XD_NO_ERROR) {
            result.ok = true;
            result.tempAndHumidity = tempHumid;
            Serial.print("\t");
            Serial.print(tempHumid.rh, 1);
            Serial.print(" %hum\t");
            Serial.print(tempHumid.t, 1);
            Serial.print(" °C \n");
        } else {
            Serial.println(String("Failed to read temp sensor ") + String(tempHumid.error));
        }
        return result;
    }
}