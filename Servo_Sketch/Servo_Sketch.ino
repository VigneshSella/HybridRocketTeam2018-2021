#include <Servo.h>

Servo myservo;
int input;
void setup()
{
  myservo.attach(13);
  Serial.begin(9600);
  myservo.write(75);
}

void loop() {
  if (Serial.available() > 0) {
    input = Serial.parseInt();
    myservo.write(input);
    Serial.println(input);
    delay (1000);
  }
}
