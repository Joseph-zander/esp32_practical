#include <esp_system.h> // Correct header for ESP32 chip info
void setup() {
  Serial.begin(115200);
  delay(1000); // Allow serial to initialize
}
void showChipInfo() {
  Serial.println("=== ESP32 Chip Info ===");
  Serial.print("Chip Model: ");
  Serial.println(ESP.getChipModel()); // Chip model (e.g., ESP32-D0WD-V3)
  Serial.print("Chip Revision: ");
  Serial.println(ESP.getChipRevision()); // Chip revision number
  Serial.print("Chip ID (MAC): 0x");
  Serial.println(ESP.getEfuseMac(), HEX); // Chip MAC address
  Serial.print("Flash Size: ");
  Serial.print(ESP.getFlashChipSize() / 1024); // Flash size in KB
  Serial.println(" KB");
  Serial.print("Free Heap: ");
  Serial.print(ESP.getFreeHeap()); // Free heap memory in bytes
  Serial.println(" bytes");
  Serial.print("CPU Frequency: ");
  Serial.print(ESP.getCpuFreqMHz()); // CPU frequency in MHz
  Serial.println(" MHz");
  Serial.println("=========================================================");
  Serial.println();
}
void loop() {
  showChipInfo();
  delay(20000);
}