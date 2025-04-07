#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// 自定义服务和特征的UUID
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
void connectToWiFi();
// 用于存储WiFi的账号和密码
String ssid = "";
String password = "";
// 标记是否已经接收到账号和密码
bool credentialsReceived = false;

// 定义BLE回调类
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();

        if (value.length() > 0) {
            String input = String(value.c_str());
            int commaIndex = input.indexOf(',');
            if (commaIndex != -1) {
                ssid = input.substring(0, commaIndex);
                password = input.substring(commaIndex + 1);
                credentialsReceived = true;
                Serial.print("接收到的账号: ");
                Serial.println(ssid);
                Serial.print("接收到的密码: ");
                Serial.println(password);
                 connectToWiFi();
            } else {
                Serial.println("输入格式错误，请使用 账号,密码 的格式。");
            }
        }
    }
};

void setup() {
    Serial.begin(115200);

    // 初始化蓝牙
    BLEDevice::init("ESP32_Bluetooth");
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                            CHARACTERISTIC_UUID,
                                            BLECharacteristic::PROPERTY_READ |
                                            BLECharacteristic::PROPERTY_WRITE
                                        );

    pCharacteristic->setCallbacks(new MyCallbacks());
    pCharacteristic->addDescriptor(new BLE2902());

    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0);
    BLEDevice::startAdvertising();

    Serial.println("蓝牙已启动，等待连接...");
}

void loop() {
    // 主循环中可以添加其他逻辑
}

void connectToWiFi() {
    Serial.print("正在连接到 ");
    Serial.println(ssid);

    WiFi.begin(ssid.c_str(), password.c_str());

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 10) {
        delay(1000);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("");
        Serial.println("WiFi连接成功");
        Serial.print("IP地址: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("");
        Serial.println("WiFi连接失败，请检查账号和密码后重新输入。");
        credentialsReceived = false;
    }
}    