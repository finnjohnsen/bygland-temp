#include <Arduino.h>
#include "temp.h"
#include "ble.h"
#include "oled.h"


void setup() {
  Serial.begin(115200);
  Temp::setup(GPIO_NUM_32);
  BLE::setup();
  OLED::setup();
}

void loop() {
  //  delay(dht.getMinimumSamplingPeriod());

  delay(2100);
  
  Temp::loop();
  BLE::loop(Temp::getLastRead());
  OLED::loop(Temp::getLastRead());

}
