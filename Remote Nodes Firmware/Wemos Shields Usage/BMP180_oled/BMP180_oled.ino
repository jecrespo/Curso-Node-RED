#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

//0x31
#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_RESET);

void setup()
{
  Serial.begin(115200);
  //Wire.begin (4, 5);
  if (!bmp.begin())
  {
    Serial.println("Could not find BMP180 or BMP085 sensor at 0x77");
    while (1) {}
  }

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

}

void loop()
{
  float t = bmp.readTemperature();
  float p = bmp.readPressure();

  Serial.print("Temperature = ");
  Serial.print(t);
  Serial.println(" Celsius");

  Serial.print("Pressure = ");
  Serial.print(p);
  Serial.println(" Pascal");

  // Clear the buffer.
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.setTextColor(WHITE);

  display.println("T: ");
  display.setTextSize(2);
  display.println(t);

  display.setTextSize(1);
  display.println("P: ");
  display.setTextSize(2);
  display.println(p);

  display.display();
  delay(5000);
}
