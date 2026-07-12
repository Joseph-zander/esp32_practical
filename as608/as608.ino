#include <Adafruit_Fingerprint.h>

/**
* AS608   引脚ESP32-C3   引脚说明
* VCC      3.3V           推荐用 3.3V（部分模块支持 5V，但 ESP32-C3 电平是 3.3V）
* GND      GND              共地
* TX       RX（自定义）      模块发送 → 板子接收
* RX       TX（自定义）      模块接收 → 板子发送
**TX → GPIO 21     RX → GPIO 20
*/

// ================== 引脚设置（请修改） ==================
#define FINGER_RX  20   // 接 AS608 的 TX
#define FINGER_TX  21   // 接 AS608 的 RX
// ======================================================

HardwareSerial mySerial(1);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup() {
  Serial.begin(115200);
  delay(100);
  
  mySerial.begin(57600, SERIAL_8N1, FINGER_RX, FINGER_TX);
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("✓ 找到指纹模块！");
  } else {
    Serial.println("✗ 未找到指纹模块，请检查接线");
    while (1) delay(1);
  }

  finger.getParameters();
  printMenu();
}

void loop() {
  if (Serial.available()) {
    int cmd = Serial.parseInt();
    while (Serial.available()) Serial.read();  // 清空缓冲区

    switch (cmd) {
      case 1:  enrollFingerprint();   break;
      case 2:  matchFingerprint();    break;
      case 3:  deleteFingerprint();   break;
      case 4:  emptyDatabase();       break;
      case 5:  showSensorInfo();      break;
      default:
        Serial.println("无效命令");
        printMenu();
        break;
    }
  }
}

// -------------------- 菜单 --------------------
void printMenu() {
  Serial.println("\n========== 指纹管理系统 ==========");
  Serial.println("1 - 录入指纹");
  Serial.println("2 - 识别匹配");
  Serial.println("3 - 删除指定ID");
  Serial.println("4 - 清空所有指纹");
  Serial.println("5 - 查看传感器信息");
  Serial.println("请输入数字命令：");
}

// -------------------- 1. 录入指纹 --------------------
void enrollFingerprint() {
  Serial.println("\n--- 录入指纹 ---");
  Serial.println("请输入要保存的ID (1~127)：");
  
  while (!Serial.available());
  uint8_t id = Serial.parseInt();
  while (Serial.available()) Serial.read();

  if (id < 1 || id > 127) {
    Serial.println("ID无效！");
    return;
  }

  Serial.print("正在录入 ID #");
  Serial.println(id);

  int p = -1;
  Serial.println("请将手指放在传感器上...");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) Serial.print(".");
    delay(50);
  }
  Serial.println("\n图像已采集");

  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    Serial.println("特征提取失败");
    return;
  }

  Serial.println("请移开手指");
  delay(2000);
  while (finger.getImage() != FINGERPRINT_NOFINGER);

  Serial.println("请再次放置同一根手指...");
  p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) Serial.print(".");
    delay(50);
  }
  Serial.println("\n图像已采集");

  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) {
    Serial.println("特征提取失败");
    return;
  }

  p = finger.createModel();
  if (p != FINGERPRINT_OK) {
    Serial.println("两次指纹不匹配，请重试");
    return;
  }

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("✓ 录入成功！");
  } else {
    Serial.println("✗ 存储失败");
  }
  printMenu();
}

// -------------------- 2. 识别匹配 --------------------
void matchFingerprint() {
  Serial.println("\n--- 识别匹配 ---");
  Serial.println("请将手指放在传感器上...");

  int p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) {
      delay(50);
      continue;
    }
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    Serial.println("特征提取失败");
    printMenu();
    return;
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.print("✓ 匹配成功！ID = ");
    Serial.print(finger.fingerID);
    Serial.print("  置信度 = ");
    Serial.println(finger.confidence);
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("✗ 未找到匹配的指纹");
  } else {
    Serial.println("识别出错");
  }
  printMenu();
}

// -------------------- 3. 删除指定ID --------------------
void deleteFingerprint() {
  Serial.println("\n--- 删除指纹 ---");
  Serial.println("请输入要删除的ID：");

  while (!Serial.available());
  uint8_t id = Serial.parseInt();
  while (Serial.available()) Serial.read();

  int p = finger.deleteModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.print("✓ 已删除 ID #");
    Serial.println(id);
  } else {
    Serial.println("✗ 删除失败（可能该ID不存在）");
  }
  printMenu();
}

// -------------------- 4. 清空所有指纹 --------------------
void emptyDatabase() {
  Serial.println("\n--- 清空所有指纹 ---");
  Serial.println("确认清空？输入 y 确认，其他键取消");

  while (!Serial.available());
  char c = Serial.read();
  while (Serial.available()) Serial.read();

  if (c == 'y' || c == 'Y') {
    int p = finger.emptyDatabase();
    if (p == FINGERPRINT_OK) {
      Serial.println("✓ 已清空所有指纹");
    } else {
      Serial.println("✗ 清空失败");
    }
  } else {
    Serial.println("已取消");
  }
  printMenu();
}

// -------------------- 5. 查看传感器信息 --------------------
void showSensorInfo() {
  Serial.println("\n--- 传感器信息 ---");
  finger.getParameters();
  Serial.print("容量: ");          Serial.println(finger.capacity);
  Serial.print("安全等级: ");      Serial.println(finger.security_level);
  Serial.print("设备地址: 0x");    Serial.println(finger.device_addr, HEX);
  Serial.print("数据包长度: ");    Serial.println(finger.packet_len);
  Serial.print("波特率: ");        Serial.println(finger.baud_rate);
  printMenu();
}