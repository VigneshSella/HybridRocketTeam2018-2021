/*
  AnalogReadSerial
  Reads an analog input on pin 0, prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
*/

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  //Here, you could change the reference voltage to achieve a more accurate read. Google command. analogReference() I think.
  //Reference Voltage Options: 1V, 2.56 V, Default 5V
  
}

// the loop routine runs over and over again forever:
void loop() {
  // read the inputs on analog pins 0 & 1:
  int sensorValue1 = analogRead(A0);
  int sensorValue2 = analogRead(A1); //Should be grounded. Just for reference.
  int V_difference= sensorValue1-sensorValue2;
  // print out the value you read:
  Serial.println(V_difference*5/1024.000);
  delay(100);        // delay in between reads for stability
  //Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
}
