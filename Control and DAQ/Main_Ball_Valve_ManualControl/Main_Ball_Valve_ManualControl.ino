#include <Servo.h>

Servo myservo;
Servo myservo2;
int input;
void setup()
{
  myservo.attach(5);
  myservo2.attach(7);
  Serial.begin(9600);
  myservo.write(180);
  myservo2.write(180);
}

void loop() {
  if (Serial.available() > 0) {
    input = Serial.parseInt();
    myservo.write(input);
    myservo2.write(input);
    Serial.println(input);
    delay (1000);
  }
}
