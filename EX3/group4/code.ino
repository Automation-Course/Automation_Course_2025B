#include <Wire.h>
#include <LiquidCrystal_I2C.h>   // Library for LCD display
#include <Servo.h>                // Library for controlling the servo motor

LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD with I2C address 0x27 and 16x2 size

// Pin definitions
const int trigPin = 5;             // Ultrasonic sensor trigger pin
const int echoPin = 6;             // Ultrasonic sensor echo pin
const int servoPin = 9;            // Servo motor control pin
const int pourButtonPin = 2;       // Pushbutton to trigger water pour
const int ledPin = 7;              // LED output for visual indication
const int buzzerPin = 10;          // Buzzer for reminders
const int startButtonPin = 4;      // Button to start/end the training

Servo myServo;

// System state variables
bool trainingStarted = false;       // True if training has started
bool trainingLogicActive = false;   // True after 15s delay has passed and logic is active
bool trainingEnded = false;         // True if training ended after second button press
bool pouring = false;               // True when currently pouring water
bool waitingForRelease = false;     // True when waiting for pour button to be released
bool reminderPlayed = false;        // Tracks if 20s reminder was already played
bool inActiveMessage = false;       // Prevents actions while temporary message is displayed

// Time tracking variables
unsigned long trainingStartTime = 0; // Start time of training
unsigned long pourStartTime = 0;     // Time when pouring started
unsigned long lastPourEndTime = 0;   // Time of last completed pour

int cupCounter = 0;                  // Number of cups poured in this session
int buttonPressCount = 0;            // Count of presses on the start button

// Debounce and press detection variables
bool lastStartButtonState = HIGH;    // Previous state of start button
bool buttonHandled = false;          // Ensures one action per press
bool readyToDetectPress = false;     // True if button was released and ready to press

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  // Configure pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(pourButtonPin, INPUT_PULLUP);
  pinMode(startButtonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  myServo.attach(servoPin);
  myServo.write(90); // Set servo to neutral position

  // Display welcome message
  lcd.setCursor(0, 0);
  lcd.print("Press to start");
  lcd.setCursor(0, 1);
  lcd.print("the training");
}

void loop() {
  unsigned long currentTime = millis(); // Get current time

  // Read current state of the start button
  bool currentStartState = digitalRead(startButtonPin);

  // Detect that the button was released — now it’s okay to detect press
  if (currentStartState == HIGH) {
    readyToDetectPress = true;
    buttonHandled = false;
  }

  // Detect transition from not pressed to pressed
  if (readyToDetectPress &&
      lastStartButtonState == HIGH &&
      currentStartState == LOW &&
      !buttonHandled) {

    buttonHandled = true;
    buttonPressCount++;

    // First press: start training session
    if (buttonPressCount == 1 && !trainingStarted) {
      trainingStarted = true;
      trainingStartTime = currentTime;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Let's hydrate");
      lcd.setCursor(0, 1);
      lcd.print("Cups: ");
      lcd.print(cupCounter);
    }

    // Second press: end training and reset after delay
    else if (buttonPressCount == 2 && trainingStarted) {
      trainingStarted = false;
      trainingEnded = true;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Training ended");
      lcd.setCursor(0, 1);
      lcd.print("Cups: ");
      lcd.print(cupCounter);

      delay(5000); // Hold end message for 5 seconds

      // Reset all states
      trainingEnded = false;
      trainingLogicActive = false;
      buttonPressCount = 0;
      cupCounter = 0;
      pouring = false;
      waitingForRelease = false;
      reminderPlayed = false;
      inActiveMessage = false;

      readyToDetectPress = false;
      buttonHandled = false;
      lastStartButtonState = digitalRead(startButtonPin); // Update state

      // Show welcome message again
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Press to start");
      lcd.setCursor(0, 1);
      lcd.print("the training");
      return;
    }
  }

  // Save the current button state for the next loop
  lastStartButtonState = currentStartState;

  // Stop logic if training hasn’t started or already ended
  if (!trainingStarted || trainingEnded) return;

  // Wait 15s after start, then activate logic
  if (!trainingLogicActive && currentTime - trainingStartTime >= 15000) {
    playOdeToJoy(buzzerPin);
    showActiveMessage("Insert cup");
    delay(2000);
    showIdleScreen();
    trainingLogicActive = true;
  }

  if (!trainingLogicActive) return;

  // Read distance from ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;
  bool cupDetected = (distance > 0 && distance < 10); // Considered present if within 10cm

  // Reminder if 20 seconds passed since last pour
  if (lastPourEndTime > 0 && !reminderPlayed && currentTime - lastPourEndTime >= 20000) {
    showActiveMessage("Insert cup");
    playOdeToJoy(buzzerPin);
    reminderPlayed = true;
    delay(2000);
    showIdleScreen();
  }

  // Stop pouring after 4 seconds
  if (pouring && currentTime - pourStartTime >= 4000) {
    myServo.write(90); // Stop servo
    pouring = false;

    showActiveMessage("Done");
    digitalWrite(ledPin, HIGH);
    delay(3000);
    digitalWrite(ledPin, LOW);

    lastPourEndTime = currentTime;
    reminderPlayed = false;
    cupCounter++;
    showIdleScreen();
    return;
  }

  if (inActiveMessage) return;

  // No cup detected — reset waiting state
  if (!cupDetected) {
    waitingForRelease = false;
    return;
  }

  // Button pressed while cup detected
  if (digitalRead(pourButtonPin) == LOW) {
    showActiveMessage("Release btn");
    waitingForRelease = true;
    return;
  }

  // Pouring starts when button released
  if (waitingForRelease && digitalRead(pourButtonPin) == HIGH) {
    pouring = true;
    pourStartTime = currentTime;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Pouring...");
    lcd.setCursor(0, 1);
    lcd.print("               ");

    myServo.write(120); // Start pouring
    waitingForRelease = false;
    return;
  }
}

// Show idle screen with cup count
void showIdleScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Let's hydrate");
  lcd.setCursor(0, 1);
  lcd.print("Cups: ");
  lcd.print(cupCounter);
}

// Show a temporary message on LCD
void showActiveMessage(String msg) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(msg);
  lcd.setCursor(0, 1);
  lcd.print("                ");
}

// Buzzer melody reminder
void playOdeToJoy(int pin) {
  tone(pin, 330, 300); delay(350);
  tone(pin, 330, 300); delay(350);
  tone(pin, 349, 300); delay(350);
  tone(pin, 392, 300); delay(350);
  tone(pin, 392, 300); delay(350);
  tone(pin, 349, 300); delay(350);
  tone(pin, 330, 300); delay(350);
  tone(pin, 294, 300); delay(350);
  tone(pin, 262, 300); delay(350);
  tone(pin, 262, 300); delay(350);
  tone(pin, 294, 300); delay(350);
  tone(pin, 330, 300); delay(350);
  tone(pin, 330, 300); delay(350);
  tone(pin, 294, 300); delay(350);
  tone(pin, 294, 450); delay(500);
  noTone(pin);
}