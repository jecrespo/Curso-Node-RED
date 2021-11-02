/*
   Ejemplo básico de nodo remoto para interacción con MQTT y Node-RED para Wemos D1 Mini + BMP Shield + OLED Shield
   Publica en un topic cada 10 segundos con la temperatura y en otro cada 60 segundos
   Está suscrito a dos topic, uno de ellos enciende y apaga el led integrado de ESP8266 y el otro manda por serial lo recibido
   En cada reset publica un mensaje indicando que se ha reiniciado
   Publica los datos de presión barométrica de la shield BMP180 en Broker
   Está suscrito a un tercer topic, que muestra el mensaje por pantalla oled
   Instalar librerías: Adafruit_GFX.h, Adafruit_SSD1306.h y Adafruit_BMP085.h
*/

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

WiFiClient espClient;
PubSubClient client(espClient);

//variables globales
long lastMsg = 0;
long lastMsgM = 0;
char msg[50];
String m = "";
int value = 0;
int valueM = 0;

const char* publish_temp = "casa/wemos2/temperatura";
const char* publish_pres = "casa/wemos2/presion";
const char* publish_10sec = "casa/wemos2/dato10s";
const char* publish_60sec = "casa/wemos2/dato60s";
const char* publish_reset = "casa/wemos2/reset";
const char* subs_led = "casa/wemos2/led";
const char* subs_text = "casa/wemos2/text";
const char* subs_oled = "casa/wemos2/oled";

//0x31
#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_RESET);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);

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

void loop() {
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

  long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 50, "hello world 10s #%d", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(publish_10sec, msg);

    Serial.print("Temperature = ");
    Serial.print(t);
    Serial.println(" Celsius");

    Serial.print("Pressure = ");
    Serial.print(p);
    Serial.println(" Pascal");

    Serial.print("Publish message Temperature: ");
    snprintf (msg, 50, "%4.2f", t);
    Serial.println(msg);
    client.publish(publish_temp, msg);

    Serial.print("Publish message Pressure: ");
    snprintf (msg, 50, "%i", p);
    Serial.println(msg);
    client.publish(publish_pres, msg);
  }

  if (now - lastMsgM > 60000) {
    lastMsgM = now;
    ++valueM;
    snprintf (msg, 50, "hello world 60s #%d", valueM);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(publish_60sec, msg);
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
      digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
      // but actually the LED is on; this is because
      // it is active low on the ESP-01)
    } else {
      digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
    }
  }

  if (String(topic) == String(subs_oled)) {
    // muestra mensaje en pantalla
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
    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(publish_reset, "reset");
      // ... and resubscribe
      client.subscribe(subs_led);
      client.subscribe(subs_text);
      client.subscribe(subs_oled);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
