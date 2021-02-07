#include "ldr_secrets.h"
#include "FS.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

int ldrInitialReadValue;
int ldrFinalReadValue;
bool hasCarArrived = false;
const char* pubSubSuccessMessage = "{\n\t\"sender\": \"esp8266-publisher\",\n\t\"hasCarArrived\": true\n}";


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

PubSubClient client(AWS_ENDPOINT, MQTT_PORT, callback, wiFiEspClient);

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
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  setup_wifi();
  delay(1000);
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }

  Serial.print("Heap: "); Serial.println(ESP.getFreeHeap());

  // Load certificate file
  File cert = SPIFFS.open("/cert.der", "r");
  if (!cert) {
    Serial.println("Failed to open cert file");
  }
  else
    Serial.println("Success to open cert file");

  delay(1000);

  // Load private key file
  File private_key = SPIFFS.open("/key.der", "r");
  if (!private_key) {
    Serial.println("Failed to open private cert file");
  }
  else
    Serial.println("Success to open private cert file");

  delay(1000);

  // Load CA file
  File ca = SPIFFS.open("/rootCA.der", "r");
  if (!ca) {
    Serial.println("Failed to open ca ");
  }
  else
    Serial.println("Success to open ca");

  delay(1000);



  if (wiFiEspClient.loadCertificate(cert))
    Serial.println("cert loaded");
  else
    Serial.println("cert not loaded");

  if (wiFiEspClient.loadPrivateKey(private_key))
    Serial.println("private key loaded");
  else
    Serial.println("private key not loaded");

  if (wiFiEspClient.loadCACert(ca))
    Serial.println("ca loaded");
  else
    Serial.println("ca failed");

  Serial.print("Heap: "); Serial.println(ESP.getFreeHeap());


}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  ldrInitialReadValue = analogRead(A0);
  delay(1000);
  ldrFinalReadValue = analogRead(A0);
  

  if (ldrFinalReadValue - ldrInitialReadValue > 100) {
    hasCarArrived = true;
    Serial.print("Object is approaching");
    Serial.print("\n");
    Serial.print(hasCarArrived);
    client.publish(AWS_IOT_CORE_TOPIC, pubSubSuccessMessage);
  }
  delay(100);
}
