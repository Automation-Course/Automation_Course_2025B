#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include "pitches.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define pins
#define BUTTON_PIN 7
#define PIR_PIN 8
#define SERVO_PIN 9
#define LED_GREEN 12
#define LED_RED 13
#define BUZZER_PIN 10

// State variables
bool started = false;
int totalCount = 0;
int currentRound = 1;
unsigned long startTime = 0;
const unsigned long ROUND_DURATION = 15000;
const unsigned long PAUSE_DURATION = 3000;
Servo punchServo;
static int lastPIRState = LOW;
static unsigned long lastDetectionTime = 0;
const unsigned long COOLDOWN_PERIOD = 150;

// Flags to prevent repeated printing
bool roundAnnounced = false;
bool restAnnounced = false;
bool finishAnnounced = false;

// Melody data
int tempo = 76;
int wholenote = (60000 * 4) / tempo;

int melody[] = {
  NOTE_G4, 4, NOTE_G4, 4,
  NOTE_AS4, -4, NOTE_G4, 8, NOTE_G4, 4,
  NOTE_AS4, 4, REST, 4, NOTE_G4, 8, NOTE_AS4, 8,
  NOTE_DS5, 4, NOTE_D5, -4, NOTE_C5, 8,
  NOTE_C5, 4, NOTE_AS4, 4, NOTE_F4, 8, NOTE_G4, 8,
  NOTE_GS4, 4, NOTE_F4, 4, NOTE_F4, 8, NOTE_G4, 8,
  NOTE_GS4, 4, REST, 4, NOTE_F4, 8, NOTE_GS4, 8,
  NOTE_D5, 8, NOTE_C5, 8, NOTE_AS4, 4, NOTE_D5, 4,
  NOTE_DS5, 4, REST, 4, NOTE_DS4, 8, NOTE_DS4, 8,
  NOTE_DS5, 2, NOTE_C5, 8, NOTE_GS4, 8,
  NOTE_AS4, 2, NOTE_G4, 8, NOTE_DS4, 8,
  NOTE_GS4, 4, NOTE_AS4, 4, NOTE_C5, 4,
  NOTE_AS4, 2, NOTE_DS4, 8, NOTE_DS4, 8,
  NOTE_DS5, 2, NOTE_C5, 8, NOTE_GS4, 8,
  NOTE_AS4, 2, NOTE_G4, 8, NOTE_DS4, 8,
  NOTE_AS4, 4, NOTE_G4, 4, NOTE_DS4, 4,
  NOTE_DS4, 2
};

int notes = sizeof(melody) / sizeof(melody[0]) / 2;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();


  lcd.print("Welcome!");
  lcd.setCursor(0, 1);
  lcd.print("Press the button!");
  Serial.println("System initialized. Waiting for button press...");

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);

  if (!started && buttonState == HIGH) {
    delay(50);
    if (digitalRead(BUTTON_PIN) == HIGH) {
      lcd.clear();
      lcd.print("Starting...    ");
      delay(1000);
      lcd.clear();
      startTime = millis();
      started = true;
      currentRound = 1;
      totalCount = 0;
      roundAnnounced = false;
      restAnnounced = false;
      finishAnnounced = false;
      Serial.println("Button pressed - workout started.");
    }
  }

  if (started) {
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - startTime;

    if (currentRound <= 4) {
      if (elapsedTime < ROUND_DURATION) {
        lcd.setCursor(0, 0);
        lcd.print("Position " + String(currentRound) + "    ");

        if (!roundAnnounced  && currentRound <= 4) {
          Serial.print("Round ");
          Serial.print(currentRound);
          Serial.println(" started.");
          roundAnnounced = true;
          restAnnounced = false;
        }

        int pirState = digitalRead(PIR_PIN);
        if (pirState == HIGH && lastPIRState == LOW && (millis() - lastDetectionTime >= COOLDOWN_PERIOD)) {
          totalCount++;
          lastDetectionTime = millis();
          digitalWrite(LED_RED, HIGH);
          digitalWrite(LED_GREEN, LOW);
          Serial.print("Motion detected! Total count: ");
          Serial.println(totalCount);
        } else {
          digitalWrite(LED_RED, LOW);
          digitalWrite(LED_GREEN, pirState == LOW ? HIGH : LOW);
          digitalWrite(LED_RED, pirState == HIGH ? HIGH : LOW);
          lastPIRState = pirState;
        }

      } else if (elapsedTime < ROUND_DURATION + PAUSE_DURATION) {
        lcd.setCursor(0, 0);
        lcd.print("Resting...     ");
        playBeep();
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_GREEN, LOW);

        if (!restAnnounced) {
          Serial.println("Resting...");
          restAnnounced = true;
        }

      } else if(currentRound <= 4) {
        currentRound++;
        startTime = millis();
        roundAnnounced = false;
        restAnnounced = false;
        finishAnnounced = false;
        Serial.print("Moving to next round: ");
        Serial.println(currentRound);
      }

    } else if (currentRound == 5) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Good Job!     ");
      lcd.setCursor(0, 1);
      lcd.print("Moves: ");
      lcd.print(totalCount);
      delay(3000);

      if (!finishAnnounced) {
        Serial.print("Workout finished. Total moves: ");
        Serial.println(totalCount);
        finishAnnounced = true;
      }

      lcd.clear();
      lcd.print("Take a deep breath");
      delay(500);

      punchServo.attach(SERVO_PIN);  // Connect servo
      delay(500);
      punchServo.write(180);         // Move to 180°
      delay(4000);                   // Stay at 180° for 4 seconds
      punchServo.detach();           // Disconnect


      lcd.clear();
      lcd.print("Shavasna");
      Serial.println("Playing melody...");
      playMelody();

      lcd.clear();
      lcd.print("Press the button!");
      Serial.println("System ready for next workout.");
      started = false;
    }
  }
}

void playMelody() {
  int divider = 0;
  int noteDuration = 0;

  for (int thisNote = 0; thisNote < notes * 2; thisNote += 2) {
    divider = melody[thisNote + 1];

    if (divider > 0) {
      noteDuration = wholenote / divider;
    } else if (divider < 0) {
      noteDuration = (wholenote / abs(divider)) * 1.5;
    }

    tone(BUZZER_PIN, melody[thisNote], noteDuration * 0.9);
    delay(noteDuration);
    noTone(BUZZER_PIN);
  }
}

void playBeep() {
  tone(BUZZER_PIN, 1000, 500);
  delay(500);
  noTone(BUZZER_PIN);
}
