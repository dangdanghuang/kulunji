#include <Preferences.h>
#include <Arduino.h>

// 创建 Preferences 对象
Preferences preferences;
// 定义 NVS 命名空间
const char* nvsNamespace = "myStorage";
// 定义存储数据的键名
const char* dataKey = "";
void get_config(){
  String storedData = preferences.getString(dataKey, "");
      if (!storedData.isEmpty()) {
        Serial.print("Data read from NVS: ");
        Serial.println(storedData);
      } else {
        Serial.println("No data found in NVS.");
      }
}
void setup() {
  // 初始化串口通信，设置波特率为 115200
  Serial.begin(115200);
  // 打开 NVS 命名空间，以读写模式打开
  preferences.begin(nvsNamespace, false);
  get_config();
}

void loop() {
  if (Serial.available() > 0) {
    // 读取串口输入的一行数据
    String inputData = Serial.readStringUntil('\n');
    // 去除字符串首尾的空白字符
    inputData.trim();


    if (!inputData.isEmpty()) {
      // 将数据存储到 NVS 中
      inputData=inputData.toFloat()/1.00233;
      
      preferences.putString(dataKey, inputData);
      Serial.print("Data stored in NVS: ");
      Serial.println(inputData);

      // 从 NVS 中读取存储的数据
     
    }
  }
   
      delay(1000);
}