/*
    Display message on OLED

    Install Libraries:
     - Adafruit_GFX_Library
     - Adafruit_SSD1306_Wemos_Mini_OLED: https://github.com/stblassitude/Adafruit_SSD1306_Wemos_OLED
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//0x31
#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_RESET);

void setup()
{
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}

void loop()
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.setTextColor(WHITE);
    display.println("Hola Mundo");
    display.setCursor(0, 30);
    display.println(millis()/1000);
    display.display();
    delay(1000);
}
