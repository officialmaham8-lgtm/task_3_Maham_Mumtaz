#include <WiFi.h>
#include <PubSubClient.h>

// -------------------- WiFi Credentials --------------------
const char* ssid = "WOKWI-GUEST";
const char* password = "";

// -------------------- Adafruit IO --------------------
#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883

#define AIO_USERNAME "maham_mumtaz"
#define AIO_KEY "aio_alQI96OZ7z6KNP3qo63BXL1x9x0s"

// -------------------- HC-SR04 Pins --------------------
#define TRIG_PIN 5
#define ECHO_PIN 18

WiFiClient wifiClient;
PubSubClient client(wifiClient);

const char* feed = AIO_USERNAME "/feeds/distance";

// -------------------- WiFi --------------------
void connectWiFi() {
  Serial.print("Connecting to WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");
}

// -------------------- MQTT --------------------
void connectMQTT() {

  while (!client.connected()) {

    Serial.print("Connecting to Adafruit IO...");

    if (client.connect("ESP32Client", AIO_USERNAME, AIO_KEY)) {

      Serial.println(" Connected");

    } else {

      Serial.print(" Failed. Error = ");
      Serial.print(client.state());
      Serial.println(" Retrying...");
      delay(2000);

    }
  }
}

// -------------------- Distance --------------------
float getDistance() {

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);

  float distance = duration * 0.0343 / 2;

  return distance;
}

// -------------------- Setup --------------------
void setup() {

  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  connectWiFi();

  client.setServer(AIO_SERVER, AIO_SERVERPORT);
}

// -------------------- Loop --------------------
void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!client.connected()) {
    connectMQTT();
  }

  client.loop();

  float distance = getDistance();

  char message[10];
  dtostrf(distance, 4, 2, message);

  client.publish(feed, message);

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(1000);
}