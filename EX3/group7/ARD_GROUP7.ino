
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Pin definitions
const int buttonPin = 2;
const int ledPin = 3;
const int buzzerPin = 4;
const int servoPin = 9;
const int ldrPin = A0;

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myServo;

unsigned long reactionStartTime = 0;
unsigned long reactionEndTime = 0;
bool waitingForReaction = false;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  lcd.init();
  lcd.backlight();

  myServo.attach(servoPin);
  myServo.write(0);

  lcd.setCursor(0, 0);
  lcd.print("Ready to react?");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Get Ready...");
  delay(1000);

  // בדיקת חיישן אור לפני הירייה
  int lightValue = analogRead(ldrPin);

  if (lightValue < 300) {  // חושך – החיישן מכוסה
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("No light -");
    lcd.setCursor(0, 1);
    lcd.print("Can't start!");
    return;  // לא ממשיכים
  }

  delay(random(2000, 5000));

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("NOW!");
  digitalWrite(ledPin, HIGH);
  reactionStartTime = millis();
  waitingForReaction = true;
}

void loop() {
  if (waitingForReaction) {
    if (digitalRead(buttonPin) == LOW) {
      reactionEndTime = millis();
      unsigned long reactionTime = reactionEndTime - reactionStartTime;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Reaction time:");
      lcd.setCursor(0, 1);
      lcd.print(reactionTime);
      lcd.print(" ms");

      digitalWrite(ledPin, LOW);
      waitingForReaction = false;

      if (reactionTime > 4000) {
        tone(buzzerPin, 1000);
        myServo.write(90);
        delay(1000);
        myServo.write(0);
        noTone(buzzerPin);
      }
    }
  }
}
