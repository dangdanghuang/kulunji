#define BLINKER_WIFI
 #include <Blinker.h>
#include <Wire.h>
#include <INA226.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <stdio.h>
#include <stdlib.h>
#include <Preferences.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#define voltage_addr 0

U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, 5,13,/* reset=*/ U8X8_PIN_NONE);


// 创建 Preferences 对象
Preferences preferences;
const String RESTART_COMMAND = "rst";
// 定义 NVS 命名空间
const char* nvsNamespace = "myStorage";
// 定义存储数据的键名
const char* dataKey = "set_input_value";
const char* dataKey1="energy";
//const char* dataKey = "set_input_val";
//定义点灯组件和认证信息
char auth[] ="8f2d2e5c87b2";
char ssid[] ="lhhg";
char password[]="84497172";

BlinkerNumber NumberVolt("voltage");
BlinkerNumber Numbercurrent("current");
BlinkerNumber NumberPower("Power");
BlinkerNumber NumberEnergy("Energy");
// 初始化 INA226
//INA226 ina(Wire);
INA226 INA(0x44);
float calibrationValue = 0, set_input_val=1 , voltage=0, current=0, power=0 ,energy=0;
bool flag_config=0;
unsigned long lastTime = 0;
void getPOWER()
{
  voltage = set_input_val*INA.getBusVoltage(); // 总线电压（V） 
  current = INA.getCurrent();    // 电流（A）
  power = INA.getPower();       // 功率（W）
  
  // // 计算电量
  unsigned long currentTime = millis();
  if (lastTime != 0) {
    float deltaTime = (currentTime - lastTime) / 3600000.0; // 转换为小时
    energy += power * deltaTime; // 电量 = 功率 * 时间
    preferences.putFloat(dataKey1, energy);
  }
  lastTime = currentTime;
  //Blinker.print(Numbercurrent,"current");
}
void display()
{
// 更新 OLED 显示
  u8g2.firstPage();
  do {
    u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);

  u8g2.setCursor(0, 10);
  u8g2.print("Voltage: ");
   u8g2.print(voltage, 2);
  u8g2.print(" V");

  u8g2.setCursor(0, 26);
  u8g2.print("Current: ");
  u8g2.print(current, 3);
  u8g2.print(" A");

  u8g2.setCursor(0, 42);
  u8g2.print("Power: ");
  u8g2.print(power, 2);
  u8g2.print(" W");

  u8g2.setCursor(0, 58);
  u8g2.print("Energy: ");
  u8g2.print(energy, 2);
  u8g2.print(" Wh");
  
  } while ( u8g2.nextPage() );
  
}
void parseCommand(String command){
  //分割命令和参数
  int firstComma = command.indexOf(',');
  int secondComma = command.indexOf(',',firstComma + 1);
  String cmd =command.substring(0,firstComma);
  String param1 = (firstComma != -1) ? command.substring(firstComma + 1,secondComma) :"" ;
  String param2 = (secondComma != -1) ? command.substring(secondComma + 1) :"" ;

  if (cmd.equalsIgnoreCase("v_cal")){
        if(param1.equalsIgnoreCase("on")){
          Serial.println("led is on");
          //此处添加控制代码
        }
        else if (param1.equalsIgnoreCase("off")){

           Serial.println("led is off");
          //此处添加控制代码
        }
        else {
           Serial.println("Invalid command. try another one");

          //此处添加控制代码
        }
  }
  /////////////////////////////////////
   if (cmd.equalsIgnoreCase("R_SHUNT")){
        if(param1.equalsIgnoreCase("on")){
          Serial.println("led is on");
          //此处添加控制代码
        }
        else if (param1.equalsIgnoreCase("off")){

           Serial.println("led is off");
          //此处添加控制代码
        }
        else {
           Serial.println("Invalid command. try another one");
           
          //此处添加控制代码
        }
  }

}
void get_config(){
    // String test = preferences.getString(dataKey,"" );
    // set_input_val=test.toFloat();
    //  Serial.print("get set value  =");
    //  Serial.println(set_input_val );
      String storedData = preferences.getString(dataKey, "");
      energy=preferences.getFloat(dataKey1,energy);
      if (!storedData.isEmpty()) {
        set_input_val=storedData.toFloat();
        Serial.print("Data read from NVS: ");

        Serial.println(storedData);
      } else {
        Serial.println("No data found in NVS.");
      }
}
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
  // 初始化串口
  Serial.begin(115200);
  BLINKER_DEBUG.stream(Serial);
  setup_wifi();
  Blinker.begin(auth, ssid, password);
 // 初始化 OLED
  u8g2.begin();
//   // 初始化 INA226
  Wire.begin();
  if (!INA.begin()) {
    Serial.println("Failed to find INA226 chip");
    while (1);
  }
  INA.setMaxCurrentShunt(40.0, 0.002);                    // 最大电流 2A，分流电阻 0.1 欧姆
  preferences.begin(nvsNamespace, false);                 // 打开 NVS 命名空间，以读写模式打开
   get_config();
}

void loop() {
      Blinker.run();
    if (Serial.available() > 0) {
        // 读取串口输入的校准值
        String inputData = Serial.readStringUntil('\n');
        if (!inputData.isEmpty()) {
         
          inputData.trim();
          if(inputData==RESTART_COMMAND){
            Serial.println("Restarting the program...");
      // 重启 ESP32
             ESP.restart();
          }
          inputData=inputData.toFloat()/INA.getBusVoltage();
          preferences.putString(dataKey, inputData);
          Serial.print("Data stored in NVS:inputData is");
          Serial.println(inputData);   
           }  
        
      
      }
     
     
      getPOWER();
      display();
       Serial.print("voltage=");
      Serial.println(voltage);

      Serial.print("电压=");
      Serial.println(INA.getBusVoltage());
     
      Serial.print("set_input_val=");
      Serial.println(set_input_val);

      Serial.print("电流=");
      Serial.println(INA.getCurrent_mA());
       
    //  Serial.println(INA.getBusVoltage());
  // 打印数据到串口

  // Serial.print("Voltage: ");
  // Serial.print(calibrationValue, 2);
  // Serial.print(" set_input_val: ");
  // Serial.print(set_input_val, 3);
  // Serial.print(" V, Current: ");
  // Serial.print(current, 3);
  // Serial.print(" A, Power: ");
  // Serial.print(power, 2);
  // Serial.print(" W, Energy: ");
  // Serial.print(energy, 2);
  // Serial.println(" Wh");

  // 延时 1 秒
   
 delay(2000);
}