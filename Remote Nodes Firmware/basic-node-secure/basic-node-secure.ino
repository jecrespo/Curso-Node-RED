/*
   Ejemplo básico de nodo remoto para interacción con MQTT y Node-RED
   Publica en un topic cada 10 segundos y en otro cada 60 segundos
   Está suscrito a dos topic, uno de ellos enciende y apaga el led integrado de ESP8266 y el otro muestra por pantalla lo recibido
   En cada reset publica un mensaje indicando que se ha reiniciado
*/

/* secrets.h

  #define SSID "......."
  #define PASSWORD "......"
  #define MQTT_SERVER "......"
  #define MQTT_USER "......."
  #define MQTT_PASSWORD "........"

*/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "secrets.h"


#define DISPOSITIVO "enrique" //Dispositivo que identifica al publicar en MQTT
#define RAIZ "cursocefire/wemos"  //raiz de la ruta donde va a publicar

// Fingerprint of the broker CA SHA-1
// Verify the SHA1 fingerprint of the certificate returned matches this one.
// If the server certificate changes, it will fail. If an array of 20 bytes are sent in, it is assumed they are the binary SHA1 values.
// openssl x509 -in  mqttserver.crt -sha1 -noout -fingerprint
// SHA-1 fingerprint enriquecrespo.com valid until viernes, 16 de abril de 2021
const char* fingerprint = "20:9E:19:B1:22:05:FA:1E:77:45:86:22:F9:32:BB:A5:FD:43:15:9E";

WiFiClientSecure espClient;
PubSubClient client(espClient);
long lastMsg = 0;
long lastMsgM = 0;
char msg[50];
int value = 0;
int valueM = 0;

//Topics
String topic_root =  String(RAIZ) + "/" + String(DISPOSITIVO);
String publish_10sec_string = topic_root + "/dato10s";
const char* publish_10sec = publish_10sec_string.c_str();
String publish_60sec_string = topic_root + "/dato60s";
const char* publish_60sec = publish_60sec_string.c_str();
String publish_reset_string = topic_root + "/reset";
const char* publish_reset = publish_reset_string.c_str();
String subs_led_string = topic_root + "/led";
const char* subs_led = subs_led_string.c_str();
String subs_text_string = topic_root + "/text";
const char* subs_text = subs_text_string.c_str();

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  Serial.begin(115200);
  setup_wifi();
  espClient.setFingerprint(fingerprint);
  // Alternative: espClient.setInsecure;
  client.setServer(MQTT_SERVER, 8883);
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
    snprintf (msg, 50, "hello world 10s #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(publish_10sec, msg);
  }

  if (now - lastMsgM > 60000) {
    lastMsgM = now;
    ++valueM;
    snprintf (msg, 50, "hello world 60s #%ld", valueM);
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
    String clientId = "ESP8266-" + String(DISPOSITIVO) + "-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
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
