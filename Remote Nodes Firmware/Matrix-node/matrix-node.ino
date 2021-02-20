/*
   Ejemplo de nodo remoto para interacción con MQTT y Node-RED con Wemos D1 Mini +  Shield Matrix led: https://www.wemos.cc/en/latest/d1_mini_shield/matrix_led.html
   Publica en un topic cada 10 segundos y en otro cada 60 segundos
   Está suscrito a dos topic, uno de ellos enciende y apaga el led integrado de ESP8266 y el otro enciende y apaga el relé
   En cada reset publica un mensaje indicando que se ha reiniciado
   En el topic matrix hace:
   - 0 --> efecto
   - 1 --> 1 linea
   - 2 --> 2 lineas
   - 3 --> 3 lineas
   ...
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
#include <WEMOS_Matrix_LED.h>
#include "secrets.h"

#define DISPOSITIVO "enrique" //Dispositivo que identifica al publicar en MQTT
#define RAIZ "cursocefire/wemos"  //raiz de la ruta donde va a publicar

MLED mled(5); //set intensity=5

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
const char* subs_led = subs_led_string.c_str();
String subs_matrix_string = topic_root + "/matrix";
const char* subs_matrix = subs_matrix_string.c_str();

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

  if (String(topic) == String(subs_matrix)) {
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '0') {
      clear_display ();
      Serial.println ("efecto");
      for (int y = 0; y < 8; y++)
      {
        for (int x = 0; x < 8; x++)
        {
          mled.dot(x, y); // draw dot
          mled.display();
          delay(200);
          mled.dot(x, y, 0); //clear dot
          mled.display();
          delay(200);
        }
      }
    }
    if ((char)payload[0] == '1') {
      clear_display ();
      Serial.println ("Una linea");
      for (int y = 0; y < 1; y++)
      {
        for (int x = 0; x < 8; x++)
        {
          mled.dot(x, y); // draw dot
          mled.display();
        }
      }
    }
    if ((char)payload[0] == '2') {
      clear_display ();
      Serial.println ("Dos lineas");
      for (int y = 0; y < 2; y++)
      {
        for (int x = 0; x < 8; x++)
        {
          mled.dot(x, y); // draw dot
          mled.display();
        }
      }
    }
    if ((char)payload[0] == '3') {
      clear_display ();
      Serial.println ("Tres lineaa");
      for (int y = 0; y < 3; y++)
      {
        for (int x = 0; x < 8; x++)
        {
          mled.dot(x, y); // draw dot
          mled.display();
        }
      }
    }
    if ((char)payload[0] == '4') {
      clear_display ();
      Serial.println ("Cuatro lineas");
      for (int y = 0; y < 4; y++)
      {
        for (int x = 0; x < 8; x++)
        {
          mled.dot(x, y); // draw dot
          mled.display();
        }
      }
    }
    if ((char)payload[0] == '5') {
      clear_display ();
      Serial.println ("Cinco lineas");
      for (int y = 0; y < 5; y++)
      {
        for (int x = 0; x < 8; x++)
        {
          mled.dot(x, y); // draw dot
          mled.display();
        }
      }
    }
    if ((char)payload[0] == '6') {
      clear_display ();
      Serial.println ("Seis lineas");
      for (int y = 0; y < 6; y++)
      {
        for (int x = 0; x < 8; x++)
        {
          mled.dot(x, y); // draw dot
          mled.display();
        }
      }
    }
    if ((char)payload[0] == '7') {
      clear_display ();
      Serial.println ("Siete lineas");
      for (int y = 0; y < 7; y++)
      {
        for (int x = 0; x < 8; x++)
        {
          mled.dot(x, y); // draw dot
          mled.display();
        }
      }
    }
    if ((char)payload[0] == '8') {
      clear_display ();
      Serial.println ("Ocho lineas");
      for (int y = 0; y < 8; y++)
      {
        for (int x = 0; x < 8; x++)
        {
          mled.dot(x, y); // draw dot
          mled.display();
        }
      }
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
      client.subscribe(subs_matrix);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void clear_display () {
  for (int y = 0; y < 8; y++)
  {
    for (int x = 0; x < 8; x++)
    {
      mled.dot(x, y, 0); //clear dot
      mled.display();
    }
  }
}
