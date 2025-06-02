#include <Servo.h>
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

hd44780_I2Cexp lcd;
Servo myServo;

// פינים
const int trigPin = 3;
const int echoPin = 2;
const int buttonPin = 4;
const int buzzerPin = 8;
const int servoPin = 13;

int reps = 0;
int Set = 0;
bool inPosition = false;
bool buttonPressed = false;
bool goalReached = false;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW); // ודא שהבאזר כבוי

  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setBacklight(HIGH);
  myServo.attach(servoPin);
  //myServo.write(0);

  lcd.setCursor(0, 0);
  lcd.print("Ready,Set,Go");
  lcd.setCursor(0, 1);
  lcd.print("Reps: 0");
}

void loop() {
  float distance = readDistance();

  Serial.print("Distance: ");
  Serial.println(distance);

  // רק כשנכנס מתחת ל-15 ס"מ וטרם היה במצב הזה
  if (distance < 15 && !inPosition) {
    reps++;
    inPosition = true;

    // ✅ ביפ רק כשיש חזרה חדשה
    digitalWrite(buzzerPin, LOW);
    delay(250);
    digitalWrite(buzzerPin, HIGH);

    updateLCD(distance);

    if (reps == 10 && !goalReached) {
      activateSuccessSignal();
      goalReached = true;
    }
  }

  // כשמרחק גדול מ-15 → מתכונן לחזרה הבאה
  if (distance >= 15) {
    inPosition = false;
  }

  if (digitalRead(buttonPin) == LOW && !buttonPressed) {
    lcd.clear();
    buttonPressed = true;
    reps = 0;
    goalReached = false;
    myServo.write(0);
    //digitalWrite(buzzerPin, LOW); // ודא שהבאזר כבוי
    updateLCD(distance);
    Serial.println("Reset pressed");
    Set ++ ;
  }

  if (digitalRead(buttonPin) == HIGH) {
    buttonPressed = false;
  }

  delay(200);
}

float readDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2;
  return distance;
}

void updateLCD(float distance) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Reps: ");
  lcd.print(reps);
  lcd.print("     ");

  lcd.setCursor(0, 1);
  lcd.print("Sets: ");
  lcd.print(Set);
}

void activateSuccessSignal() {
  myServo.attach(servoPin);
  Serial.println("Goal Reached! Activating signal...");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Goal reached!");
  lcd.setCursor(0, 1);
  lcd.print("Set done!");

  myServo.write(0);
  myServo.write(90);
  myServo.write(180);
  delay(1000);
  myServo.detach();  
}
