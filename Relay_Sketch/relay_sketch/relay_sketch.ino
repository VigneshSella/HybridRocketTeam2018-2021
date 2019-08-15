void setup() {
  Serial.begin(9600);
  pinMode(8, OUTPUT);

}

void loop() {
  digitalWrite(8, HIGH);
  delay(2000);
  digitalWrite(8, LOW);
  delay(2000);
  

}
