#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>             // SPI library

// Define pins for ESP32-S3
#define TFT_SCK    13  // Custom SCK pin (instead of GPIO18)
#define TFT_MOSI   12  // Custom MOSI pin (instead of GPIO23)
#define TFT_RST    4   // Reset pin
#define TFT_DC     5   // Data/Command pin
#define TFT_CS     15  // Chip select


// Initialize ST7735 with custom pins (software SPI)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);

void setup(void) {
  Serial.begin(115200);  // For debugging
  Serial.println("ST7735 Test!");

  // Initialize the display
  tft.initR(INITR_BLACKTAB);  // For common ST7735S modules

  tft.setRotation(1);  // Rotate 90 degrees clockwise (160x128)
  // Fill screen with black
  tft.fillScreen(ST7735_BLACK);

  // Set text color and size
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);

  // Set cursor position
  tft.setCursor(10, 60);

  // Print "Hello World!"
  tft.println("Hello World1!");
  tft.println("Hello World2!");
  tft.println("Hello World3!");
  tft.println("Hello World4!");
  tft.println("Hello World5!");
  tft.println("Hello World6!");
      

  Serial.println("Done!");
}

void loop() {
  Serial.println("hello loop!");
  delay(1000);
}