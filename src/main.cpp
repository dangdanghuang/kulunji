#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <Preferences.h>

// 定义Web服务器，监听80端口
WebServer server(80);
// 用于存储账号密码到Flash
Preferences preferences;

// 全局变量存储账号密码
String ssid;
String password;
void startAP();
// 处理根路径请求，返回配网页面
void handleRoot() {

  String page = "<html><body>";
  //page += "<h1>WiFi Configuration</h1>";
  page += "<h1 style="font-size: 36px; color: blue; text-align: center;">WiFi Configuration</h1>";
  page += "<form method='post' action='/save'>";
  page += "SSID: <input type='text' name='ssid'><br>";
  page += "Password: <input type='password' name='password'><br>";
  page += "<input type='submit' value='Save'>";
  page += "</form>";
  page += "</body></html>";
  server.send(200, "text/html", page);
}

// 处理保存账号密码的请求
void handleSave() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    ssid = server.arg("ssid");
    password = server.arg("password");

    // 打开Preferences实例
    preferences.begin("wifi", false);
    // 保存账号密码到Flash
    preferences.putString("ssid", ssid);
    preferences.putString("password", password);
    // 关闭Preferences实例
    preferences.end();

    Serial.println("WiFi credentials saved.");
    server.send(200, "text/plain", "WiFi credentials saved. Restarting...");
    // 重启ESP32
    delay(2000);
    ESP.restart();
  } else {
    server.send(400, "text/plain", "Invalid request");
  }
}

// 尝试连接WiFi
void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid.c_str(), password.c_str());
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("Connection failed");
    // 启动AP模式，重新配网
    startAP();
  }
}

// 启动AP模式，提供配网页面
void startAP() {
  Serial.println("Starting AP mode...");
  WiFi.softAP("ESP32-Config", "password");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  // 注册路由
  server.on("/", handleRoot);
  server.on("/save", handleSave);
  // 启动服务器
  server.begin();
  Serial.println("Web server started");
}

void setup() {
  // 初始化串口通信
  Serial.begin(115200);

  // 打开Preferences实例
  //preferences.begin("wifi", true);
  // 从Flash读取账号密码
  //ssid = preferences.getString("ssid", "");
  //password = preferences.getString("password", "");
  // 关闭Preferences实例
 // preferences.end();

  if (ssid.length() > 0 && password.length() > 0) {
    connectToWiFi();
  } else {
    startAP();
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // 可以在这里添加其他功能代码
  } else {
    server.handleClient();
  }
}    