// Import libs

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>


// Pin definitions
#define BUTTON_PIN 2
#define BUZZER_PIN 8
#define SERVO_PIN 9
#define TRIG_PIN 6
#define ECHO_PIN 10

LiquidCrystal_I2C lcd(0x27, 16, 2);  // create LCD screen
Servo myServo;                       // define servo

// Training configuration
const int TOTAL_SETS = 3;
const int SERVO_DEGREES_PER_SET = 70;
const int INITIAL_ANGLE = 0;

// State variables
int currentAngle = INITIAL_ANGLE;
bool trainigIsOff = true;
unsigned long pressStartTime = -1;  // for button in funct isButtonHeld
bool alreadyReported = false;       // for button in funct isButtonHeld
int squatCount = 0;                 // count the current num of squats in a set
int totalSquats = 0;
int setCount = 0;
bool traningStarted = false;
unsigned long squatStartTime = 0;
bool inSquat = false;
bool waitingToExitSquat = false;
bool setFinished = false;
bool programStarted = false;

// countdown veriables
bool countdownStarted = false;
bool countdownFinished = false;
unsigned long lastUpdateTime = 0;
int countdownValue = 10;


// Custom heart character for LCD animation
byte heartChar[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};


void setup() {
lcd.createChar(3, heartChar); // Save custom character in slot 3

  Serial.begin(9600);  // start serial
  lcd.begin(16, 2);    
  lcd.backlight();    

  myServo.attach(SERVO_PIN);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

// Display instruction before program starts
void beforeStarted(){
    lcd.setCursor(0, 0);
     lcd.print("hold the button");
}

// Play startup tune
void playWorkoutStartTune() {
  int melody[] = { 262, 330, 392, 523, 659, 784, 1046 };
  int noteDuration = 200;
  for (int i = 0; i < 7; i++) {
    tone(BUZZER_PIN, melody[i], noteDuration);
    delay(noteDuration * 1.3);
  }
}

// Play tune after completing a set
void playSetCompleteTune() {
  int melody[] = { 392, 440, 392, 330, 262 };
  int durations[] = { 150, 150, 150, 300, 500 };
  for (int i = 0; i < 5; i++) {
    tone(BUZZER_PIN, melody[i], durations[i]);
    delay(durations[i] + 50);
  }
}

// Play tune after completing entire workout
void playWorkoutCompleteTune() {
  int melody[] = { 523, 659, 784, 659, 784, 880, 988, 1046 };
  int noteDuration = 180;
  for (int i = 0; i < 8; i++) {
    tone(BUZZER_PIN, melody[i], noteDuration);
    delay(noteDuration + 30);
  }
  tone(BUZZER_PIN, 1318, 500); 
  delay(500);
}

// Check if button is held for specified time
bool isButtonHeld(int buttonPin, unsigned long holdTime = 5000) {

  bool pressed = digitalRead(buttonPin) == 1;

  if (pressed) {

    if (pressStartTime == -1) {
      pressStartTime = millis(); 
    } else if (!alreadyReported && millis() - pressStartTime >= 1000) {
      alreadyReported = true;
      return true;
    }
  } else {
    pressStartTime = -1;
    alreadyReported = false;
  }

  return false;
}

// Start training session
void startTraning() {
 lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("you can start");
  lcd.setCursor(0, 1);
  lcd.print("Training :)");
  traningStarted = true;
  playWorkoutStartTune();
  delay(2000);
  lcd.clear();

}

// Display number of squats in current set
void displaySetInfo(int num) {  // this method get number of squats in set and display them on the screen
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Squats in set:");
  lcd.setCursor(0, 1);
  lcd.print(num);
}

// Measure distance using ultrasonic sensor
float getDistanceCM() {  // this method calc the distance using the sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  float distance = duration * 0.034 / 2;

  return distance;
}

// Start countdown before next set
void startCountdown() {
  if (!countdownStarted && !countdownFinished) {
    countdownStarted = true;
    countdownValue = 10;
    lastUpdateTime = millis();
    lcd.clear();

  }
}

// Update countdown every second
void updateCountdown() {
  if (countdownStarted && millis() - lastUpdateTime >= 1000) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("new set will ");
    lcd.setCursor(0, 1);
    lcd.print("start in: " + String(countdownValue));

    countdownValue--;
      
    tone(BUZZER_PIN, 1000, 100);
    lastUpdateTime = millis();

    if (countdownValue < 0) {
      tone(BUZZER_PIN, 1500, 500);
      explosionAnimation();
      tone(BUZZER_PIN, 1500, 500);
      explosionAnimation();
      delay(1000);
      countdownStarted = false;
      countdownFinished = true;

    }
  }
}
void explosionAnimation() {
  lcd.clear();

  lcd.setCursor(7, 0); lcd.print("\x03"); 
  delay(150);

  lcd.setCursor(6, 0); lcd.print("\x03");
  lcd.setCursor(8, 0); lcd.print("\x03");
  lcd.setCursor(7, 1); lcd.print("\x03");
  delay(150);

  lcd.setCursor(5, 0); lcd.print("\x03");
  lcd.setCursor(9, 0); lcd.print("\x03");
  lcd.setCursor(6, 1); lcd.print("\x03");
  lcd.setCursor(8, 1); lcd.print("\x03");
  delay(150);

  lcd.setCursor(4, 0); lcd.print("\x03");
  lcd.setCursor(10, 0); lcd.print("\x03");
  lcd.setCursor(5, 1); lcd.print("\x03");
  lcd.setCursor(9, 1); lcd.print("\x03");
  delay(200);

  lcd.clear();
}

// Rotate 360° servo in specified direction
void rotateServo360(String direction, int degrees) {
  int timePerDegree = 7;
  int rotationTime = degrees * timePerDegree;

  if (direction == "CW") {
    myServo.write(120);
  } else if (direction == "CCW") {
    myServo.write(60);
    rotationTime= rotationTime* setCount;
  }

  delay(rotationTime);
  myServo.write(90);  
}

// Reset servo to initial position
void resetServo() {
  myServo.write(INITIAL_ANGLE);
  currentAngle = INITIAL_ANGLE;
  delay(500);  
}

// Handle squat detection and counting
void set() {
  float distance = getDistanceCM();

  if (distance < 50.0) {
    if (!inSquat && !waitingToExitSquat) {
      squatStartTime = millis();
      inSquat = true;
    } else if (inSquat && millis() - squatStartTime >= 3000) {
      squatCount++;
      totalSquats++;
      tone(BUZZER_PIN, 2222, 100);
      displaySetInfo(squatCount);

      waitingToExitSquat = true;
      inSquat = false;

      if (squatCount == 10) {
        whenSetFinishedSeq();
      }
    }
  } else {

    if (waitingToExitSquat) {
      waitingToExitSquat = false;
    }
    inSquat = false;
  }
}

// Actions when a set is completed
void whenSetFinishedSeq() {
  rotateServo360("CW", SERVO_DEGREES_PER_SET);
  setCount++;
  delay(2000);
  setFinished = true;  // flag finish set
  traningStarted = false;

  // reset countdown
  countdownFinished = false;
  countdownStarted = false;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("set finished :)");

  playSetCompleteTune();
  delay(1500);
  squatCount = 0;  // reset squt counter
}

// Handles set transition and countdown
void setFinished_sequence() {
  startCountdown();
  updateCountdown();
 if (countdownFinished) {
    displaySetInfo(0);
    traningStarted = true;
    setFinished = false;
}
}

// Final screen and reset after all sets completed
void LastSet(){
    playWorkoutCompleteTune();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Workout done!");
    lcd.setCursor(0, 1);
    lcd.print("Great job :)");

    delay(3000);
    rotateServo360("CCW", SERVO_DEGREES_PER_SET);
    setCount = 0;
    totalSquats = 0;
    squatCount = 0;
    countdownStarted = false;
    countdownFinished = false;
    traningStarted = false;
    setFinished = false;
    programStarted = false;
    lcd.clear();
}

// Main loop
void loop() {

  if (!programStarted){
    beforeStarted();
  }
  
  if (isButtonHeld(BUTTON_PIN) && !programStarted) {  // if button held shotrly - display "start tranign screen"
    startTraning();
    displaySetInfo(0);
    programStarted = true;
  }

  if (traningStarted) {
    set();
  }

  if (setFinished) {
   if (setCount < TOTAL_SETS){
    setFinished_sequence();
    }
  }

  if (setCount >= TOTAL_SETS || (isButtonHeld(BUTTON_PIN) && programStarted )) {
    LastSet();
}
}