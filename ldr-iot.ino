int sensorValue;

void setup() {
  Serial.begin(9600);

}

void loop() {
  sensorValue = analogRead(A0);
  Serial.print(sensorValue, DEC);
  Serial.print("\n");
  delay(100);
}l
