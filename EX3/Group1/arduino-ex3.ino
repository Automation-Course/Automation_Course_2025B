#include <Servo.h>
#include <LiquidCrystal_I2C.h>

// --- Pin Definitions ---
const int trigPin = 4;
const int echoPin = 5;
const int piezoPin = 6;
const int buttonPin = 2;
const int servoPin = 9;

// --- System Objects ---
Servo myServo;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- Variables ---
int repCount = 0;
int repsInCurrentSet = 0;
int totalSets = 0;

bool inPushup = false;
bool inSet = false;
bool inRest = false;

unsigned long setStartTime = 0;
unsigned long lastMotionTime = 0;
unsigned long restStartTime = 0;
unsigned long currentTime = 0;

long duration;
int distance;
int thresholdDistance = 15;

//
// Initializes hardware: pins, servo, LCD, and serial monitor.
//
void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(piezoPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  myServo.attach(servoPin);

  lcd.init();
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Ready");

  Serial.println("System initialized.");
  myServo.write(90);  // neutral position
}

//
// Main program loop: handles reset, set detection, push-up logic, and rest.
//
void loop() {
  currentTime = millis();

  // --- Reset Button ---
  if (digitalRead(buttonPin) == LOW) {
    repCount = 0;
    repsInCurrentSet = 0;
    totalSets = 0;
    inSet = false;
    inRest = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("All reset");
    myServo.write(90);
    Serial.println("System reset by button.");
    delay(500);
  }

  // --- Auto-start first set ---
  if (!inSet && !inRest && repCount == 0) {
    inSet = true;
    repsInCurrentSet = 0;
    totalSets++;
    setStartTime = currentTime;
    lastMotionTime = currentTime;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Set ");
    lcd.print(totalSets);
    lcd.print(" Started");
    tone(piezoPin, 1000, 200);
    Serial.print("Set ");
    Serial.print(totalSets);
    Serial.println(" started.");
    delay(500);
  }

  // --- During active set ---
  if (inSet) {
    distance = readDistance();

    if (distance < thresholdDistance && !inPushup) {
      inPushup = true;
      repCount++;
      repsInCurrentSet++;
      lastMotionTime = currentTime;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Set ");
      lcd.print(totalSets);
      lcd.setCursor(0, 1);
      lcd.print("Reps: ");
      lcd.print(repsInCurrentSet);
      tone(piezoPin, 1200, 100);

      Serial.print("Push-up detected. Total reps: ");
      Serial.println(repCount);
    }

    if (distance >= thresholdDistance + 5 && inPushup) {
      inPushup = false;
      lastMotionTime = currentTime;
    }

    // --- End set after 10s of inactivity ---
    if ((currentTime - lastMotionTime) > 10000) {
      inSet = false;
      inRest = true;
      restStartTime = currentTime;

      myServo.write(0); // rotate during rest

      unsigned long setDuration = (currentTime - setStartTime) / 1000;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Set ");
      lcd.print(totalSets);
      lcd.print(" Done");
      lcd.setCursor(0, 1);
      lcd.print("Reps:");
      lcd.print(repsInCurrentSet);
      lcd.print(" Time:");
      lcd.print(setDuration - 10); // subtract rest time
      lcd.print("s");

      tone(piezoPin, 800, 300);
      Serial.print("Set ");
      Serial.print(totalSets);
      Serial.print(" ended. Reps: ");
      Serial.print(repsInCurrentSet);
      Serial.print(". Time (adjusted): ");
      Serial.print(setDuration - 10);
      Serial.println("s");

      Serial.println("Rest started. Servo rotating...");
      delay(2000);
    }
  }

  // --- During rest period ---
  if (inRest) {
    unsigned long elapsed = currentTime - restStartTime;
    if (elapsed >= 13000) { //(10 seconds rest + 3 seconds delay)
      myServo.write(90); // stop servo
      Serial.println("Rest ended. Servo stopped.");

      inRest = false;
      inSet = true;
      repsInCurrentSet = 0;
      totalSets++;
      setStartTime = currentTime;
      lastMotionTime = currentTime;

      delay(3000);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Set ");
      lcd.print(totalSets);
      lcd.print(" Started");
      tone(piezoPin, 1000, 300);

      Serial.print("Set ");
      Serial.print(totalSets);
      Serial.println(" started.");
      delay(2000);
      lcd.clear();
    } else {
      unsigned long timeLeft = (13000 - elapsed) / 1000;
      lcd.setCursor(0, 0);
      lcd.print("Resting...       ");
      lcd.setCursor(0, 1);
      lcd.print("Time left: ");
      lcd.print(timeLeft);
      lcd.print("s  ");
    }
  }
}

//
// Returns distance in cm using the ultrasonic sensor.
//
int readDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;
}
