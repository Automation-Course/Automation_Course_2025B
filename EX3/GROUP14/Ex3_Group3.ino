#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int buttonPin = 6;
const int ledPin = 13;
const int trigPin = 7;
const int echoPin = 8;
const int ldrPin = A0;
const int servoPin = 9;

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Initialize LCD with I2C address 0x27
Servo myServo;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);   // Button uses internal pull-up resistor
  pinMode(ledPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  lcd.begin(16, 2);      // Start LCD
  lcd.backlight();       // Turn on backlight
  Serial.begin(9600);    // Start Serial Monitor
  lcd.print("System Ready");
  delay(1500);
  lcd.clear();

  randomSeed(analogRead(A5));  // Initialize random seed for task selection
}

void loop() {
  if (digitalRead(buttonPin) == LOW) {
    lcd.clear();
    lcd.print("New Task Incoming!");
    delay(1000);
    lcd.clear();

    int task = random(1, 3); // Randomly choose 1 = light, 2 = distance
    bool success = false;
    unsigned long reactionTime = 0;
    unsigned long timeout = random(1000, 3001);  // Set a time limit between 1¨C3 seconds

    if (task == 1) {
      // Light blocking task
      Serial.println("========== NEW TASK ==========");
      Serial.println("Task: Block the light");
      Serial.print("Time limit: ");
      Serial.print(timeout / 1000.0, 1);
      Serial.println(" seconds");

      lcd.setCursor(0, 0);
      lcd.print("Task: Block light");
      lcd.setCursor(0, 1);
      lcd.print("Time limit: ");
      lcd.print(timeout / 1000.0, 1);
      lcd.print("s");
      delay(2000);
      lcd.clear();

      unsigned long startTime = millis();
      while (millis() - startTime < timeout) {
        int lightVal = analogRead(ldrPin);
        if (lightVal > 250) {
          reactionTime = millis() - startTime;
          success = true;
          break;
        }
      }

    } else if (task == 2) {
      // Distance target task
      int targetDist = random(1, 8);  // narrowed range to 1¨C8 cm

      Serial.println("========== NEW TASK ==========");
      Serial.println("Task: Reach target distance");
      Serial.print("Target distance: ");
      Serial.print(targetDist);
      Serial.println(" cm");
      Serial.print("Time limit: ");
      Serial.print(timeout / 1000.0, 1);
      Serial.println(" seconds");

      lcd.setCursor(0, 0);
      lcd.print("Reach ");
      lcd.print(targetDist);
      lcd.print("cm!");
      lcd.setCursor(0, 1);
      lcd.print("Time: ");
      lcd.print(timeout / 1000.0, 1);
      lcd.print("s");
      delay(2000);
      lcd.clear();

      unsigned long startTime = millis();
      while (millis() - startTime < timeout) {
        long duration;
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
        duration = pulseIn(echoPin, HIGH);
        unsigned long distance = duration / 58;

        if (distance >= targetDist - 0.3 && distance <= targetDist + 0.3) {
          reactionTime = millis() - startTime;
          success = true;
          break;
        }
      }
    }

    lcd.clear();
    if (success) {
      // Show success on LCD + Serial
      lcd.setCursor(0, 0);
      lcd.print("Success!");

      lcd.setCursor(0, 1);
      lcd.print("                ");  // clear line
      lcd.setCursor(0, 1);
      lcd.print("Time: ");
      lcd.print(reactionTime/1000.0, 4);
      lcd.print("s");

      Serial.println("SUCCESS!");
      Serial.print("Reaction time: ");
      Serial.print(reactionTime/1000.0, 4);
      Serial.println(" seconds");

      digitalWrite(ledPin, HIGH);

      // servo motion and detach
      myServo.attach(servoPin);
      myServo.write(90);
      delay(1500);
      myServo.write(0);
      delay(500);
      myServo.detach();
    } else {
      // Show failure
      lcd.setCursor(0, 0);
      lcd.print("Too slow!");
      lcd.setCursor(0, 1);
      lcd.print("Try again.");

      Serial.println("FAILED: User did not respond in time.");
    }

    digitalWrite(ledPin, LOW);
    delay(2000);
    lcd.clear();
    lcd.print("Ready again");
  }
}