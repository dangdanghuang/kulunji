#include <U8g2lib.h>
#include <Arduino.h>

// 定义 EC11 编码器的 CLK 引脚，连接到 Arduino 的数字引脚 2
#define ENCODER_CLK_PIN 2
// 定义 EC11 编码器的 DT 引脚，连接到 Arduino 的数字引脚 3
#define ENCODER_DT_PIN 3
// 定义 EC11 编码器的 SW 引脚，连接到 Arduino 的数字引脚 4，用于确认操作
#define ENCODER_SW_PIN 4

// 定义 U8g2 实例，使用 128x64 I2C OLED 屏幕，软件 I2C 方式
// U8G2_R0 表示屏幕旋转角度为 0 度
// SCL 是时钟线，SDA 是数据线，U8X8_PIN_NONE 表示没有复位引脚
U8g2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

// 定义菜单选项数组，可根据需要添加更多菜单项
const char* menuItems[] = {
  "Option 1",
  "Option 2",
  "Option 3",
  "Option 4",
  "Option 5",
  "Option 6",
  "Option 7",
  "Option 8",
  "Option 9",
  "Option 10"
};
// 计算菜单选项的数量
const int menuItemCount = sizeof(menuItems) / sizeof(menuItems[0]);
// 当前选中的菜单项索引，初始化为 0
int currentMenuItem = 0;

// 编码器变量，用于记录上一次 CLK 引脚的状态
int lastCLKState;
// 编码器变量，用于记录当前 CLK 引脚的状态
int currentCLKState;

// 每页显示的菜单项数量
const int itemsPerPage = 4; 
// 当前显示的页码，初始化为 0
int currentPage = 0;

// 初始化函数，在程序开始时执行一次
void setup() {
  // 初始化串口通信，波特率为 9600
  Serial.begin(9600);

  // 初始化 U8g2 库，准备与 OLED 屏幕通信
  u8g2.begin();
  // 设置 OLED 屏幕的字体为 u8g2_font_ncenB08_tr
  u8g2.setFont(u8g2_font_ncenB08_tr);

  // 将编码器的 CLK 引脚设置为输入上拉模式
  pinMode(ENCODER_CLK_PIN, INPUT_PULLUP);
  // 将编码器的 DT 引脚设置为输入上拉模式
  pinMode(ENCODER_DT_PIN, INPUT_PULLUP);
  // 将编码器的 SW 引脚设置为输入上拉模式
  pinMode(ENCODER_SW_PIN, INPUT_PULLUP);

  // 读取编码器 CLK 引脚的初始状态
  lastCLKState = digitalRead(ENCODER_CLK_PIN);
}

// 主循环函数，程序会不断循环执行此函数
void loop() {
  // 读取编码器 CLK 引脚的当前状态
  currentCLKState = digitalRead(ENCODER_CLK_PIN);

  // 检查 CLK 引脚状态是否发生变化
  if (currentCLKState != lastCLKState) {
    // 根据 DT 引脚状态判断编码器旋转方向
    if (digitalRead(ENCODER_DT_PIN) != currentCLKState) {
      // 顺时针旋转，当前菜单项索引加 1
      currentMenuItem++;
      // 如果当前菜单项索引超出菜单选项数量，将其重置为 0
      if (currentMenuItem >= menuItemCount) {
        currentMenuItem = 0;
      }
    } else {
      // 逆时针旋转，当前菜单项索引减 1
      currentMenuItem--;
      // 如果当前菜单项索引小于 0，将其设置为最后一个菜单项的索引
      if (currentMenuItem < 0) {
        currentMenuItem = menuItemCount - 1;
      }
    }
    // 当菜单项索引改变时，更新 OLED 屏幕上的菜单显示
    drawMenu();
  }
  // 更新上一次 CLK 引脚的状态
  lastCLKState = currentCLKState;

  // 检测编码器的 SW 引脚是否被按下
  if (digitalRead(ENCODER_SW_PIN) == LOW) {
    // 消抖处理，延迟 50 毫秒
    delay(50);
    // 再次检查 SW 引脚是否仍然被按下
    if (digitalRead(ENCODER_SW_PIN) == LOW) {
      // 记录按键按下的开始时间
      unsigned long pressStartTime = millis();
      // 持续检测按键是否被按下
      while (digitalRead(ENCODER_SW_PIN) == LOW) {
        // 检查按键是否长按超过 1 秒
        if (millis() - pressStartTime > 1000) { 
          // 长按操作，切换到下一页
          currentPage++;
          // 如果当前页码超出总页数，将其重置为 0
          if (currentPage * itemsPerPage >= menuItemCount) {
            currentPage = 0;
          }
          // 更新 OLED 屏幕上的菜单显示
          drawMenu();
          // 等待按键释放
          while (digitalRead(ENCODER_SW_PIN) == LOW); 
          break;
        }
      }
      // 如果按键按下时间小于 1 秒，视为短按操作
      if (millis() - pressStartTime < 1000) {
        // 短按操作，确认选择当前菜单项
        Serial.print("Selected: ");
        Serial.println(menuItems[currentMenuItem]);
        // 等待按键释放
        while (digitalRead(ENCODER_SW_PIN) == LOW);
      }
    }
  }
}

// 绘制菜单到 OLED 屏幕的函数
void drawMenu() {
  // 开始绘制第一页
  u8g2.firstPage();
  do {
    // 计算当前页第一个菜单项的索引
    int startIndex = currentPage * itemsPerPage;
    // 计算当前页最后一个菜单项的索引
    int endIndex = startIndex + itemsPerPage;
    // 如果最后一个菜单项索引超出菜单选项数量，将其设置为菜单选项数量
    if (endIndex > menuItemCount) {
      endIndex = menuItemCount;
    }
    // 遍历当前页的菜单项
    for (int i = startIndex; i < endIndex; i++) {
      // 计算菜单项在当前页的显示索引
      int displayIndex = i - startIndex;
      // 如果当前菜单项是选中项，在前面绘制 '>' 标记
      if (i == currentMenuItem) {
        u8g2.drawStr(0, (displayIndex + 1) * 10, "> ");
      } else {
        // 否则绘制两个空格
        u8g2.drawStr(0, (displayIndex + 1) * 10, "  ");
      }
      // 在屏幕上绘制菜单项的文本
      u8g2.drawStr(10, (displayIndex + 1) * 10, menuItems[i]);
    }
    // 在屏幕右下角显示当前页码和总页数信息
    u8g2.drawStr(100, 60, String(currentPage + 1) + "/" + String((menuItemCount + itemsPerPage - 1) / itemsPerPage).c_str());
  // 继续绘制下一页，直到所有页绘制完成
  } while (u8g2.nextPage());
}    