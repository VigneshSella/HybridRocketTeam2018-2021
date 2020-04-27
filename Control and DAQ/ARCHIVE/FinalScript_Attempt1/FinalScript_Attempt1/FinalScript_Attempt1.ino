#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <max6675.h>
#include <SD.h>
#include <SPI.h>

int pressureSCL = 1;        //Pressure Sensor Shield Pin define
int pressureSDA = 2;

int thermocoupleSO = 3;    //Thermocouple Shield Pin define
int thermocoupleCS = 4;   
int thermocoupleSCK= 5;    

int sdcardCS = 6;           //MicroSD Card reader Pin define
int sdcardSCK = 7;          
int sdcardMOSI = 8;         
int sdcardMISO = 9;         

int xbeePOUT = 10;          //XBee Pin define
int xbeePIN = 11;

int relayCtrl = 12;         //Relay Control Pin define

int servoCtrl = 13;         //Servo Control Pin define

MAX6675 ktc(thermocoupleSCK, thermocoupleCS, thermocoupleSO);     //initialize/define thermocouple
Adafruit_ADS1115 ads(0x48);                                       //initialize/define pressure sensor 

File DataFile;


void setup() {
    Serial.begin(9600);
    delay(500); //give Thermocouple time to settle
    
    ads.begin(); //PressureSensorInitialize
    pinMode(pinCS, OUTPUT);

    if(SD.begin())    //SD Card Initialization
    {
      Serial.println("SD Card is ready to use");
    }
    else
    {
      Serial.println("SD Card initialization failed");
    }

    


}


void loop() {
    //PressureSensorRead
    int16_t adc0;  // we read from the ADC, we have a sixteen bit integer as a result
    adc0 = ads.readADC_SingleEnded(0);
    Voltage = (adc0 * 0.1875)/1000;

    //ThermocoupleSensorRead
    Temperature = ktc.readCelsius();
    



}
