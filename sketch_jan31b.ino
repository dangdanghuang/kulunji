#include <EEPROM.h>
#include <U8g2lib.h>
#include <Wire.h>
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
#define U8G2_WITH_FONT_ROTATION
#define U8G2_WITH_ALL_FONTS
/*IO定义*/
#define up 14
#define down 13
#define left 16
#define right 17
#define mid 15
#define laser_out 18
#define sign_out 19
#define buzz 23
#define buck_sw 25
#define adc_temp 36
#define adc_volt 39
/*pwm通道定义，无需修改*/
#define laser_pwm 0
#define sign_pwm 1
/*激光功率上下限设置*/
const int laser_pct_max = 75;
const int laser_pct_min = 40;
/*光路校准功率设置*/
const int laser_pct_cal = 5;
/*指示亮度上下限设置*/
const int sign_pct_max = 100;
const int sign_pct_min = 0;
/*激光器允许温度上限*/
const float temp_max = 40;
/*电池低电量下限*/
const float bat_low = 10.8;
/*警告图标数组*/
static const unsigned char bmp1[] U8X8_PROGMEM = {
  0x00,
  0x80,
  0x01,
  0x00,
  0x00,
  0xC0,
  0x03,
  0x00,
  0x00,
  0xC0,
  0x03,
  0x00,
  0x00,
  0xE0,
  0x07,
  0x00,
  0x00,
  0x60,
  0x0E,
  0x00,
  0x00,
  0x70,
  0x0E,
  0x00,
  0x00,
  0x30,
  0x1C,
  0x00,
  0x00,
  0x38,
  0x1C,
  0x00,
  0x00,
  0x9C,
  0x39,
  0x00,
  0x00,
  0xDC,
  0x33,
  0x00,
  0x00,
  0xCE,
  0x73,
  0x00,
  0x00,
  0xCE,
  0x63,
  0x00,
  0x00,
  0xC7,
  0xE3,
  0x00,
  0x00,
  0xC3,
  0xC3,
  0x01,
  0x80,
  0x83,
  0xC1,
  0x01,
  0x80,
  0x81,
  0x81,
  0x03,
  0xC0,
  0x81,
  0x81,
  0x03,
  0xE0,
  0x80,
  0x01,
  0x07,
  0xE0,
  0x80,
  0x01,
  0x06,
  0x70,
  0x80,
  0x01,
  0x0E,
  0x70,
  0x00,
  0x00,
  0x0C,
  0x38,
  0x80,
  0x01,
  0x1C,
  0x18,
  0xC0,
  0x03,
  0x38,
  0x1C,
  0xC0,
  0x03,
  0x38,
  0x0E,
  0x80,
  0x01,
  0x70,
  0x0E,
  0x00,
  0x00,
  0x70,
  0xFF,
  0xFF,
  0xFF,
  0xFF,
  0xFF,
  0xFF,
  0xFF,
  0xFF,
  0xFE,
  0xFF,
  0xFF,
  0x3F,
};
static const unsigned char bmp2[] U8X8_PROGMEM = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0xFF,
  0xFF,
  0xFF,
  0x3F,
  0xFF,
  0xFF,
  0xFF,
  0x3F,
  0x03,
  0x00,
  0x00,
  0x30,
  0x13,
  0x00,
  0x30,
  0x30,
  0x33,
  0x00,
  0x78,
  0x30,
  0x73,
  0x00,
  0x78,
  0x30,
  0x73,
  0x00,
  0x78,
  0x30,
  0xF3,
  0x00,
  0x78,
  0x30,
  0xF3,
  0x00,
  0x78,
  0xE0,
  0xF3,
  0x00,
  0x78,
  0xE0,
  0xF3,
  0x01,
  0x78,
  0xE0,
  0xF3,
  0x01,
  0x30,
  0xE0,
  0xF3,
  0x03,
  0x30,
  0xE0,
  0xF3,
  0x03,
  0x30,
  0xE0,
  0xF3,
  0x07,
  0x30,
  0xE0,
  0xF3,
  0x0F,
  0x30,
  0x30,
  0xF3,
  0x0F,
  0x00,
  0x30,
  0xF3,
  0x1F,
  0x30,
  0x30,
  0xF3,
  0x1F,
  0x78,
  0x30,
  0xF3,
  0x3F,
  0x78,
  0x30,
  0x03,
  0x00,
  0x30,
  0x30,
  0x03,
  0x00,
  0x00,
  0x30,
  0xFF,
  0xFF,
  0xFF,
  0x3F,
  0xFF,
  0xFF,
  0xFF,
  0x3F,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
};
/*初始化*/
void setup() {
  // put your setup code here, to run once:
  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(left, INPUT_PULLUP);
  pinMode(right, INPUT_PULLUP);
  pinMode(mid, INPUT_PULLUP);
  pinMode(buck_sw, OUTPUT);
  pinMode(buzz, OUTPUT);
  pinMode(adc_temp, INPUT);
  pinMode(adc_volt, INPUT);
  ledcSetup(laser_pwm, 5000, 8);
  ledcSetup(sign_pwm, 5000, 8);
  ledcAttachPin(laser_out, laser_pwm);
  ledcAttachPin(sign_out, sign_pwm);
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  u8g2.setFontDirection(0);
  EEPROM.begin(1024);
  digitalWrite(buck_sw, HIGH);  //输出高电平，光耦导通拉低on脚默认关闭降压模块
}
/*主程序*/
void loop() {
  // put your main code here, to run repeatedly:
  int menu_num_x = 1, menu_num_y = 1, menu_num_ref;  //菜单光标操作变量和刷新变量
  bool refresh, refresh_0 = 1;                       //光标刷新标志和首次刷新标志
  /*显示主菜单*/
  u8g2.clearBuffer();
  u8g2.setCursor(35, 15);
  u8g2.print("手持激光器");
  u8g2.setCursor(10, 35);
  u8g2.print("持续出光");
  u8g2.setCursor(10, 50);
  u8g2.print("瞬间出光");
  u8g2.setCursor(70, 35);
  u8g2.print("参数设置");
  u8g2.setCursor(70, 50);
  u8g2.print("设备状态");
  u8g2.sendBuffer();
  /*菜单选择*/
  while (1) {
    menu_num_ref = menu_num_x;  //记录初值
    if (!digitalRead(down)) {
      menu_num_y++;
      delay(150);
    }
    if (!digitalRead(up)) {
      menu_num_y--;
      delay(150);
    }
    if (menu_num_y > 4) menu_num_y = 1;
    if (menu_num_y < 1) menu_num_y = 4;  //滚动
    switch (menu_num_y) {
      case 1:
        {
          menu_num_x = 1;
          break;
        }
      case 2:
        {
          menu_num_x = 3;
          break;
        }
      case 3:
        {
          menu_num_x = 2;
          break;
        }
      case 4:
        {
          menu_num_x = 4;
          break;
        }
    }  //yx序号同步
    if (!digitalRead(right)) {
      menu_num_x++;
      delay(150);
    }
    if (!digitalRead(left)) {
      menu_num_x--;
      delay(150);
    }
    if (menu_num_x > 4) menu_num_x = 1;
    if (menu_num_x < 1) menu_num_x = 4;  //滚动
    switch (menu_num_x) {
      case 1:
        {
          menu_num_y = 1;
          break;
        }
      case 2:
        {
          menu_num_y = 3;
          break;
        }
      case 3:
        {
          menu_num_y = 2;
          break;
        }
      case 4:
        {
          menu_num_y = 4;
          break;
        }
    }  //xy序号同步
    if (menu_num_x != menu_num_ref) {
      refresh = 1;
    } else {
      refresh = 0;
    }  //判断光标是否需要刷新
    if (refresh_0) {
      refresh = 1;
      refresh_0 = 0;  //首次刷新例外处理
    }
    /*显示光标*/
    if (refresh) {
      u8g2.setDrawColor(0);        // 0 表示背景色
      u8g2.drawBox(4, 25, 5, 30);  //起点x，y；长，宽
      u8g2.drawBox(65, 25, 5, 30);
      u8g2.sendBuffer();
      u8g2.setDrawColor(1);
      refresh = 0;
      switch (menu_num_y) {
        case 1:
          {
            u8g2.setCursor(4, 35);
            u8g2.print(">");
            u8g2.sendBuffer();
            break;
          }
        case 2:
          {
            u8g2.setCursor(4, 50);
            u8g2.print(">");
            u8g2.sendBuffer();
            break;
          }
        case 3:
          {
            u8g2.setCursor(65, 35);
            u8g2.print(">");
            u8g2.sendBuffer();
            break;
          }
        case 4:
          {
            u8g2.setCursor(65, 50);
            u8g2.print(">");
            u8g2.sendBuffer();
            break;
          }
      }
    }
    /*进入下级菜单*/
    if (!digitalRead(mid)) {
      while (!digitalRead(mid))
        ;  //防止连进两级
      switch (menu_num_y) {
        case 1:
          {
            OUT_L();  //持续出光
            break;
          }
        case 2:
          {
            OUT_S();  //爆闪出光
            break;
          }
        case 3:
          {
            SET();  //参数设置
            break;
          }
        case 4:
          {
            Condition();  //设备状态
            break;
          }
      }
      break;  //跳出菜单选择以刷新主菜单显示
    }
  }
}

/*参数设置菜单*/
void SET() {
  bool setMenu = 1, refresh = 1;  //菜单循环控制,光标刷新控制
  int set_cur = 1, set_cur_ref;
  while (setMenu) {  //菜单循环
    /*显示菜单页面*/
    u8g2.clearBuffer();
    u8g2.setCursor(15, 15);
    u8g2.print("激光功率");
    u8g2.setCursor(15, 30);
    u8g2.print("闪亮时长");
    u8g2.setCursor(15, 45);
    u8g2.print("指示亮度");
    u8g2.setCursor(15, 60);
    u8g2.print("警告音开关");
    u8g2.sendBuffer();
    /*菜单选择*/
    while (1) {
      set_cur_ref = set_cur;
      if (!digitalRead(down)) {
        set_cur++;
        while (!digitalRead(down))
          ;
      }
      if (!digitalRead(up)) {
        set_cur--;
        while (!digitalRead(up))
          ;
      }
      if (set_cur > 4) set_cur = 1;
      if (set_cur < 1) set_cur = 4;             //滚动
      if (set_cur != set_cur_ref) refresh = 1;  //决定是否刷新
      /*显示光标*/
      if (refresh) {
        u8g2.setDrawColor(0);        // 0 表示背景色
        u8g2.drawBox(5, 5, 10, 55);  //起点x，y；长，宽
        u8g2.sendBuffer();
        u8g2.setDrawColor(1);
        refresh = 0;  //防止重复刷新
        switch (set_cur) {
          case 1:
            {
              u8g2.setCursor(5, 15);
              u8g2.print(">");
              u8g2.sendBuffer();
              break;
            }
          case 2:
            {
              u8g2.setCursor(5, 30);
              u8g2.print(">");
              u8g2.sendBuffer();
              break;
            }
          case 3:
            {
              u8g2.setCursor(5, 45);
              u8g2.print(">");
              u8g2.sendBuffer();
              break;
            }
          case 4:
            {
              u8g2.setCursor(5, 60);
              u8g2.print(">");
              u8g2.sendBuffer();
              break;
            }
        }
      }
      /*进入调节页面*/
      if (digitalRead(mid) == LOW || digitalRead(right) == LOW) {
        while (digitalRead(mid) == LOW || digitalRead(right) == LOW)
          ;  //防止连进两级
        switch (set_cur) {
          case 1:
            {
              pwmADJ();  //功率调节
              break;
            }
          case 2:
            {
              timADJ();  //时长调节
              break;
            }
          case 3:
            {
              crossADJ();  //指示线亮度调节
              break;
            }
          case 4:
            {
              alarmSET();  //警告音设置
              break;
            }
        }
        refresh = 1;  //强制重新刷新光标，如果不刷新会导致光标消失，原因尚不明确
        break;        //跳出菜单选择以重新显示设置菜单
      }
      if (!digitalRead(left)) {  //向左跳出循环返回主菜单，不按则退出调节后继续显示该级菜单
        while (!digitalRead(left))
          ;
        setMenu = 0;  //关闭菜单循环，退出该级菜单
        break;        //退出菜单选择循环
      }
    }
  }
}
/*激光功率调节子菜单*/
void pwmADJ() {
  bool refresh = 0;
  int laser_pct = readSETi(100);  //读取设置
  while (1) {
    u8g2.clearBuffer();
    u8g2.setCursor(5, 20);
    u8g2.print("持续出光功率（％）：");
    u8g2.setCursor(20, 60);
    u8g2.print("按中键保存设置");
    u8g2.setCursor(50, 40);
    u8g2.print(laser_pct);
    u8g2.sendBuffer();
    if (digitalRead(down) == LOW && laser_pct > laser_pct_min) {
      laser_pct--;
      refresh = 1;
      delay(150);
    }
    if (digitalRead(up) == LOW && laser_pct < laser_pct_max) {
      laser_pct++;
      refresh = 1;
      delay(150);
    }
    if (refresh) {
      u8g2.setDrawColor(0);          // 0 表示背景色
      u8g2.drawBox(50, 20, 20, 20);  //起点x，y；长，宽
      u8g2.sendBuffer();
      u8g2.setDrawColor(1);
      refresh = 0;
    }
    /*保存配置及动画*/
    if (!digitalRead(mid)) {
      writeSETi(100, laser_pct);     //写配置
      u8g2.setDrawColor(0);          // 0 表示背景色
      u8g2.drawBox(20, 50, 90, 20);  //起点x，y；长，宽
      u8g2.sendBuffer();
      u8g2.setDrawColor(1);
      u8g2.setCursor(30, 60);
      u8g2.print("参数已保存");
      u8g2.sendBuffer();
      delay(1000);
    }
    if (!digitalRead(left)) {
      while (!digitalRead(left))
        ;
      break;
    }
  }
}
/*爆闪时长调节子菜单*/
void timADJ() {
  bool refresh = 0;
  int laser_tim = readSETi(200);  //读取配置
  while (1) {
    u8g2.clearBuffer();
    u8g2.setCursor(5, 20);
    u8g2.print("爆闪时长（毫秒）：");
    u8g2.setCursor(20, 60);
    u8g2.print("按中键保存设置");
    u8g2.setCursor(50, 40);
    u8g2.print(laser_tim * 10);
    u8g2.sendBuffer();
    if (digitalRead(down) == LOW && laser_tim > 50) {
      laser_tim = laser_tim - 50;
      refresh = 1;
      delay(150);
    }
    if (digitalRead(up) == LOW && laser_tim < 200) {
      laser_tim = laser_tim + 50;
      refresh = 1;
      delay(150);
    }
    if (refresh) {
      u8g2.setDrawColor(0);          // 0 表示背景色
      u8g2.drawBox(50, 20, 20, 20);  //起点x，y；长，宽
      u8g2.sendBuffer();
      u8g2.setDrawColor(1);
      refresh = 0;
    }
    /*保存配置及动画*/
    if (!digitalRead(mid)) {
      writeSETi(200, laser_tim);     //写配置
      u8g2.setDrawColor(0);          // 0 表示背景色
      u8g2.drawBox(20, 50, 90, 20);  //起点x，y；长，宽
      u8g2.sendBuffer();
      u8g2.setDrawColor(1);
      u8g2.setCursor(30, 60);
      u8g2.print("参数已保存");
      u8g2.sendBuffer();
      delay(1000);
    }
    if (!digitalRead(left)) {
      while (!digitalRead(left))
        ;
      break;
    }
  }
}
/*指示线亮度调节子菜单*/
void crossADJ() {
  bool refresh = 0;
  int sign_pct = readSETi(300);  //读取配置
  while (1) {
    int pwm = map(sign_pct, 0, 100, 0, 255);
    ledcWrite(sign_pwm, pwm);//亮度预览
    u8g2.clearBuffer();
    u8g2.setCursor(5, 20);
    u8g2.print("指示线亮度（％）：");
    u8g2.setCursor(20, 60);
    u8g2.print("按中键保存设置");
    u8g2.setCursor(50, 40);
    u8g2.print(sign_pct);
    u8g2.sendBuffer();
    if (digitalRead(down) == LOW && sign_pct > sign_pct_min) {
      sign_pct--;
      refresh = 1;
      delay(150);
    }
    if (digitalRead(up) == LOW && sign_pct < sign_pct_max) {
      sign_pct++;
      refresh = 1;
      delay(150);
    }
    if (refresh) {
      u8g2.setDrawColor(0);          // 0 表示背景色
      u8g2.drawBox(50, 20, 20, 20);  //起点x，y；长，宽
      u8g2.sendBuffer();
      u8g2.setDrawColor(1);
      refresh = 0;
    }
    /*保存配置及动画*/
    if (!digitalRead(mid)) {
      writeSETi(300, sign_pct);      //写配置
      u8g2.setDrawColor(0);          // 0 表示背景色
      u8g2.drawBox(20, 50, 90, 20);  //起点x，y；长，宽
      u8g2.sendBuffer();
      u8g2.setDrawColor(1);
      u8g2.setCursor(30, 60);
      u8g2.print("参数已保存");
      u8g2.sendBuffer();
      delay(1000);
    }
    if (!digitalRead(left)) {
      while (!digitalRead(left))
        ;
      ledcWrite(sign_pwm, 0);//关闭亮度预览
      break;
    }
  }
}
/*蜂鸣设置子菜单*/
void alarmSET() {
  bool setMenu = 1, refresh = 1;  //菜单循环控制，光标刷新控制,开关控件刷新控制（刷新是覆盖，所以第一次关闭）
  bool alm_out = readSETb(400);   //下面依次读取flash中输出提示、过热报警、电量不足告警开关的配置
  bool alm_tmp = readSETb(500);
  bool alm_bat = readSETb(600);
  int set_cur = 1, set_cur_ref;
  while (setMenu) {  //菜单循环
    /*显示菜单页面和开关控件*/
    u8g2.clearBuffer();
    u8g2.setCursor(35, 15);
    u8g2.print("警告音设置");
    u8g2.setCursor(15, 30);
    u8g2.print("输出提示：");
    u8g2.setCursor(15, 45);
    u8g2.print("过热警告：");
    u8g2.setCursor(15, 60);
    u8g2.print("低电警告：");
    u8g2.setCursor(80, 30);
    if (alm_out) {
      u8g2.print("开");
    } else {
      u8g2.print("关");
    }
    u8g2.setCursor(80, 45);
    if (alm_tmp) {
      u8g2.print("开");
    } else {
      u8g2.print("关");
    }
    u8g2.setCursor(80, 60);
    if (alm_bat) {
      u8g2.print("开");
    } else {
      u8g2.print("关");
    }
    u8g2.sendBuffer();
    /*菜单选择与控件刷新*/
    while (1) {
      set_cur_ref = set_cur;
      if (!digitalRead(down)) {
        set_cur++;
        delay(150);
      }
      if (!digitalRead(up)) {
        set_cur--;
        delay(150);
      }
      if (set_cur > 3) set_cur = 1;
      if (set_cur < 1) set_cur = 3;             //滚动
      if (set_cur != set_cur_ref) refresh = 1;  //决定是否刷新光标
      /*显示光标*/
      if (refresh) {
        u8g2.setDrawColor(0);         // 0 表示背景色
        u8g2.drawBox(5, 15, 10, 45);  //起点x，y；长，宽
        u8g2.sendBuffer();
        u8g2.setDrawColor(1);
        refresh = 0;
        switch (set_cur) {
          case 1:
            {
              u8g2.setCursor(5, 30);
              u8g2.print(">");
              u8g2.sendBuffer();
              break;
            }
          case 2:
            {
              u8g2.setCursor(5, 45);
              u8g2.print(">");
              u8g2.sendBuffer();
              break;
            }
          case 3:
            {
              u8g2.setCursor(5, 60);
              u8g2.print(">");
              u8g2.sendBuffer();
              break;
            }
        }
      }

      /*进入调节页面&刷新控件*/
      if (!digitalRead(mid)) {
        while (!digitalRead(mid))
          ;  //防止反复切换
        switch (set_cur) {
          case 1:
            {
              alm_out = !alm_out;
              break;
            }
          case 2:
            {
              alm_tmp = !alm_tmp;
              break;
            }
          case 3:
            {
              alm_bat = !alm_bat;
              break;
            }
        }
        /*覆盖开关控件*/
        u8g2.setDrawColor(0);          // 0 表示背景色
        u8g2.drawBox(80, 15, 15, 50);  //起点x，y；长，宽
        u8g2.sendBuffer();
        u8g2.setDrawColor(1);
        refresh = 1;  //强制重新刷新光标，如果不刷新会导致光标消失，原因尚不明确
        break;        //跳出菜单选择以重新显示设置菜单
      }
      if (!digitalRead(left)) {  //向左跳出循环返回主菜单，不按则退出调节后继续显示该级菜单
        while (!digitalRead(left))
          ;
        setMenu = 0;  //关闭菜单循环，退出该菜单
        break;        //退出菜单交互循环
      }
    }
  }
  /*退出设置菜单时写入配置到flash*/
  writeSETb(400, alm_out);
  writeSETb(500, alm_tmp);
  writeSETb(600, alm_bat);
}
/*出光控制*/
void OUT_L() {
  int flash = readSETi(100);
  int pwm = map(flash, 0, 100, 0, 255);
  bool sw = 0, out = 1;
  while (out) {
    if (OverHeat()) {
      sw = 0;
      out = 0;
    }
    if (BatteryLow()) {
      sw = 0;
      out = 0;
    }
    sign(1);  //开启指示线灯
    u8g2.clearBuffer();
    u8g2.setCursor(20, 20);
    u8g2.print("按中键开启输出");
    u8g2.setCursor(10, 50);
    u8g2.print("功率百分比：");
    u8g2.setCursor(85, 50);
    u8g2.print(flash);
    u8g2.setCursor(102, 50);
    u8g2.print("％");
    u8g2.sendBuffer();
    /*启动激光输出*/
    if (!digitalRead(mid)) {
      while(!digitalRead(mid));
      digitalWrite(buck_sw, LOW);    //开启降压模块
      delay(50);                     //缓启动
      ledcWrite(laser_pwm, pwm);     //按设定功率开启pwm
      beep(1);                       //声音提示
      u8g2.setDrawColor(0);          // 0 表示背景色
      u8g2.drawBox(10, 10, 70, 20);  //起点x，y；长，宽
      u8g2.sendBuffer();
      u8g2.setDrawColor(1);
      u8g2.setCursor(15, 20);
      u8g2.print("按任意键关闭输出");
      u8g2.sendBuffer();
      sw = 1;
    }
    /*输出时处理*/
    while (sw) {
      if (OverHeat()) {
        sw = 0;
        out = 0;
      }
      if (BatteryLow()) {
        sw = 0;
        out = 0;
      }
      if (digitalRead(up) == LOW || digitalRead(down) == LOW || digitalRead(left) == LOW || digitalRead(right) == LOW || digitalRead(mid) == LOW) {
        ledcWrite(laser_pwm, 0);      //占空比归零
        digitalWrite(buck_sw, HIGH);  //关闭降压模块
        sw = 0;
        while (!digitalRead(mid))
          ;  //防止再次开启
      }
    }
    /*退出处理*/
    if (!digitalRead(left)) {
      while (!digitalRead(left))
        ;
      break;
    }
  }
  sign(0);
}
/*爆闪控制*/
void OUT_S() {
  int flash = readSETi(200);
  unsigned long t1, dt = flash * 10;
  bool sw = 0, out = 1;
  while (out) {
    if (OverHeat()) {
      break;
    }
    if (BatteryLow()) {
      break;
    }
    sign(1);  //开启指示线灯
    u8g2.clearBuffer();
    u8g2.setCursor(20, 20);
    u8g2.print("按中键启动爆闪");
    u8g2.setCursor(10, 50);
    u8g2.print("爆闪时长：");
    u8g2.setCursor(70, 50);
    u8g2.print(dt);
    u8g2.setCursor(100, 50);
    u8g2.print("毫秒");
    u8g2.sendBuffer();
    /*启动爆闪输出*/
    if (!digitalRead(mid)) {
      while(!digitalRead(mid));
      digitalWrite(buck_sw, LOW);    //开启降压模块
      beep(1);                       //蜂鸣同时起到缓启动作用
      u8g2.setDrawColor(0);          // 0 表示背景色
      u8g2.drawBox(10, 10, 70, 20);  //起点x，y；长，宽
      u8g2.sendBuffer();
      u8g2.setDrawColor(1);
      u8g2.setCursor(15, 20);
      u8g2.print("按任意键关闭输出");
      u8g2.sendBuffer();
      sw = 1;
      ledcWrite(laser_pwm, 255);  //满功率开启pwm
      t1 = millis();              //开始计时
    }
    while (sw) {
      if (OverHeat()) {
        sw = 0;
        out = 0;
      }
      if (BatteryLow()) {
        sw = 0;
        out = 0;
      }
      if (digitalRead(up) == LOW || digitalRead(down) == LOW || digitalRead(left) == LOW || digitalRead(right) == LOW || digitalRead(mid) == LOW) {
        ledcWrite(laser_pwm, 0);      //占空比归零
        digitalWrite(buck_sw, HIGH);  //关闭降压模块
        sw = 0;
        while (!digitalRead(mid))
          ;  //防止再次开启
      }
      /*计时关闭输出*/
      if (millis() - t1 >= dt) {
        ledcWrite(laser_pwm, 0);      //占空比归零
        digitalWrite(buck_sw, HIGH);  //关闭降压模块
        sw = 0;
      }
    }
    /*退出处理*/
    if (!digitalRead(left)) {
      while (!digitalRead(left))
        ;
      break;
    }
  }
  sign(0);
}
/*系统状态显示页面*/
void Condition() {
  int upNum = 0, downNum = 0;
  bool console = 0, next = 0;
  BatteryLow();
  OverHeat();
  while (1) {
    u8g2.clearBuffer();
    u8g2.setCursor(5, 20);
    u8g2.print("设备温度：");
    u8g2.setCursor(65, 20);
    u8g2.print(adcTEMP());
    u8g2.setCursor(100, 20);
    u8g2.print("℃");
    u8g2.setCursor(5, 40);
    u8g2.print("电池电压：");
    u8g2.setCursor(65, 40);
    u8g2.print(adcVOLT());
    u8g2.setCursor(100, 40);
    u8g2.print("V");
    u8g2.sendBuffer();
    delay(50);
    if (digitalRead(left) == LOW) {
      while (!digitalRead(left))
        ;
      break;
    }
    if (digitalRead(mid) == LOW && console == 0) {  //由于中键同时作为进入调试菜单之用，这里单独处理
      while (!digitalRead(mid))
        ;
      break;
    }
    /*进入调试模式*/
    if (!digitalRead(up)) {
      while (!digitalRead(up))
        ;
      upNum++;
    }
    if (upNum >= 3) next = 1;
    if (digitalRead(down) == LOW && next == 1) {
      while (!digitalRead(down))
        ;
      downNum++;
    }
    if (downNum >= 3) console = 1;
    if (digitalRead(mid) == LOW && console == 1) {  //进入调试菜单
      while (!digitalRead(mid))
        ;
      Console();
      upNum = 0;
      downNum = 0;
      next = 0;
      console = 0;
    }
  }
}
/*调试模式菜单*/
void Console() {
  bool conMenu = 1, refresh = 1;  //菜单循环控制,光标刷新控制
  int con_cur = 1, con_cur_ref;
  while (conMenu) {  //菜单循环
    /*显示菜单页面*/
    u8g2.clearBuffer();
    u8g2.setCursor(25, 15);
    u8g2.print("调试模式菜单");
    u8g2.setCursor(15, 35);
    u8g2.print("红外瞄准瞬时出光");
    u8g2.setCursor(15, 50);
    u8g2.print("激光光瞄校准");
    u8g2.sendBuffer();
    /*菜单选择*/
    while (1) {
      con_cur_ref = con_cur;
      if (!digitalRead(down)) {
        con_cur++;
        while (!digitalRead(down))
          ;
      }
      if (!digitalRead(up)) {
        con_cur--;
        while (!digitalRead(up))
          ;
      }
      if (con_cur > 2) con_cur = 1;
      if (con_cur < 1) con_cur = 2;             //滚动
      if (con_cur != con_cur_ref) refresh = 1;  //决定是否刷新
      /*显示光标*/
      if (refresh) {
        u8g2.setDrawColor(0);        // 0 表示背景色
        u8g2.drawBox(5, 5, 10, 55);  //起点x，y；长，宽
        u8g2.sendBuffer();
        u8g2.setDrawColor(1);
        refresh = 0;  //防止重复刷新
        switch (con_cur) {
          case 1:
            {
              u8g2.setCursor(5, 35);
              u8g2.print(">");
              u8g2.sendBuffer();
              break;
            }
          case 2:
            {
              u8g2.setCursor(5, 50);
              u8g2.print(">");
              u8g2.sendBuffer();
              break;
            }
        }
      }
      /*进入调节页面*/
      if (digitalRead(mid) == LOW || digitalRead(right) == LOW) {
        while (digitalRead(mid) == LOW || digitalRead(right) == LOW)
          ;  //防止连进两级
        switch (con_cur) {
          case 1:
            {
              OUT_S_IRaim();  //红外瞄准模式
              break;
            }
          case 2:
            {
              Optcal();  //光路校准模式
              break;
            }
        }
        refresh = 1;  //强制重新刷新光标，如果不刷新会导致光标消失，原因尚不明确
        break;        //跳出菜单选择以重新显示设置菜单
      }
      if (!digitalRead(left)) {  //向左跳出循环返回主菜单，不按则退出调节后继续显示该级菜单
        while (!digitalRead(left))
          ;
        conMenu = 0;  //关闭菜单循环，退出该级菜单
        break;        //退出菜单选择循环
      }
    }
  }
}
/*红外瞄准模式*/
void OUT_S_IRaim() {
  int flash = readSETi(200);
  unsigned long t1, dt = flash * 10;
  bool sw = 0, out = 1;
  int signpw = map(laser_pct_cal, 0, 100, 0, 255);
  digitalWrite(buck_sw, LOW);  //开启降压模块
  delay(50);
  ledcWrite(laser_pwm, signpw);  //以校准功率开启pwm
  while (out) {
    if (OverHeat()) {
      break;
    }
    if (BatteryLow()) {
      break;
    }
    u8g2.clearBuffer();
    u8g2.setCursor(20, 20);
    u8g2.print("按中键启动爆闪");
    u8g2.setCursor(10, 50);
    u8g2.print("爆闪时长：");
    u8g2.setCursor(70, 50);
    u8g2.print(dt);
    u8g2.setCursor(100, 50);
    u8g2.print("毫秒");
    u8g2.sendBuffer();
    /*启动爆闪输出*/
    if (!digitalRead(mid)) {
      while(!digitalRead(mid));
      beep(1);                       //蜂鸣同时起到缓启动作用
      u8g2.setDrawColor(0);          // 0 表示背景色
      u8g2.drawBox(10, 10, 70, 20);  //起点x，y；长，宽
      u8g2.sendBuffer();
      u8g2.setDrawColor(1);
      u8g2.setCursor(15, 20);
      u8g2.print("按任意键关闭输出");
      u8g2.sendBuffer();
      sw = 1;
      ledcWrite(laser_pwm, 255);  //满功率开启pwm
      t1 = millis();              //开始计时
    }
    while (sw) {
      if (OverHeat()) {
        sw = 0;
        out = 0;
      }
      if (BatteryLow()) {
        sw = 0;
        out = 0;
      }
      if (digitalRead(up) == LOW || digitalRead(down) == LOW || digitalRead(left) == LOW || digitalRead(right) == LOW || digitalRead(mid) == LOW) {
        ledcWrite(laser_pwm, signpw);  //占空比回到校准模式功率
        sw = 0;
        while (!digitalRead(mid))
          ;  //防止再次开启
      }
      /*计时关闭输出*/
      if (millis() - t1 >= dt) {
        ledcWrite(laser_pwm, signpw);  //占空比回到校准模式功率
        sw = 0;
      }
    }
    /*退出处理*/
    if (!digitalRead(left)) {
      while (!digitalRead(left))
        ;
      break;
    }
  }
  ledcWrite(laser_pwm, 0);      //占空比置零
  digitalWrite(buck_sw, HIGH);  //关闭降压模块
}
/*光路校准模式*/
void Optcal() {
  int pwm = map(laser_pct_cal, 0, 100, 0, 255);
  bool sw = 0, out = 1;
  while (out) {
    if (OverHeat()) {
      sw = 0;
      out = 0;
    }
    if (BatteryLow()) {
      sw = 0;
      out = 0;
    }
    sign(1);  //开启指示线灯
    u8g2.clearBuffer();
    u8g2.setCursor(20, 20);
    u8g2.print("按中键开启输出");
    u8g2.setCursor(10, 50);
    u8g2.print("光路校准模式：");
    u8g2.setCursor(95, 50);
    u8g2.print(laser_pct_cal);
    u8g2.setCursor(102, 50);
    u8g2.print("％");
    u8g2.sendBuffer();
    /*启动激光输出*/
    if (!digitalRead(mid)) {
      while(!digitalRead(mid));
      digitalWrite(buck_sw, LOW);    //开启降压模块
      delay(50);                     //缓启动
      ledcWrite(laser_pwm, pwm);     //按设定功率开启pwm
      beep(1);                       //声音提示
      u8g2.setDrawColor(0);          // 0 表示背景色
      u8g2.drawBox(10, 10, 70, 20);  //起点x，y；长，宽
      u8g2.sendBuffer();
      u8g2.setDrawColor(1);
      u8g2.setCursor(15, 20);
      u8g2.print("按任意键关闭输出");
      u8g2.sendBuffer();
      sw = 1;
    }
    /*输出时处理*/
    while (sw) {
      if (OverHeat()) {
        sw = 0;
        out = 0;
      }
      if (BatteryLow()) {
        sw = 0;
        out = 0;
      }
      if (digitalRead(up) == LOW || digitalRead(down) == LOW || digitalRead(left) == LOW || digitalRead(right) == LOW || digitalRead(mid) == LOW) {
        ledcWrite(laser_pwm, 0);      //占空比归零
        digitalWrite(buck_sw, HIGH);  //关闭降压模块
        sw = 0;
        while (!digitalRead(mid))
          ;  //防止重复开启
      }
    }
    /*退出处理*/
    if (!digitalRead(left)) {
      while (!digitalRead(left))
        ;
      break;
    }
  }
  sign(0);
}
/*ADC温度计算*/
float adcTEMP() {
  float vTemp = analogReadMilliVolts(adc_temp) / 1000.00;
  float ntc = 10000 * (3.3 - vTemp) / vTemp;
  float temp = 1.0 / (1.0 / 298.15 + (1.0 / 3380.0) * log(ntc / 10000.0)) - 273.15;
  return temp;
}
/*ADC电压计算*/
float adcVOLT() {
  float batVolt = analogReadMilliVolts(adc_volt) / 1000.00 * 5.00;
  return batVolt;
}
/*蜂鸣器控制*/
void beep(int a) {
  bool alm_out = readSETb(400);  //下面依次读取flash中输出提示、过热报警、电量不足告警开关的配置
  bool alm_tmp = readSETb(500);
  bool alm_bat = readSETb(600);
  if (a == 1 && alm_out == 1) {
    digitalWrite(buzz, HIGH);
    delay(500);
    digitalWrite(buzz, LOW);
  } else if (a == 2 && alm_tmp == 1) {
    digitalWrite(buzz, HIGH);
    delay(2000);
    digitalWrite(buzz, LOW);
  } else if (a == 3 && alm_bat == 1) {
    digitalWrite(buzz, HIGH);
    delay(700);
    digitalWrite(buzz, LOW);
    delay(200);
    digitalWrite(buzz, HIGH);
    delay(700);
    digitalWrite(buzz, LOW);
    delay(200);
    digitalWrite(buzz, HIGH);
    delay(700);
    digitalWrite(buzz, LOW);
  }
}
/*指示线灯控制*/
void sign(int a) {
  int flash = readSETi(300);
  int pwm = map(flash, 0, 100, 0, 255);
  if (a == 1) {
    ledcWrite(sign_pwm, pwm);
  } else if (a == 2) {
    for (int i = 1; i <= 5; i++) {
      ledcWrite(sign_pwm, 255);
      delay(100);
      ledcWrite(sign_pwm, 0);
      delay(100);
    }
  } else if (a == 0) {
    ledcWrite(sign_pwm, 0);
  }
}
/*过热保护控制*/
bool OverHeat() {
  if (adcTEMP() >= temp_max) {
    ledcWrite(laser_pwm, 0);
    digitalWrite(buck_sw, HIGH);  //关闭pwm同时关闭降压模块
    u8g2.clearBuffer();
    u8g2.drawXBMP(50, 0, 32, 29, bmp1);
    u8g2.setCursor(20, 45);
    u8g2.print("设备过热，请关闭");
    u8g2.setCursor(5, 60);
    u8g2.print("并静置冷却！");
    u8g2.sendBuffer();
    sign(2);
    beep(2);
    return 1;
  } else {
    return 0;
  }
}
/*电池低电保护*/
bool BatteryLow() {
  if (adcVOLT() <= bat_low) {
    ledcWrite(laser_pwm, 0);
    digitalWrite(buck_sw, HIGH);  //关闭pwm同时关闭降压模块
    u8g2.clearBuffer();
    u8g2.drawXBMP(50, 0, 32, 29, bmp2);
    u8g2.setCursor(20, 45);
    u8g2.print("电池电量低，请关闭设备");
    u8g2.setCursor(5, 60);
    u8g2.print("并及时充电！");
    u8g2.sendBuffer();
    sign(2);
    beep(3);
    return 1;
  } else {
    return 0;
  }
}

/*参数设置类读取封装函数*/
int readSETi(int addr) {  //arduino不支持函数重载，只能加i或b区分返回值
  int value, sign;
  EEPROM.get(addr - 50, sign);
  if (addr == 100) {
    if (sign == 22) {
      EEPROM.get(addr, value);
      return value;
    } else {
      return 60;  //默认激光功率60%
    }
  } else if (addr == 200) {
    if (sign == 22) {
      EEPROM.get(addr, value);
      return value;
    } else {
      return 50;  //默认爆闪时间0.5s，此处除以10保存
    }
  } else if (addr == 300) {
    if (sign == 22) {
      EEPROM.get(addr, value);
      return value;
    } else {
      return 80;  //默认指示亮度80%
    }
  }
}
/*开关设置类读取封装函数*/
bool readSETb(int addr) {
  bool value;
  int sign;
  EEPROM.get(addr - 50, sign);  //初始化判定
  if (addr == 400) {
    if (sign == 22) {
      EEPROM.get(addr, value);
      return value;
    } else {
      return 1;  //告警蜂鸣都默认开启
    }
  } else if (addr == 500) {
    if (sign == 22) {
      EEPROM.get(addr, value);
      return value;
    } else {
      return 1;
    }
  } else if (addr == 600) {
    if (sign == 22) {
      EEPROM.get(addr, value);
      return value;
    } else {
      return 1;
    }
  }
}
/*参数设置类写入封装函数*/
void writeSETi(int addr, int value) {
  int sign, value_0;  //初始化标志位读取，先前数据读取
  EEPROM.get(addr - 50, sign);
  EEPROM.get(addr, value_0);
  if (sign != 22) {  //当数据更新时写入，降低flash寿命损耗，下同
    EEPROM.put(addr - 50, 22);
    EEPROM.commit();
  }
  if (value_0 != value) {
    EEPROM.put(addr, value);
    EEPROM.commit();
  }
}
/*开关设置类写入封装函数*/
void writeSETb(int addr, bool value) {
  int sign;
  bool value_0;
  EEPROM.get(addr - 50, sign);
  EEPROM.get(addr, value_0);
  if (sign != 22) {
    EEPROM.put(addr - 50, 22);
    EEPROM.commit();
  }
  if (value_0 != value) {
    EEPROM.put(addr, value);
    EEPROM.commit();
  }
}