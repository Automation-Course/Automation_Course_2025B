#include <Wire.h>
#include <LiquidCrystal_I2C.h>  // For I2C LCD screen
#include <Servo.h>              // For servo motor

// Pin configuration
#define TRIG_PIN 5
#define ECHO_PIN 7
#define BUZZER_PIN 3
#define BUTTON_RIGHT 2
#define BUTTON_LEFT 13
#define SERVO_PIN 6
#define LED_PIN 8

// Components
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servo;

// System state variables
bool countdownStarted = false;
bool trainingStarted = false;
bool taskActive = false;
bool servoMoved = false;

// Task control
int currentTaskDirection = -1;             // -1 = none, 0 = left, 1 = right
unsigned long countdownStartTime = 0;
int countdownSeconds = 5;
int taskCount = 0;
const int maxTasks = 9;

// Timing
unsigned long taskStartTime = 0;
float totalReactionTime = 0;
float currentReactionTime = 0;

void setup() {
  Serial.begin(9600);                     // For debug output
  randomSeed(analogRead(A0));            // Random task direction

  lcd.init();                             // LCD setup
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MOVE TO START");

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  servo.attach(SERVO_PIN);
  servo.write(90);                        // Start at center position
}

void loop() {
  // Detect hand proximity to start countdown
  if (!countdownStarted && getDistanceCM() <= 10) {
    countdownStarted = true;
    countdownStartTime = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("STARTING IN...");
  }

  // Countdown before training begins
  if (countdownStarted && !trainingStarted) {
    int secondsPassed = (millis() - countdownStartTime) / 1000;
    int secondsLeft = countdownSeconds - secondsPassed;

    if (secondsLeft > 0) {
      lcd.setCursor(0, 1);
      lcd.print("              ");
      lcd.setCursor(6, 1);
      lcd.print(secondsLeft);
    }

    if (secondsLeft <= 0) {
      trainingStarted = true;
      digitalWrite(LED_PIN, HIGH);       // Turn on LED during training
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("LET'S START");
      playMarioStartTune();              // Play startup sound
      delay(1000);
      lcd.clear();
    }
  }

  // End training after completing all tasks
  if (taskCount >= maxTasks && trainingStarted) {
    trainingStarted = false;
    digitalWrite(LED_PIN, LOW);         // Turn off LED
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Training Is");
    lcd.setCursor(0, 1);
    lcd.print("Over");
    playGameOverTune();                 // End sound
    delay(3000);
    lcd.clear();

    // Display average reaction time
    float avgReaction = totalReactionTime / maxTasks;
    lcd.setCursor(0, 0);
    lcd.print("Avg Time:");
    lcd.setCursor(0, 1);
    lcd.print(avgReaction, 2);
    lcd.print(" sec");
    delay(5000);
    lcd.clear();

    // Reset system for new session
    countdownStarted = false;
    trainingStarted = false;
    taskActive = false;
    servoMoved = false;
    taskCount = 0;
    currentTaskDirection = -1;
    totalReactionTime = 0;
    taskStartTime = 0;
    currentReactionTime = 0;

    servo.write(90);                    // Return servo to center
    lcd.setCursor(0, 0);
    lcd.print("MOVE TO START");
  }

  // Create a new random task
  if (trainingStarted && !taskActive && taskCount < maxTasks) {
    currentTaskDirection = random(0, 2);      // 0 = left, 1 = right
    Serial.print("New Task: ");
    Serial.println(currentTaskDirection == 0 ? "LEFT" : "RIGHT");

    taskActive = true;
    servoMoved = true;
    taskStartTime = millis();
    currentReactionTime = 0;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Time: 0.00 sec");

    servo.write(currentTaskDirection == 0 ? 180 : 0); // Move servo
  }

  // Update timer display while waiting for user response
  if (taskActive) {
    currentReactionTime = (millis() - taskStartTime) / 1000.0;
    lcd.setCursor(0, 0);
    lcd.print("Time: ");
    lcd.print(currentReactionTime, 2);
    lcd.print(" sec ");
  }

  // Handle button input
  if (taskActive) {
    if (currentTaskDirection == 0 && digitalRead(BUTTON_LEFT) == LOW) {
      onTaskSuccess();
    } else if (currentTaskDirection == 1 && digitalRead(BUTTON_RIGHT) == LOW) {
      onTaskSuccess();
    } else if (currentTaskDirection == 0 && digitalRead(BUTTON_RIGHT) == LOW) {
      onTaskError();
    } else if (currentTaskDirection == 1 && digitalRead(BUTTON_LEFT) == LOW) {
      onTaskError();
    }
  }

  // 🔁 Print distance to Serial every 500ms
  static unsigned long lastDistancePrint = 0;
  if (millis() - lastDistancePrint >= 500) {
    getDistanceCM();
    lastDistancePrint = millis();
  }
}

// Measure distance using ultrasonic sensor and print to console
long getDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  long distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  return distance;
}

// Called when the user presses the correct button
void onTaskSuccess() {
  float reactionTime = (millis() - taskStartTime) / 1000.0;
  totalReactionTime += reactionTime;

  playSuccessTone();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WELL DONE!");
  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  lcd.print(reactionTime, 2);
  lcd.print(" sec");
  servo.write(90);
  delay(1500);

  taskCount++;
  taskActive = false;
  currentTaskDirection = -1;
  servoMoved = false;
}

// Called when the user presses the wrong button
void onTaskError() {
  playErrorTone();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WRONG BUTTON!");
  delay(1000);
  lcd.clear();
}

// Startup melody (can be replaced with another tune)
void playMarioStartTune() {
  tone(BUZZER_PIN, 660, 100); delay(150);
  tone(BUZZER_PIN, 660, 100); delay(300);
  tone(BUZZER_PIN, 660, 100); delay(300);
  tone(BUZZER_PIN, 510, 100); delay(150);
  tone(BUZZER_PIN, 660, 100); delay(300);
  tone(BUZZER_PIN, 770, 100); delay(550);
  tone(BUZZER_PIN, 380, 100); delay(575);
  noTone(BUZZER_PIN);
}

// Short tone for success
void playSuccessTone() {
  tone(BUZZER_PIN, 880, 100); delay(150);
  tone(BUZZER_PIN, 1046, 100); delay(150);
  tone(BUZZER_PIN, 1318, 100); delay(200);
  noTone(BUZZER_PIN);
}

// Short tone for error
void playErrorTone() {
  tone(BUZZER_PIN, 300, 200); delay(250);
  tone(BUZZER_PIN, 200, 200); delay(250);
  noTone(BUZZER_PIN);
}

// End-of-training melody
void playGameOverTune() {
  tone(BUZZER_PIN, 440, 200); delay(200);
  tone(BUZZER_PIN, 330, 200); delay(200);
  tone(BUZZER_PIN, 262, 200); delay(200);
  tone(BUZZER_PIN, 330, 200); delay(200);
  tone(BUZZER_PIN, 440, 200); delay(200);
  tone(BUZZER_PIN, 349, 400); delay(400);
  tone(BUZZER_PIN, 349, 400); delay(400);
  noTone(BUZZER_PIN);
}
