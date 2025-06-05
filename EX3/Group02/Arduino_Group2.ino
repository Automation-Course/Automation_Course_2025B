#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Define pin connections
#define SS_PIN 10
#define RST_PIN 9
#define BUTTON_PIN 8
#define BUZZER_PIN 5
#define TRIG_PIN 3
#define ECHO_PIN 4

Servo flagServo;  // יצירת אובייקט סרבו

MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);


bool waitingForButton = false;
bool trainingStarted = false;
int shotCount = 0;

// Play intro melody when training starts
void playMarioKartIntro() {
  playNote(987, 100);  // B5
  delay(800);
  playNote(987, 100);  // B5
  delay(800);
  playNote(987, 100);  // B5
  delay(800);

 
  playNote(1319, 500); // E6
  delay(800);
}

// Helper function to play a tone
void playNote(int frequency, int duration) {
  tone(BUZZER_PIN, frequency, duration);
  delay(duration * 1.3);
}

// Calculate distance in cm from ultrasonic sensor
long readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  long distance = duration * 0.034 / 2;
  return distance;
}

void setup() {
Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Scan your tag");

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  flagServo.attach(6);  // חיבור לפין D6
  flagServo.write(90);  // עצירה בהתחלה


}

void loop() {
   // === PART 1: Waiting for RFID scan ===
    if (!waitingForButton && !trainingStarted) {
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
      String knownTag = "DA 6F FC B0";
      String knownName = "Oriya";

      String tagID = "";
      if (rfid.uid.size > 0) tagID += String(rfid.uid.uidByte[0], HEX);
      if (rfid.uid.size > 1) tagID += " " + String(rfid.uid.uidByte[1], HEX);
      if (rfid.uid.size > 2) tagID += " " + String(rfid.uid.uidByte[2], HEX);
      if (rfid.uid.size > 3) tagID += " " + String(rfid.uid.uidByte[3], HEX);
      tagID.toUpperCase();

      lcd.clear();
      lcd.setCursor(0, 0);

      if (tagID == knownTag) {
          // If tag matches, show welcome and get ready for button press
        lcd.print("Welcome " + knownName);
        Serial.println(knownName + " has entered the facility");
      } else {
           // If tag is unknown, reset and wait for next scan
        lcd.print("Unknown tag");
        delay(1000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Scan your tag");
        Serial.println("System is ready to register a trainee");
        waitingForButton = false;
        rfid.PICC_HaltA();
        return;
      }

      waitingForButton = true;
      rfid.PICC_HaltA();
    }
  }
  // === PART 2: Start training after button press ===
  if (waitingForButton && !trainingStarted) {
    if (digitalRead(BUTTON_PIN) == HIGH) {
      Serial.println("Button pushed for basketball shooting training");
      playMarioKartIntro();

      trainingStarted = true;
      waitingForButton = false;
      shotCount = 0;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Training started");
      lcd.setCursor(0, 1);
      lcd.print("Shots: 0");
      delay(500);
    }
  }
  // === PART 3: Active training session ===
  if (trainingStarted) {
    long distance = readDistanceCM();

    if (distance > 0 && distance < 8) {
      shotCount++;
      lcd.setCursor(0, 1);
      lcd.print("Shots: ");
      lcd.print(shotCount);
      lcd.print("   ");
      flagServo.write(180);
      delay(1000);
      flagServo.write(90);
      delay(1000);
      Serial.println("Successful shot");
    }
   // Stop training if button is pressed again
    if (digitalRead(BUTTON_PIN) == HIGH) {
      Serial.println("Training was stopped by the user, total successful shots: " + String(shotCount));
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Bye!");
      lcd.setCursor(0, 1);
      lcd.print("Shots: ");
      lcd.print(shotCount);

      delay(1000);
  // Reset to initial state, ready for new scan
      trainingStarted = false;
      waitingForButton = false;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Scan your tag");
      Serial.println("System is ready to register a trainee");
    }
  }

}