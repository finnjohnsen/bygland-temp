#include <Arduino.h>
#include <esp_wifi.h>
#include <arduino-timer.h>
#include "tempSensor.h"
#include "ble.h"
#include "oled.h"

#ifndef DEVICE_NAME
#pragma message(DEVICE_NAME MUST BE SET)
#error Set -D DEVICE_NAME=xx pin in build_flag in platform.io
#endif

const auto READ_TEMPERATURE_INTERVAL_MS = 3000;
const auto BLE_NOTIFY_INTERVAL_MS = 6010;
Timer<> timer;
TempReadResult lastRead;
bool bleDeviceIsConnected = false;

bool ble_notify(void *) {
  if (lastRead.ok == true) {
    BLE::notify(lastRead.tempAndHumidity);
  }
  return true;
}

bool temperature_read(void *) {
    lastRead = TempSensor::readTemperatureAndHumidity();
    if (lastRead.ok == true) {
      OLED::updateScreen(lastRead.tempAndHumidity, bleDeviceIsConnected);
    } else {
      OLED::updateScreen("Sensorfeil 😭");
    }
  return true;
}

bool startup(void *) {
  temperature_read(nullptr);
  timer.every(READ_TEMPERATURE_INTERVAL_MS, temperature_read);
  timer.every(BLE_NOTIFY_INTERVAL_MS, ble_notify);
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
  esp_deep_sleep_start();
  return true;
}

void setup() {
  esp_wifi_stop();
  Serial.begin(115200);
  Serial.printf("Startup");
  
  OLED::setup();
  BLE::setup(DEVICE_NAME);
  TempSensor::setup();
  timer.in(2100, startup);
  timer.every(500, ble_update_connections);
  //timer.in(30000, sleep);
}

void loop() {
  timer.tick();
}
