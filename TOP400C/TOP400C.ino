#include <Wire.h>
#include <VL53L1X.h>
VL53L1X sensor;

void setup() {
  Serial.begin(115200);
  delay(500);
  Wire.begin(6, 7);   // SDA=6, SCL=7   ← 改成你实际用的
  
  Serial.println("setup...");

  sensor.setTimeout(500);
  if (!sensor.init()) {
    Serial.println("Failed to detect VL53L1X / TOF400C !");
    while (1) {
      Serial.println("while...");
      delay(1000);
    }
  }
  Serial.println("END...");
  //sensor.setSignalThreshold(512);  // 默认大概 1024，调低到 300~800 试试
  sensor.setDistanceMode(VL53L1X::Long);
  sensor.setMeasurementTimingBudget(100000);  // 50ms

  sensor.startContinuous(50);  // 每50ms测量一次
  Serial.println("TOF400C VL53L1X started - 输出距离单位：厘米 (cm)");

}

void loop() {
    //Serial.println("entry loop...");
    sensor.read();
    /*if (sensor.ranging_data.range_status == 0) {  // 0=有效
      float distance_mm = sensor.ranging_data.range_mm;     // 原生毫米值
      float distance_cm = distance_mm / 10.0;               // 转换为厘米

      Serial.print("距离: ");
      Serial.print(distance_cm, 2);   // 保留1位小数，更直观
      Serial.println(" 厘米");
    } else {
      Serial.println("Out of range / error");
   }*/

Serial.print("Status: ");
Serial.print(sensor.ranging_data.range_status);
Serial.print(" | Peak signal: ");
Serial.print(sensor.ranging_data.peak_signal_count_rate_MCPS, 2);
Serial.print(" MCPS | Ambient: ");
Serial.print(sensor.ranging_data.ambient_count_rate_MCPS, 2);
Serial.print(" MCPS | Dist: ");
Serial.print(sensor.ranging_data.range_mm / 10.0, 1);
Serial.println(" cm");

    delay(1200);
}