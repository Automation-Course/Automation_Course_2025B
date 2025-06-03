#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>

// פיני RFID
#define SS_PIN 10
#define RST_PIN A0

MFRC522 mfrc522(SS_PIN, RST_PIN);

// פינים לרכיבים
const int buttonPin = 7;
const int ledPin = 6;
const int buzzerPin = 3;  // ← SIG של הבאזר הפאסיבי
const int servoPin = 9;

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myServo;

unsigned long signalTime = 0;
unsigned long reactionTime = 0;
bool authorized = false;

// UID מורשה
byte knownUID[4] = {0x4A, 0x49, 0x1A, 0xB0};

void setup() {
  Serial.begin(9600);  // ← הפעלת תקשורת לסריאל
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  myServo.attach(servoPin);
  myServo.write(90);  // מצב מנוחה

  lcd.begin(16, 2);
  lcd.backlight();
  lcd.print("Scan your card");

  // צפצוף באתחול
  tone(buzzerPin, 1000, 200);
}

void loop() {
  // שלב 1: זיהוי תג RFID
  if (!authorized) {
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      if (checkUID(mfrc522.uid.uidByte)) {
        authorized = true;
        lcd.clear();
        lcd.print("Welcome!");
        delay(1500);
        lcd.clear();
      } else {
        lcd.clear();
        lcd.print("Access Denied");
        tone(buzzerPin, 500, 400);  // צליל דחייה
        delay(1000);
        lcd.clear();
        lcd.print("Scan your card");
        return;
      }
    }
    return;
  }

  // שלב 2: אתגר רפלקסים
  lcd.setCursor(0, 0);
  lcd.print("Get Ready...");
  delay(random(2000, 5000));

  // הדלקת LED + באזר
  digitalWrite(ledPin, HIGH);
  tone(buzzerPin, 1000, 200);  // צליל בזמן ההדלקה

  signalTime = millis();
  lcd.clear();
  lcd.print("Push Button!");

  while (true) {
    if (digitalRead(buttonPin) == LOW) {
      reactionTime = millis() - signalTime;

      // 🔽 הדפסת זמן תגובה ל־Serial Monitor
      Serial.print("Reaction time: ");
      Serial.print(reactionTime);
      Serial.println(" ms");

      lcd.clear();
      lcd.print("Reaction:");
      lcd.setCursor(0, 1);
      lcd.print(reactionTime);
      lcd.print(" ms");

      if (reactionTime < 500) {
        myServo.write(120);
        tone(buzzerPin, 1200, 200);  // צליל תגמול
        delay(1000);
        myServo.write(90);
      }

      delay(3000);
      lcd.clear();
      lcd.print("Scan your card");
      authorized = false;
      break;
    }
  }

  digitalWrite(ledPin, LOW);
}

// פונקציה להשוואת UID
bool checkUID(byte *uid) {
  for (int i = 0; i < 4; i++) {
    if (uid[i] != knownUID[i]) return false;
  }
  return true;
}
