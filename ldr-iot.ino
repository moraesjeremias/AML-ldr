int ldrInitialReadValue;
int ldrFinalReadValue;

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
