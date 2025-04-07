#define BLINKER_WIFI
#include <Blinker.h>


// Blinker 设备密钥
char auth[] ="8f2d2e5c87b2";
char ssid[] ="lhhg";
char pswd[]="84497172";


// DHT 传感器相关

// Blinker 数据组件
BlinkerNumber HUMI("humi");
BlinkerNumber TEMP("temp");

// 记录上次上传数据的时间
unsigned long lastUploadTime = 0;
// 上传数据的时间间隔（毫秒）
const unsigned long uploadInterval = 5000;

void setup_wifi() {
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, pswd);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected successfully!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void setup() {
    Serial.begin(115200);
    BLINKER_DEBUG.stream(Serial);

    // 初始化 DHT 传感器
   

    // 连接 WiFi
    setup_wifi();

    // 初始化 Blinker
    Blinker.begin(auth, ssid, pswd);
}

void loop() {
    Blinker.run();

    // 获取当前时间
    unsigned long currentTime = millis();

    // 达到上传时间间隔
    if (currentTime - lastUploadTime >= uploadInterval) {
        // 读取温度和湿度
        float h = 3.666;
        float t = 5.89;
        TEMP.print(t);
            HUMI.print(h);
            Serial.print("Temperature: ");
            Serial.print(t);
            Serial.print(" °C, Humidity: ");
            Serial.print(h);
            Serial.println(" %");
        // 更新上次上传时间
        lastUploadTime = currentTime;
    }
}
    