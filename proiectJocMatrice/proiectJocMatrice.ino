#include "LedControl.h" //  need the library
#include <LiquidCrystal.h>
LiquidCrystal lcd(3,4,7,8,9,13); // 3 4 7 8 9 13
LedControl lc = LedControl(12, 11, 10, 1); //DIN, CLK, LOAD, No. DRIVER
 
// pin 12 is connected to the MAX7219 pin 1
// pin 11 is connected to the CLK pin 13
// pin 10 is connected to LOAD pin 12
// 1 as we are only using 1 MAX7219

#define trigPin 5
#define echoPin 6
#define buttonPin 2

unsigned long startTimeGame = micros(), timeGame = 0, score = 0;
float delayTime = 1200;
int carPositionPre, carPosition = 0, distMicro, mapPosition = 0;
float distanceObject;
short gameStatus = 0, numberOfLives = 3;
char matrix[45][8] //game's map
{
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {1, 1, 1, 0, 0, 0, 0, 1},
  {1, 1, 1, 0, 0, 0, 0, 1},
  {1, 1, 1, 0, 0, 0, 1, 1},
  {1, 1, 1, 0, 0, 0, 1, 1},
  {1, 0, 0, 0, 0, 0, 1, 1},
  {1, 0, 0, 0, 0, 1, 1, 1},
  {1, 0, 0, 0, 0, 1, 1, 1},
  {1, 0, 0, 0, 0, 1, 1, 1},
  {1, 0, 0, 0, 1, 1, 1, 1},
  {1, 0, 0, 0, 1, 1, 1, 1},
  {1, 0, 0, 0, 1, 1, 1, 1},
  {1, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 1},
  {1, 1, 0, 0, 0, 0, 1, 1},
  {1, 1, 0, 0, 0, 0, 1, 1},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 1, 1, 0, 0, 0},
  {0, 0, 0, 1, 1, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {1, 0, 0, 0, 0, 0, 0, 1},
  {1, 1, 0, 0, 0, 0, 1, 1},
  {1, 1, 0, 0, 0, 0, 1, 1},
  {1, 1, 0, 0, 0, 0, 1, 1},
  {1, 1, 0, 0, 0, 0, 1, 1},
  {1, 1, 1, 0, 0, 0, 1, 1},
  {1, 1, 1, 0, 0, 0, 0, 1},
  {1, 1, 1, 1, 0, 0, 0, 1},
  {1, 1, 1, 1, 0, 0, 0, 1},
  {1, 1, 1, 1, 0, 0, 0, 1},
  {1, 1, 0, 0, 0, 0, 0, 1},
  {1, 1, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 1, 1, 0, 0, 1},
  {1, 0, 0, 1, 1, 0, 0, 1},
  {0, 0, 0, 1, 1, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {1, 1, 1, 0, 0, 0, 1, 1},
  {1, 1, 1, 0, 0, 0, 1, 1},
  {1, 0, 0, 0, 0, 1, 1, 1},
  {1, 0, 0, 0, 0, 1, 1, 1}
};
 
void setup()
{
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, 2); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen
  pinMode (trigPin, OUTPUT);
  pinMode (echoPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  //Serial.begin(9600);
  lcd.begin(16,2);
  lcd.clear();

}

void readSensor()
{
  digitalWrite(trigPin, LOW); //clear trig
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  distMicro = pulseIn(echoPin, HIGH); // time in microseconds
  distanceObject = 0.034 * distMicro / 2;
  //Serial.println(distanceObject);
  //Serial.println("masina = ");
  //Serial.println(carPosition);
    //Serial.println(micros());
}

void calibrateSensor()
{
    if(distanceObject > 3 && distanceObject < 27)
      if(abs(carPosition - distanceObject) > 1)
      {
        carPositionPre = carPosition;
        carPosition = distanceObject; 
      }
}

void displayCar()
{
  if(carPositionPre != carPosition)
  {
    lc.setLed(0, 7, carPositionPre / 3 - 1, false);
    lc.setLed(0, 7, carPosition / 3 - 1, true);  
  }
  else lc.setLed(0, 7, carPosition / 3 - 1, true);
}

void displayMap()
{
   for (int i = 7; i >= 0; i--)
    for (int j = 7; j >= 0; j--)
      lc.setLed(0, i, j, matrix[(7 - i + mapPosition)%45][j]);  
}

void verifyColision()
{
  for(int j = 0; j <= 7; j++)
  {
    if(matrix[(mapPosition) % 45][j] == 1 && j == carPosition / 3 - 1)
      endOfLife();
   // else Serial.println ("noooo");
  }
  
}

void endOfLife()
{
    for(int i = 0; i <= 7; i++)
    for(int j = 0; j <= 7; j++)
    {
      if(i == j)
        lc.setLed(0, i, j, true);
      else if(7 - i == j)
        lc.setLed(0, i, j, true);
      else lc.setLed(0, i, j, false);
    }  
    numberOfLives--;
    Serial.println(numberOfLives);
    if(numberOfLives <= 0)
      gameStatus = 0;
    else gameStatus = 2;
}

void changeLevel()
{
  delayTime = delayTime - 10;
}

void displayScoreLife()
{
    clearScreen();
    lcd.setCursor(0, 0);
    lcd.print("Lives = ");
    lcd.print(numberOfLives);
    lcd.setCursor(0, 1);
    lcd.print("Score = ");
    lcd.print (score / 1000); 
}

void clearScreen()
{
  lcd.setCursor (0, 0);
  for (int i = 0; i < 16; ++i)
  {
    lcd.write(' ');
  }
  lcd.setCursor (0, 1);
  for (int i = 0; i < 16; ++i)
  {
    lcd.write(' ');
  }
}

void displayStartGame()
{
    clearScreen();
    lcd.setCursor(0, 0);
    lcd.print("Press Start");
    lcd.setCursor(0, 1);
    lcd.print("Score = ");
    lcd.print (score / 1000);
}

void scoreCalculus()
{
  score = score + (millis() - timeGame);
}

void loop()
{
  if(gameStatus == 0)// this is game over or start of the game
  {
    displayStartGame();
    readSensor();
    calibrateSensor();
    displayMap();
    displayCar();
    mapPosition = 0;
    numberOfLives = 3;
    delayTime = 1200;
    if(digitalRead(buttonPin) == 0)
    {
      gameStatus = 1;
      score = 0;
      timeGame = millis();
    }
      
  }
  if(gameStatus == 1) // this means game is running
  {
    timeGame = millis();
    displayScoreLife();
    readSensor();
    calibrateSensor();
    displayCar();
    if(millis() - startTimeGame > delayTime)
    {
      displayMap();
      mapPosition++;
      startTimeGame = millis();
  //    Serial.println(mapPosition);
    }
    verifyColision();
    if(mapPosition % 20 == 0)
      changeLevel();
    scoreCalculus();
  }
  
  if(gameStatus == 2) //this means game is paused because of death, freeze and invincibility for 5s.
  {
    displayScoreLife();
    readSensor();
    calibrateSensor();
    displayCar();
    displayMap();
    if(digitalRead(buttonPin) == 0)
      gameStatus = 1;
  }
  
}
