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

#define DISPOSITIVO "enrique/nododhtrele1" //Dispositivo que identifica al publicar en MQTT
#define RAIZ "cursomqtt"  //raiz de la ruta donde va a publicar

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
long lastMsgM = 0;
char msg[50];
int value = 0;
int valueM = 0;

const int relayPin = D1;

//Topics
String topic_root =  String(RAIZ) + "/" + String(DISPOSITIVO);
String publish_10sec_string = topic_root + "/dato10s";
const char* publish_10sec = publish_10sec_string.c_str();
String publish_60sec_string = topic_root + "/dato60s";
const char* publish_60sec = publish_60sec_string.c_str();
String publish_reset_string = topic_root + "/reset";
const char* publish_reset = publish_reset_string.c_str();
String subs_led_string = topic_root + "/led";
const char* subs_led = subs_led_string.c_str();	//No funciona porque el pin led es el D4 usado por el shield DHT
String publish_temp_string = topic_root + "/temp";
const char* publish_temp = publish_temp_string.c_str();
String publish_hum_string = topic_root + "/hum";
const char* publish_hum = publish_hum_string.c_str();
String subs_text_string = topic_root + "/text";
const char* subs_text = subs_text_string.c_str();
String subs_rele_string = topic_root + "/rele";
const char* subs_rele = subs_rele_string.c_str();
String lwt_topic_string = topic_root + "/status";
const char* lwt_topic = lwt_topic_string.c_str();

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
    client.publish(publish_10sec, msg, true);
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
      client.publish(publish_temp, msg, true);
      int valueH = DHT.getHumidity();
      snprintf (msg, 50, "%d", valueH);
      client.publish(publish_hum, msg, true);
    }
  }

  if (now - lastMsgM > 60000) {
    lastMsgM = now;
    ++valueM;
    snprintf (msg, 50, "hello world 60s #%d", valueM);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(publish_60sec, msg, true);
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
    //clientId += String(random(0xffff), HEX);
    clientId += DISPOSITIVO;
    // Attempt to connect
    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD, lwt_topic, 2, true, "KO")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(publish_reset, "reset");
      client.publish(lwt_topic, "OK", true);
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
