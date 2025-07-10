#include <BleGamepad.h>

// 按键配置
#define TOTAL_BUTTONS 14
const uint8_t buttonPins[TOTAL_BUTTONS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

// 编码器配置
#define ENC1_CLK 18
#define ENC1_DT 19
#define ENC1_SW 20
#define ENC2_CLK 21
#define ENC2_DT 22
#define ENC2_SW 23

BleGamepad bleGamepad;

// 方向盘控制变量
int16_t steeringValue = 0;    // 方向盘位置 (-32767~32767)
int16_t throttleValue = 0;    // 油门值 (0~32767)
int16_t brakeValue = 0;       // 刹车值 (0~32767)

// 按钮状态跟踪
bool lastButtonStates[TOTAL_BUTTONS] = {false};
bool lastEnc1BtnState = false;
bool lastEnc2BtnState = false;

// 编码器状态变量
int lastCLK1 = HIGH;
int lastCLK2 = HIGH;
int lastDT1 = HIGH;
int lastDT2 = HIGH;
int32_t encoder1Pos = 0;
int32_t encoder2Pos = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("开始初始化模拟器方向盘...");
  
  // 初始化按键引脚 (启用内部上拉电阻)
  for(int i = 0; i < TOTAL_BUTTONS; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  
  // 初始化编码器引脚 (启用内部上拉电阻)
  pinMode(ENC1_CLK, INPUT_PULLUP);
  pinMode(ENC1_DT, INPUT_PULLUP);
  pinMode(ENC2_CLK, INPUT_PULLUP);
  pinMode(ENC2_DT, INPUT_PULLUP);
  
  // 读取初始状态
  lastCLK1 = digitalRead(ENC1_CLK);
  lastDT1 = digitalRead(ENC1_DT);
  lastCLK2 = digitalRead(ENC2_CLK);
  lastDT2 = digitalRead(ENC2_DT);
  
  // 初始化编码器按钮 (启用内部上拉电阻)
  pinMode(ENC1_SW, INPUT_PULLUP);
  pinMode(ENC2_SW, INPUT_PULLUP);
  
  // 配置蓝牙游戏手柄
  BleGamepadConfiguration config;
  config.setControllerType(CONTROLLER_TYPE_GAMEPAD);
  config.setButtonCount(16);  // 14个按键 + 2个编码器按钮
  config.setAutoReport(false); // 手动发送报告
  
  bleGamepad.begin(&config);
  
  Serial.println("等待蓝牙连接...");
  unsigned long startTime = millis();
  const unsigned long timeout = 30000; // 30秒超时
  
  while(!bleGamepad.isConnected()) {
    delay(500);
    Serial.print(".");
    
    // 添加超时检测
    if(millis() - startTime > timeout) {
      Serial.println("\n蓝牙连接超时，请检查设备！");
      while(1); // 停止执行
    }
  }
  Serial.println("\n方向盘已成功连接!");
}

// 手动处理编码器旋转
void updateEncoder(int clkPin, int dtPin, int &lastCLK, int &lastDT, int32_t &position) {
  int currentCLK = digitalRead(clkPin);
  int currentDT = digitalRead(dtPin);
  
  // 检查状态变化
  if (currentCLK != lastCLK || currentDT != lastDT) {
    // CLK变化时检测方向
    if (currentCLK != lastCLK) {
      if (currentDT != currentCLK) {
        position++; // 顺时针
      } else {
        position--; // 逆时针
      }
    }
    // DT变化时检测方向
    else if (currentDT != lastDT) {
      if (currentCLK == currentDT) {
        position++; // 顺时针
      } else {
        position--; // 逆时针
      }
    }
    
    // 更新状态
    lastCLK = currentCLK;
    lastDT = currentDT;
  }
}

void loop() {
  if(bleGamepad.isConnected()) {
    // 1. 更新编码器位置
    updateEncoder(ENC1_CLK, ENC1_DT, lastCLK1, lastDT1, encoder1Pos);
    updateEncoder(ENC2_CLK, ENC2_DT, lastCLK2, lastDT2, encoder2Pos);
    
    // 2. 读取并更新按键状态
    for(int i = 0; i < TOTAL_BUTTONS; i++) {
      bool currentState = !digitalRead(buttonPins[i]); // 按下时为低电平
      
      // 只发送状态变化
      if(currentState != lastButtonStates[i]) {
        if(currentState) {
          bleGamepad.press(i+1); // 按钮编号从1开始
        } else {
          bleGamepad.release(i+1);
        }
        lastButtonStates[i] = currentState;
      }
    }
    
    // 3. 处理方向盘控制
    static int32_t lastEncoder1Pos = 0;
    if(encoder1Pos != lastEncoder1Pos) {
      int delta = (encoder1Pos - lastEncoder1Pos) * 100; // 调整灵敏度
      steeringValue = constrain(steeringValue + delta, -32767, 32767);
      lastEncoder1Pos = encoder1Pos;
      
      // 方向盘自动回正模拟
      steeringValue = steeringValue * 0.85;
      
      // 更新方向盘位置
      bleGamepad.setLeftThumb(steeringValue, 0);
    }
    
    // 4. 处理油门/刹车控制
    static int32_t lastEncoder2Pos = 0;
    if(encoder2Pos != lastEncoder2Pos) {
      int delta = (encoder2Pos - lastEncoder2Pos) * 80; // 调整灵敏度
      
      if(delta > 0) {
        // 顺时针旋转 - 油门
        throttleValue = constrain(throttleValue + delta, 0, 32767);
      } else {
        // 逆时针旋转 - 刹车
        brakeValue = constrain(brakeValue - delta, 0, 32767);
      }
      lastEncoder2Pos = encoder2Pos;
      
      // 油门/刹车阻力模拟
      throttleValue = throttleValue * 0.92;
      brakeValue = brakeValue * 0.92;
      
      // 更新油门和刹车
      bleGamepad.setThrottle(throttleValue);  // 油门
      bleGamepad.setBrake(brakeValue);        // 刹车
    }
    
    // 5. 读取编码器按钮
    bool enc1BtnState = !digitalRead(ENC1_SW);
    if(enc1BtnState != lastEnc1BtnState) {
      if(enc1BtnState) {
        bleGamepad.press(15); // 编码器1按钮对应按钮15
      } else {
        bleGamepad.release(15);
      }
      lastEnc1BtnState = enc1BtnState;
    }
    
    bool enc2BtnState = !digitalRead(ENC2_SW);
    if(enc2BtnState != lastEnc2BtnState) {
      if(enc2BtnState) {
        bleGamepad.press(16); // 编码器2按钮对应按钮16
      } else {
        bleGamepad.release(16);
      }
      lastEnc2BtnState = enc2BtnState;
    }
    
    // 6. 发送所有更新
    bleGamepad.sendReport();
    
    // 7. 调试输出
    static unsigned long lastPrint = 0;
    if(millis() - lastPrint > 200) {
      Serial.printf("方向盘: %6d | 油门: %5d | 刹车: %5d | 编码器1: %3ld | 编码器2: %3ld\n", 
                   steeringValue, throttleValue, brakeValue, encoder1Pos, encoder2Pos);
      lastPrint = millis();
    }
  }
  
  delay(5); // 更快的轮询速度
}