#include "ldr_secrets.h"

#include <ESP8266WiFi.h>


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
bool hasCarArrived;

void setup() {
  Serial.begin(9600);

}

void loop() {
  ldrInitialReadValue = analogRead(A0);
  delay(1000);
  ldrFinalReadValue = analogRead(A0);

  if(ldrFinalReadValue - ldrInitialReadValue > 100){
    Serial.print("Object is approaching");
    Serial.print("\n");
    }
  delay(100);
}
