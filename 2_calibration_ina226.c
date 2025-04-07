#include <Wire.h>
#include <INA226.h>

// 创建 INA226 对象
INA226 ina226(Wire);

// 初始取样电阻值（单位：欧姆）
float shuntResistance = 0.1;
// 初始电压校准系数
float voltageCalibrationFactor = 1.0;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    // 初始化 INA226
    if (!ina226.begin()) {
        Serial.println("Failed to initialize INA226.");
        while (1);
    }

    // 初始校准
    calibrateINA226();
    Serial.println("Initial calibration completed.");
    Serial.println("Enter 'R=xxx' to set shunt resistance (in Ohms), e.g., R=0.05");
    Serial.println("Enter 'V=xxx' to set voltage calibration factor, e.g., V=1.02");
    Serial.println("Enter 'q' to quit.");
}

void loop() {
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');

        if (input == "q") {
            Serial.println("Quitting calibration mode.");
            return;
        }

        if (input.startsWith("R=")) {
            String resistanceStr = input.substring(2);
            float newResistance = resistanceStr.toFloat();
            if (newResistance > 0) {
                shuntResistance = newResistance;
                calibrateINA226();
                Serial.print("Calibrated with new shunt resistance: ");
                Serial.print(shuntResistance);
                Serial.println(" Ohms");
            } else {
                Serial.println("Invalid resistance value. Please enter a positive number.");
            }
        } else if (input.startsWith("V=")) {
            String voltageFactorStr = input.substring(2);
            float newVoltageFactor = voltageFactorStr.toFloat();
            if (newVoltageFactor > 0) {
                voltageCalibrationFactor = newVoltageFactor;
                Serial.print("Set new voltage calibration factor: ");
                Serial.println(voltageCalibrationFactor);
            } else {
                Serial.println("Invalid voltage calibration factor. Please enter a positive number.");
            }
        } else {
            Serial.println("Invalid command. Use 'R=xxx' or 'V=xxx' or 'q'.");
        }
    }
}

void calibrateINA226() {
    // 计算校准值
    uint16_t calibrationValue = (uint16_t)((0.00512) / (shuntResistance * 0.001));
    // 设置校准寄存器
    ina226.writeCalibrationRegister(calibrationValue);
}    