/* secrets.h

  #define SSID "......."
  #define PASSWORD "......"
  #define MQTT_SERVER "......"
  #define MQTT_USER "......."
  #define MQTT_PASSWORD "........"

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BMP085.h>
#include "secrets.h"

//Sensor
Adafruit_BMP085 bmp;

//Wifi
WiFiClient espClient;
PubSubClient client(espClient);

//variables globales
unsigned long lastMsg = 0;
char msg[50];
String m = "";

//topics mqtt
const char* publish_temp = "casa/nodobmp1/temp";
const char* publish_pres = "casa/nodobmp1/pres";
const char* publish_reset = "casa/nodobmp1/reset";
const char* subs_led = "casa/nodobmp1/led";
const char* subs_text = "casa/nodobmp1/text";
const char* lwt_topic = "casa/nodobmp1/status";

//0x31
#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_RESET);

void setup()
{
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);
  //Wire.begin (4, 5);
  if (!bmp.begin())
  {
    Serial.println("Could not find BMP180 or BMP085 sensor at 0x77");
    while (1) {}
  }

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  setup_wifi();
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);

}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float t = bmp.readTemperature();
  int p = bmp.readPressure();

  // Clear the buffer.
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.setTextColor(WHITE);

  display.print("T: ");
  display.println(t);
  display.print("P: ");
  display.println(p);

  display.setTextSize(1);
  display.println("mensaje: ");
  display.println(m);

  display.display();

  unsigned long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;
    
    Serial.print("Temperature = ");
    Serial.print(t);
    Serial.println(" Celsius");

    Serial.print("Pressure = ");
    Serial.print(p);
    Serial.println(" Pascal");
    
    Serial.print("Publish message Temperature: ");
    snprintf (msg, 50, "%4.2f", t);
    Serial.println(msg);
    client.publish(publish_temp, msg, true);

    Serial.print("Publish message Pressure: ");
    snprintf (msg, 50, "%i", p);
    Serial.println(msg);
    client.publish(publish_pres, msg, true);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String mensaje = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    mensaje = mensaje + (char)payload[i];
  }
  Serial.println();

  if (String(topic) == String(subs_led)) {
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '1') {
      digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
      // but actually the LED is on; this is because
      // it is active low on the ESP-01)
    } else {
      digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
    }
  }

  if (String(topic) == String(subs_text)) {
    // show message
    m = mensaje;
  }

}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD, lwt_topic, 2, false, "KO")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(publish_reset, "reset");
      // ... and resubscribe
      client.subscribe(subs_led);
      client.subscribe(subs_text);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
