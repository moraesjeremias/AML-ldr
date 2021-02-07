#include "ldr_secrets.h"
#include "FS.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


char wifi_ssid[]       = WIFI_USER;
char wifi_password[]   = WIFI_PSWD;
char aws_endpoint[]    = AWS_ENDPOINT;
char aws_key[]         = AWS_ACCESS_KEY;
char aws_secret[]      = AWS_SECRET_KEY;
char aws_region[]      = AWS_REGION;
const char* aws_topic  = AWS_THING_SHADOW;
int port = MQTT_PORT;

int ldrInitialReadValue;
int ldrFinalReadValue;
bool hasCarArrived = false;


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
WiFiClientSecure wiFiEspClient;


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

}


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  wiFiEspClient.setBufferSizes(512, 512);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_USER);

  WiFi.begin(WIFI_USER, WIFI_PSWD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }

  wiFiEspClient.setX509Time(timeClient.getEpochTime());

}

PubSubClient client(aws_endpoint, MQTT_PORT, callback, wiFiEspClient);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESPthing")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      char buf[256];
      wiFiEspClient.getLastSSLError(buf, 256);
      Serial.print("WiFiClientSecure SSL error: ");
      Serial.println(buf);

      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);

}

void loop() {
  ldrInitialReadValue = analogRead(A0);
  delay(1000);
  ldrFinalReadValue = analogRead(A0);

  if (ldrFinalReadValue - ldrInitialReadValue > 100) {
    hasCarArrived = true;
    Serial.print("Object is approaching");
    Serial.print("\n");
  }
  delay(100);
}
