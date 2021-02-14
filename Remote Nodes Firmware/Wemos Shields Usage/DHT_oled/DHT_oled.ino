#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "DHT.h"
#define DHTPIN D4
#define DHTTYPE DHT11

//0x31
#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_RESET);

DHT dht(DHTPIN, DHTTYPE);

void setup() {

  Serial.begin(115200);

}

void loop() {
  DHT dht(DHTPIN, DHTTYPE);
  delay(500);
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.println("Error!");
    display.display();
    delay(1000);
    return;
  }

  // Clear the buffer.
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.setTextColor(WHITE);

  display.println("T: ");
  display.setTextSize(2);
  display.println(t);

  display.setTextSize(1);
  display.println("H: ");
  display.setTextSize(2);
  display.println(h);

  display.display();
  delay(1500);

}
