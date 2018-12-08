#include "LedControl.h" //  need the library
LedControl lc = LedControl(12, 11, 10, 1); //DIN, CLK, LOAD, No. DRIVER
 
// pin 12 is connected to the MAX7219 pin 1
// pin 11 is connected to the CLK pin 13
// pin 10 is connected to LOAD pin 12
// 1 as we are only using 1 MAX7219

#define trigPin 5
#define echoPin 6
unsigned long startTimeGame;
unsigned long startTimeSensor;
int distance, duration;
int carPosition, carPositionPre = 0; //carPositionPre is the previous car position
 
void setup()
{
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, 2); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen
  pinMode (trigPin, OUTPUT);
  pinMode (echoPin, INPUT);
  Serial.begin(9600);
}

void displayCar(int col, int colPre)
{
  lc.setLed(0, colPre, 7, false);
  lc.setLed(0, colPre + 1, 7, false);
  lc.setLed(0, colPre - 1, 7, false);
  lc.setLed(0, col, 7, true);
  lc.setLed(0, col + 1, 7, true);
  lc.setLed(0, col - 1, 7, true);
}

void loop()
{
  startTimeSensor = millis();
  while (millis() - startTimeSensor < 2)
    digitalWrite(trigPin, LOW); //clear trig
  startTimeSensor = millis();
  while (millis() - startTimeSensor < 10)
    digitalWrite(trigPin, HIGH);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance= duration*0.034/2; //calcularea distantei în cm
  carPosition = distance / 3;
  if(carPosition != carPositionPre)
  {
    carPositionPre = carPosition;
    if(carPosition < 3)
      displayCar(1, carPositionPre);
    else if(carPosition > 24)
      displayCar(8, carPositionPre);
    else displayCar(carPosition, carPositionPre);
  }
}
