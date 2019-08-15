
#include <max6675.h">
#include <SD.h>
#include <SPI.h>

int ktcSO = 8;          
int ktcCS = 9;
int ktcSCK = 10;

int sdCS = 53;


MAX6675 ktc(ktcSCK, ktcCS, ktcSO);

  
void setup() {
  Serial.begin(9600);
  
  delay(500);
}

void loop() {
  // basic readout test
  
   Serial.print("Deg C = "); 
   Serial.print(ktc.readCelsius());
   Serial.print("\t Deg F = ");
   Serial.println(ktc.readFahrenheit());
 
   delay(500);
}
