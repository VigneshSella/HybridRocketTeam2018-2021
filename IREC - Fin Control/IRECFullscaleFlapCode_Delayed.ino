duino#include <Wire.h>
#include <SPI.h>
#include <MPU9250.h>
#include <Adafruit_MPL3115A2.h>
#include <Adafruit_Sensor.h>  // not used in this demo but required!
#include <Servo.h>    //"import" servo "package" (python jargon)
Servo myservo;  //create servo object to control a servo
Servo myservo2;
Servo myservo3;

int orangeLED = 5;
int greenLED = 6;
int blueLED = 7;
int yellowLED = 13;
int buzzer = 8;

pinMode(5, OUTPUT);
pinMode(6, OUTPUT);
pinMode(7, OUTPUT);
pinMode(8, OUTPUT);
pinMode(13, OUTPUT);

int time_to_active_drag = 3000;     //Time that program will run for (in ms) before it activates active drag mode (fully deploy flaps)
int initial_angle = 80;             //angle that flaps should start at when code first runs
int rpm_threshold = 10;             //Minimum magnitude of rpm at which servo will start to respond
int max_turn_angle = 3;             //Maximum angle that the flaps will be allowed to turn
int max_ang_vel = 120;              //Minimum angular velocity at which flaps automatically deploy to max angle (RPM)
int launch_acceleration = 7;        //Minimum acceleration arduino must detect to start flaps  (first check)
bool launch = false;                //Tracks if aruduino has detected launch
bool launch_init = false;           //True whenever acceleration exceeds threshold
int free_fall_thresh = -1;          //Maximum acceleration arduino will allow for flaps system to start (second check)
bool free_fall = false;             //Rocket is free falling (trigger flaps system)
bool free_fall_init = false;        //True whenever acceleration (post-burnout) is below threshold
float active_drag_alt = 100000000000000;        //Altitude at which active drag triggers (really big because we wont use it)
int landing_thresh = 3.5;           //Minimum upward (positive) acceleration MPU must detect in order for it detect landing
bool apogee = false;
float az;                           //accelation in z direction (milli g 's)
double gz;                          //angular velocity in z direction (DEG/s)
double gz_offset;
double dragtimer;
float burn_timer = 0;               //how long the arduino measures the burn to last
float launch_time = 0;              //time at which arduino first detects acceleration above launch threshold
int launch_time_threshold = 500;    //Amount of time (ms) acceleration must exceed threshold to detect launch
float coast_time = 0;               //how long the arduino measures the rocket to be coasting
float burnout_time = 0;             //time at which arduino first detects acceleration (post-burn) below threshold
int freefall_time_threshold = 500;  //Amount of time (ms) acceleration must be below threshold to detect coast

float altm = 0;            //Current altitude (altitude at current velocity caluclation)
float prev_altm = 0;       //Old altitude (altitude at previous velocity calculation)
int prev_time = 0;         //Old time (time at previous velocity calculation)
int curr_time = 0;         //Current time (time at updated velocity)
float velocity = 0;        //Velocity of rocket, valculated by differentiation
float velocity_thresh = -5;     //Maximum velocity at which rocket can be approximated to be at apogee (i.e the max speed we can say the rocket is close enough to apogee)


#define mySerial Serial1

Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2();
MPU9250 mpu;


void setup()  {

  Serial.begin(115200);

  if (! baro.begin()) {
    Serial.println("Couldnt find sensor");
    return;
  }
  Wire.begin();

  myservo.attach(2);  //Initializes myservo object by assigning the physical servo to digital pin 9 (D9) on arduino
  myservo.write(initial_angle);     //Intilialize the flaps to the vertical orientation, which appears to be about 80 degrees atm.
  myservo2.attach(3);  //Initializes myservo o bject by assigning the physical servo to digital pin 9 (D9) on arduino
  myservo2.write(initial_angle);     //Intilialize the flaps to the vertical orientation, which appears to be about 80 degrees atm.
  myservo3.attach(4);  //Initializes myservo object by assigning the physical servo to digital pin 9 (D9) on arduino
  myservo3.write(initial_angle);     //Intilialize the flaps to the vertical orientation, which appears to be about 80 degrees atm.

  digitalWrite(blueLED, HIGH);      //CALIBRATION SEQUENCE
  for (int i = 0; i < 20; i++) {
      digitalWrite(buzzer, HIGH);
      delay(5);
      digitalWrite(buzzer, LOW);
      delay(5);
  }
  for (int i = 0; i < 1000; i++) {
      gz_offset += gz;
  }
  gz_offset /= 1000;
}
for (int i = 90; i <= 180; i++) {
    myservo.write(i);
    myservo2.write(i);
    myservo3.write(i);
    delay(1);
}
for (int i = 180; i >= 0; i--) {
    myservo.write(i);
    myservo2.write(i);
    myservo3.write(i);
    delay(1);
}
delay(500);
digitalWrite(buzzer, HIGH);
delay(500);
digitalWrite(buzzer, LOW);
delay(500);
digitalWrite(buzzer, HIGH);
delay(500);
digitalWrite(buzzer, LOW);

digtalWrite(blueLED, LOW);

unsigned int timer = millis();

void loop() {
  prev_altm = altm;         //Just after the velocity calcualtion, the most recent measurments become "old" (previous), pending more data, which will beocme the "current" altitude and time
  prev_time = millis();
  mpu.update();
  mpu.getRoll();

  //gz = ANGULAR VELCOCITY (DEG/s)
  //az = ACCELERATION (milli g 's)

  float ang_vel = gz*60/360;  //Convert DEG/S to RPM


if(myservo.attached() == false || myservo2.attached() == false || myservo3.attached() == false)
{
  emergency_terminate();
}




//LOGIC TO DETECT BURN AND COAST PHASES (LAUNCH AND BURNOUT):
//LAUNCH/BURN:
if(az/1000 > launch_acceleration && launch_init == true  && free_fall == false)      //If the arduino has detected a high acceleration before the coast/free fall phase...
{
  burn_timer = millis() - launch_time;                //...start measuring the lenght of the burn time
  if(burn_timer > launch_time_threshold)              //If the burn time exceeds some threshold (if high acceleration has been observed for at least some period of time...)
  {
    launch = true;                    //...launch has been detected
    Serial.println(String(millis()/1000) + "s: Launch detected");
  }
}
else if(az/1000 < launch_acceleration && launch_init == true  && free_fall == false)      //If a high acceleration during the "burn" phase is detected, but only temporarily...
{
  launch_init = false;                  //...then the arduino has not actually detected launch (the high acceleration was just an anomaly)
}

if(az/1000 > launch_acceleration && launch_init == false)    //If high acceleration is observed for the first time...
  {
    launch_time = millis();                     //...assume launch has been detected, and start measuring the burn time
    launch_init = true;
  }

//BURNOUT/COAST:
if(az/1000 < free_fall_thresh && free_fall_init == true  && free_fall == false)      //If the arduino has detected a large negative acceleration after burnout...
{
  coast_time = millis() - burnout_time;                //...start measuring the lenght of the coast time
  if(coast_time > freefall_time_threshold)              //If the burn time exceeds some threshold (if high acceleration has been observed for at least some period of time...)
  {
    free_fall = true;                    //...launch has been detected
    Serial.println(String(millis()/1000) + "s: Burnout detected. Deploy roll control.");
  }
}
else if(az/1000 > free_fall_thresh && launch_init == true  && free_fall == false)      //If a large negative acceleration during the "coast" phase is detected, but only temporarily...
{
  free_fall_init = false;                  //...then the arduino has not actually detected burnout/coast (the negative acceleration was just an anomaly)
}

if(az/1000 < free_fall_thresh && free_fall_init == false)    //If large negative acceleration is observed for the first time...
  {
    burnout_time = millis();                     //...assume burnout has been detected, and start measuring the coast time
    free_fall_init = true;
  }






if(launch == true && free_fall == true && apogee == false)                   //Once launch and burnout have been detected, activate Active Drag
{
  if(ang_vel < -rpm_threshold || ang_vel > rpm_threshold && millis() < dragtimer )  //If the rpm exceeds some threshold value...
  {
    //myservo.write(initial_angle + ((g.gyro.z)/3));     ///Turn the servo in the OPPOSITE direction, proportionately to the rpm
    myservo.write(initial_angle + constrain((ang_vel)/max_ang_vel, -max_turn_angle, max_turn_angle));      //Any angular velocity greater than or equal to +/- 120rpm (2 revolutions per second)automatically triggers max angle
    myservo2.write(initial_angle + constrain((ang_vel)/max_ang_vel, -max_turn_angle, max_turn_angle));
    myservo3.write(initial_angle + constrain((ang_vel)/max_ang_vel, -max_turn_angle, max_turn_angle));
  }
  else
  {
    myservo.write(initial_angle); //go to vertical
    myservo2.write(initial_angle);
    myservo3.write(initial_angle);
  }

  if(millis() > dragtimer || altm > active_drag_alt)
  {
    myservo.write(initial_angle + 90);     //flaps go full horizontal (perpendicular to starting angle)
    myservo2.write(initial_angle + 90);
    myservo3.write(initial_angle + 90);
    Serial.println(String(millis()/1000) + "s: Active Drag mode activated");
  }
  //logic to detect apogee

if(launch == true && free_fall == true && apogee == true)       //if the rocket is past burnout and AFTER apogee...
{
  if(az/1000 > landing_thresh)    //...look to detect landing
  {
    Serial.println(String(millis()/1000) + "s: Landing detected");
  }
}

altm = baro.getAltitude();      //at the end of the loop, just before the velocity is updated, "current" time and altitude are measured, and compared to the time and altitude measured in the previous loop. The "prev" and "current" measurements are on opposite ends of the loop to ensure a noticeable difference in the measurments
curr_time = millis();
velocity = (altm - prev_altm)/((curr_time - prev_time)/1000);     //Velocity = dh/dt (REMEMBER: millis() gives time in MILLISECONDS)
if(velocity < velocity_thresh  && launch == true && free_fall == true)
{
  apogee = true;
  Serial.println(String(millis()/1000) + "s: Apogee detected");
}

}
if (apogee == true)
{
  //reached apogee, do nothing
  myservo.write(initial_angle);
  myservo2.write(initial_angle);
  myservo3.write(initial_angle);
}
}


void emergency_terminate()         //If a servo detaches, effectively end the program
{
  delay(200000000000);
}
