#include <Wire.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1115 ads(0x48);
float Voltage = 0.0;

void setup(void) 
{
  Serial.begin(9600);  
  ads.begin();
}

void loop(void) 
{
  int16_t adc0;  // we read from the ADC, we have a sixteen bit integer as a result
  int16_t adc1;  

  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  
  Serial.print("AIN0: "); 
  Serial.print(adc0);
  Serial.print("\tAIN1: ");
  Serial.print(adc1);
  Serial.print('\n');
 
  
  delay(120);
}
