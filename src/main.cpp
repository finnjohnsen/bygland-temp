#include <Arduino.h>
#include <arduino-timer.h>
#include "tempSensor.h"
#include "ble.h"
#include "oled.h"

const auto READ_TEMPERATURE_INTERVAL_MS = 3000;
const auto BLE_NOTIFY_INTERVAL_MS = 6010;


Timer<> timer; // save as above

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

void setup() {
  Serial.begin(115200);
  OLED::setup();
  BLE::setup();
  TempSensor::setup(GPIO_NUM_32);
  timer.in(2100, startup);
  timer.every(500, ble_update_connections);
}

void loop() {
  timer.tick();
}
