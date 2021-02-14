#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DS18B20.h>

DS18B20 ds(D2);
uint8_t dir[8];

float t;

//0x31
#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_RESET);

void setup() {

  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

}

void loop() {

  while (ds.selectNext()) {
    t = ds.getTempC();
    Serial.println(t);
  }

  // Clear the buffer.
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.setTextColor(WHITE);

  display.println("T: ");
  display.setTextSize(2);
  display.println(t);

  display.display();
  delay(1500);

}
