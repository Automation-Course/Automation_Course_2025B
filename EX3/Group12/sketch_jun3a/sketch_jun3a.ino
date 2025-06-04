#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#define servoPin 11

// LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

// servo
Servo myServo;

// pins
const int trigPin = 9;
const int echoPin = 10;
const int pushButton = 7;
const int piezoPin = 8;


// variables
int counter = 0;
int sets = 0;
bool objectPreviouslyDetected = true;
int angle = 0;
int speed = 0;
bool direction = false; 

void setup() 
{
  counter = 0;
  sets = 0;
  objectPreviouslyDetected = true;
  angle = 0;
  speed = 0;
  direction = false;

  pinMode(pushButton, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(piezoPin, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);    
  lcd.print("Count: ");
  lcd.setCursor(7, 0);
  lcd.print(counter);
  lcd.setCursor(0, 1);
  lcd.print("Sets: ");
  lcd.setCursor(7, 1);
  lcd.print(sets);

  myServo.attach(servoPin);

  Serial.begin(9600);
}

void loop() 
{
  int buttonState = digitalRead(pushButton);

  float distance = readUltrasonicDistance(trigPin, echoPin);

  // reset button
  if (buttonState == HIGH) 
  {
    counter = 0;
    sets = 0;
    direction = false;

    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 0);
    lcd.print("Count: ");
    lcd.setCursor(7, 0);
    lcd.print(counter);

    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("Sets: ");
    lcd.setCursor(7, 1);
    lcd.print(sets);
  }

  // pushup counting based on distance
  if (distance > 0 && distance <= 10) 
  {
    if (!objectPreviouslyDetected) 
    {
      counter++;
      digitalWrite(piezoPin, HIGH);
      delay(300);
      digitalWrite(piezoPin, LOW);
      
      if(counter <= 5)
      {
        lcd.setCursor(0, 0);
        lcd.print("Count: ");
        lcd.setCursor(7, 0);
        lcd.print(counter);
      }

      objectPreviouslyDetected = true;

      if (counter == 5)
      {
        direction = true;
      }

      if (counter > 5) 
      {
        sets ++;
        counter = 1;
        
        
        lcd.setCursor(0, 0);
        lcd.print("Count: ");
        lcd.setCursor(7, 0);
        lcd.print(counter);

        lcd.setCursor(0, 1);
        lcd.print("Sets: ");
        lcd.setCursor(7, 1);
        lcd.print(sets);
      }
    }
  } 
  else 
  {
    objectPreviouslyDetected = false;
  }
  
  // set servo location
  if (direction)
  { 
    if (angle < 40)
    {
     angle +=1;
     speed = 0;
    }
    else
    {
      direction = false;
    }
  }
  else
  {
    if (angle > 0)
    {
     angle -=1;
     speed = 180; 
    }
    else
    {
      speed = 90;
    }
  }
  myServo.write(speed);
  delay(15);
}

// distance reading function
float readUltrasonicDistance(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duration = pulseIn(echo, HIGH);
  float distanceCM = duration * 0.034 / 2;
  return distanceCM;
}