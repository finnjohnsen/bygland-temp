#include <Arduino.h>
#include <esp_wifi.h>
#include <arduino-timer.h>
#include "tempSensor.h"
#include "ble.h"
#include "oled.h"

#ifndef WAKE_SECONDS
#pragma message(WAKE_SECONDS MUST BE SET)
#error Set -D WAKE_SECONDS=xx in platform.io
#endif

#ifndef SLEEP_SECONDS
#pragma message(SLEEP_SECONDS MUST BE SET)
#error Set -D SLEEP_SECONDS=xx in platform.io
#endif

#ifndef TEMP_READ_SECONDS
#pragma message(TEMP_READ_SECONDS MUST BE SET)
#error Set -D TEMP_READ_SECONDS=xx in platform.io
#endif

Timer<> timer;
TempReadResult lastRead;
bool bleDeviceIsConnected = false;

bool temperature_read(void *) {
    lastRead = TempSensor::readTemperatureAndHumidity();
    if (lastRead.ok == true) {
      Serial.println("Sensor-lesing OK");
      OLED::updateScreen(lastRead.tempAndHumidity, bleDeviceIsConnected);
      BLE::notify(lastRead.tempAndHumidity);
    } else {
      Serial.println("Sensorfeil");
      OLED::updateScreen("Sensorfeil 😭");
    }
  return true;
}

bool startup(void *) {
  temperature_read(nullptr);
  timer.every(1000 * TEMP_READ_SECONDS, temperature_read);
  return true;
}

bool ble_update_connections(void *) {
  BLE::updateConnections();
  bleDeviceIsConnected = BLE::isDeviceConnected();
  return true;
}

bool sleep (void *) {
  esp_sleep_enable_timer_wakeup(1000000LL * 30);
  Serial.printf("Sleep");
  OLED::updateCenterIcon(SLEEPING);
  delay(200);
  esp_deep_sleep_start();
  return true;
}

void setup() {
    auto startCause = esp_sleep_get_wakeup_cause();
    esp_wifi_stop();
    Serial.begin(115200);
    if (startCause == 0) { Serial.println(String("Startup "));} 
    else { Serial.println(String("Wakeup ")); }
    OLED::setup(startCause);
    BLE::setup();
    TempSensor::setup();
    timer.in(2100, startup);
    timer.every(500, ble_update_connections);
    timer.in(1000 * WAKE_SECONDS, sleep);
}

void loop() {
  timer.tick();
}
