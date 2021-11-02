/*
   Ejemplo básico de nodo remoto para interacción con MQTT y Node-RED para Wemos D1 Mini + DHT11 Shield + RELAY Shield
   Publica en un topic cada 10 segundos con la temperatura y en otro cada 60 segundos
   Está suscrito a dos topic, uno de ellos enciende y apaga el led integrado de ESP8266 y el otro manda por serial lo recibido
   En cada reset publica un mensaje indicando que se ha reiniciado
   Publica los datos de Temperatura y Humedad de la shield DHT en Broker
   Está suscrito a un tercer topic, que enciende y apaga el relé
   Instalar librería https://github.com/RobTillaart/DHTstable
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
#include <DHTStable.h>
#include "secrets.h"

DHTStable DHT;

#define DHT12_PIN D4

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
long lastMsgM = 0;
char msg[50];
int value = 0;
int valueM = 0;

const int relayPin = D1;

const char* publish_temp = "casa/wemos1/temperatura";
const char* publish_hum = "casa/wemos1/humedad";
const char* publish_10sec = "casa/wemos1/dato10s";
const char* publish_60sec = "casa/wemos1/dato60s";
const char* publish_reset = "casa/wemos1/reset";
const char* subs_led = "casa/wemos1/led"; //No funciona porque el pin led es el D4 usado por el shield DHT
const char* subs_text = "casa/wemos1/text";
const char* subs_rele = "casa/wemos1/rele";

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(relayPin, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 50, "hello world 10s #%d", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(publish_10sec, msg);
    int chk = DHT.read12(DHT12_PIN);
    if (chk == DHTLIB_OK) {
      Serial.print("Temperature in Celsius : ");
      Serial.println(DHT.getTemperature());
      Serial.print("Relative Humidity : ");
      Serial.println(DHT.getHumidity());
      Serial.println();
      Serial.println("Publish T&H messages...");
      int valueT = DHT.getTemperature();
      snprintf (msg, 50, "%d", valueT);
      client.publish(publish_temp, msg);
      int valueH = DHT.getHumidity();
      snprintf (msg, 50, "%d", valueH);
      client.publish(publish_hum, msg);
    }
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
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
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

  if (String(topic) == String(subs_rele)) {
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '1') {
      digitalWrite(relayPin, HIGH);   // Turn the Relay on
    } else {
      digitalWrite(relayPin, LOW);  // Turn the Relay off
    }
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
      client.subscribe(subs_rele);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
