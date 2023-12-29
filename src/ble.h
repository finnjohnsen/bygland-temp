#include <Arduino.h>
#include <NimBLEDevice.h>

NimBLECharacteristic* pTempCharacteristic = nullptr;
NimBLECharacteristic* pHumCharacteristic = nullptr;

bool deviceConnected = false;
bool oldDeviceConnected = false;

NimBLEUUID environmentService = NimBLEUUID((uint16_t)0x181A); 

/*
2A1F - sint16 temperature in 0.1 °C
2A6E - sint16 temperature in 0.01 °C
2A1C - float
0x2A6F Humidity
*/
NimBLEUUID temperatureCharacteristicUUID = NimBLEUUID((uint16_t)0x2A6E);
NimBLEUUID humidityCharacteristicUUID = NimBLEUUID((uint16_t)0x2A6F);

#ifndef DEVICE_NAME
#pragma message(DEVICE_NAME MUST BE SET)
#error Set -D DEVICE_NAME=xx pin in build_flag in platform.io
#endif


namespace BLE {

    bool isDeviceConnected() {
        return deviceConnected;
    }

    class MyServerCallbacks: public NimBLEServerCallbacks {
        void onConnect(NimBLEServer* pServer) {
            Serial.println("Connect");

            deviceConnected = true;
        };

        void onDisconnect(NimBLEServer* pServer, ble_gap_conn_desc* desc) {
        // Peer disconnected, add them to the whitelist
        // This allows us to use the whitelist to filter connection attempts
        // which will minimize reconnection time.
            NimBLEDevice::whiteListAdd(NimBLEAddress(desc->peer_ota_addr));
            deviceConnected = false;
        }
    };

    void onAdvComplete(NimBLEAdvertising *pAdvertising) {
        Serial.println("Advertising stopped");
        if (deviceConnected) {
            return;
        }
        // If advertising timed out without connection start advertising without whitelist filter
        pAdvertising->setScanFilter(false,false);
        pAdvertising->start();
    }

    void setup() {
        NimBLEDevice::init(String(DEVICE_NAME).c_str());
        // NimBLEDevice::setSecurityAuth(true, true, true);
        // NimBLEDevice::setSecurityPasskey();
        // NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);

        NimBLEServer* pServer = NimBLEDevice::createServer();
        NimBLEDevice::setPower(ESP_PWR_LVL_P9); /** +9db */

        pServer->setCallbacks(new MyServerCallbacks());
        pServer->advertiseOnDisconnect(false);
        NimBLEService *pService = pServer->createService(environmentService);

        pTempCharacteristic = pService->createCharacteristic(
                temperatureCharacteristicUUID,
                NIMBLE_PROPERTY::READ   |
                NIMBLE_PROPERTY::NOTIFY );
        pHumCharacteristic = pService->createCharacteristic(
                humidityCharacteristicUUID,
                NIMBLE_PROPERTY::READ   |
                NIMBLE_PROPERTY::NOTIFY );

        pService->start();
        NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
        pAdvertising->addServiceUUID(environmentService);
        pAdvertising->setScanResponse(false);
        pAdvertising->start();
        Serial.println("Waiting a client connection to notify...");
    }

    void notify(TempAndHumidity tempAndHumidity) {
        if (deviceConnected) {
            pTempCharacteristic->setValue(String(tempAndHumidity.temperature*100, 1).toInt());
            pTempCharacteristic->notify();
            pHumCharacteristic->setValue(String(tempAndHumidity.humidity*100, 1).toInt());
            pHumCharacteristic->notify();
        }
    }

    void updateConnections() {
        // disconnecting
        if (!deviceConnected && oldDeviceConnected) {
            NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
            if (NimBLEDevice::getWhiteListCount() > 0) {
                // Allow anyone to scan but only whitelisted can connect.
                pAdvertising->setScanFilter(false,true);
            }
            // advertise with whitelist for 30 seconds
            pAdvertising->start(30, onAdvComplete);
            Serial.println("start advertising");
            oldDeviceConnected = deviceConnected;
        }
        // connecting
        if (deviceConnected && !oldDeviceConnected) {
            // do stuff here on connecting
            oldDeviceConnected = deviceConnected;
        }
    }
}