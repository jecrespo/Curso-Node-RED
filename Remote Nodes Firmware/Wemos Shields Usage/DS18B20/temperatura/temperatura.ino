/*
    Install Library: https://github.com/matmunk/DS18B20
*/

#include <DS18B20.h>

DS18B20 ds(D2);
uint8_t dir[8];

void setup() {
  Serial.begin(115200);
}

void loop() {
  while (ds.selectNext()) {
    Serial.println(ds.getTempC());
  }
  delay(500);
}
