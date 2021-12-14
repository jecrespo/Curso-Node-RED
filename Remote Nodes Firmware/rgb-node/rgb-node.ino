/*
   Ejemplo básico de nodo remoto para interacción con MQTT y Node-RED. Para Wemos D1 Mini + RGB Shield: https://wiki.wemos.cc/products:d1_mini_shields:rgb_led_shield
   Publica en un topic cada 10 segundos y en otro cada 60 segundos
   Está suscrito a 8 topic, uno de ellos enciende y apaga el led integrado de ESP8266 y los otros 7 encienden los leds del shield
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
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>

#define PIN   D4
#define LED_NUM 7

#define DISPOSITIVO "nodo00" //Dispositivo que identifica al publicar en MQTT
#define RAIZ "nrdeveloper"  //raiz de la ruta donde va a publicar

Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_NUM, PIN, NEO_GRB + NEO_KHZ800);

WiFiClient espClient;
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
String subs_rgb1_string = topic_root + "/rgb1";
const char* subs_rgb1 = subs_rgb1_string.c_str();
String subs_rgb2_string = topic_root + "/rgb2";
const char* subs_rgb2 = subs_rgb2_string.c_str();
String subs_rgb3_string = topic_root + "/rgb3";
const char* subs_rgb3 = subs_rgb3_string.c_str();
String subs_rgb4_string = topic_root + "/rgb4";
const char* subs_rgb4 = subs_rgb4_string.c_str();
String subs_rgb5_string = topic_root + "/rgb5";
const char* subs_rgb5 = subs_rgb5_string.c_str();
String subs_rgb6_string = topic_root + "/rgb6";
const char* subs_rgb6 = subs_rgb6_string.c_str();
String subs_rgb7_string = topic_root + "/rgb7";
const char* subs_rgb7 = subs_rgb7_string.c_str();

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);

  leds.begin();
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

  if (String(topic).startsWith(String(subs_led).substring(-1))) { //Si comienza por el el topic de los leds
    int led = (String(topic).substring(String(subs_led).length() - 1)).toInt();
    //payload debe tener el formato color is a 32-bit type that merges the red, green and blue values into a single number
    leds.setPixelColor(led, int(payload));
    leds.show();
  }

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
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

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
    if (client.connect(clientId.c_str()), MQTT_USER, MQTT_PASSWORD) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(publish_reset, "reset");
      // ... and resubscribe
      client.subscribe(subs_led);
      client.subscribe(subs_rgb1);
      client.subscribe(subs_rgb2);
      client.subscribe(subs_rgb3);
      client.subscribe(subs_rgb4);
      client.subscribe(subs_rgb5);
      client.subscribe(subs_rgb6);
      client.subscribe(subs_rgb7);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
