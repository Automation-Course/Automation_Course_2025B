#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Component definitions
LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD I2C
Servo trainingServo;                  // Servo motor
const int buzzerPin = 8;              // Buzzer pin
const int servoPin = 9;               // Servo motor pin
const int buttonPin = 2;              // Button pin
const int trigPin = 3;                // Ultrasonic Trig pin
const int echoPin = 4;                // Ultrasonic Echo pin

// Sound frequency definitions
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523

// Training system variables
int currentAngle = 90;               // Current servo angle (rest position)
int menuSelection = 0;               // Menu selection
bool isTraining = false;             // Training in progress flag
bool systemReady = true;             // System ready flag
bool waitingForStart = false;        // Waiting for start after workout complete
unsigned long lastButtonPress = 0;   // For button debouncing
unsigned long lastDisplayUpdate = 0; // For display update control
const unsigned long debounceDelay = 300;
const unsigned long displayUpdateInterval = 500; // Update display every 500ms

// Ultrasonic sensor variables
float currentDistance = 0;           // Current distance in cm
float baselineDistance = 0;          // Baseline distance (standing position)
int repCount = 0;                    // Number of detected reps
bool wasClose = false;               // Was close to sensor (for rep counting)
const float proximityThreshold = 5.0; // Proximity threshold for rep counting (5cm)
unsigned long lastRepTime = 0;       // Last rep detection time
const unsigned long repCooldown = 800; // Cooldown between reps (800ms)

// Training menu
String workoutMenu[] = {
  "1.Do 10 pushups",   // Count proximity touches
  "2.Timed Touches",   // Count touches in time limit
  "3.Hold Duration"    // Measure continuous hold time
};
const int menuSize = 3;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting Smart Training System...");
 
  // Initialize components
  lcd.init();
  lcd.backlight();
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  trainingServo.attach(servoPin);
 
  // Reset to rest position
  trainingServo.write(90);
  currentAngle = 90;
 
  // System check
  systemCheck();
 
  // Calibrate ultrasonic sensor
  calibrateSensor();
 
  // Show main menu
  showMainMenu();
}

void loop() {
  // Check button press
  if (digitalRead(buttonPin) == LOW) {
    if (millis() - lastButtonPress > debounceDelay) {
      handleButtonPress();
      lastButtonPress = millis();
    }
  }
 
  // Update display only occasionally to reduce load
  if (!isTraining && !waitingForStart && (millis() - lastDisplayUpdate > displayUpdateInterval)) {
    updateMainMenuDisplay();
    lastDisplayUpdate = millis();
  }
 
  delay(50);  // Small delay to prevent CPU overload
}

void systemCheck() {
  Serial.println("Performing system check...");
 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System Check");
  lcd.setCursor(0, 1);
  lcd.print("Testing...");
  delay(1500);
 
  // Test LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LCD: OK");
  delay(800);
 
  // Test buzzer
  lcd.setCursor(0, 1);
  lcd.print("Buzzer: Testing");
  tone(buzzerPin, NOTE_A4, 300);
  delay(500);
  lcd.setCursor(8, 1);
  lcd.print("OK");
  delay(800);
 
  // Test ultrasonic sensor
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Testing Sensor");
 
  for (int i = 0; i < 3; i++) {
    float distance = measureDistance();
    lcd.setCursor(0, 1);
    lcd.print("Dist: ");
    lcd.print(distance, 1);
    lcd.print("cm   ");
   
    tone(buzzerPin, NOTE_E4, 100);
    delay(700);
   
    Serial.print("Distance measurement ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(distance);
    Serial.println(" cm");
  }
 
  // Test button
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press Button");
  lcd.setCursor(0, 1);
  lcd.print("To Continue...");
 
  while (digitalRead(buttonPin) == HIGH) {
    delay(50);
  }
 
  tone(buzzerPin, NOTE_A4, 200);
  delay(300);
 
  // Test servo
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Testing Servo");
  testServoQuick();
 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("All Tests OK!");
  lcd.setCursor(0, 1);
  lcd.print("System Ready");
 
  tone(buzzerPin, NOTE_C5, 500);
  delay(2000);
 
  Serial.println("System check completed successfully!");
}

void calibrateSensor() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Calibration");
  lcd.setCursor(0, 1);
  lcd.print("Stand Still...");
 
  Serial.println("Sensor calibration - stand in normal position...");
 
  // Multiple measurements for average calculation
  float totalDistance = 0;
  int validMeasurements = 0;
 
  for (int i = 0; i < 8; i++) {
    float distance = measureDistance();
   
    if (distance > 10 && distance < 400) {  // Valid range
      totalDistance += distance;
      validMeasurements++;
    }
   
    lcd.setCursor(12, 1);
    lcd.print(i + 1);
    delay(300);
  }
 
  if (validMeasurements > 0) {
    baselineDistance = totalDistance / validMeasurements;
   
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Calibrated!");
    lcd.setCursor(0, 1);
    lcd.print("Base: ");
    lcd.print(baselineDistance, 1);
    lcd.print("cm");
   
    Serial.print("Calibration completed - baseline distance: ");
    Serial.print(baselineDistance);
    Serial.println(" cm");
   
    tone(buzzerPin, NOTE_G4, 300);
    delay(1500);
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Calibration");
    lcd.setCursor(0, 1);
    lcd.print("Failed!");
   
    baselineDistance = 50;  // Default closer value for proximity detection
    delay(1500);
  }
}

float measureDistance() {
  // Send trigger signal
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Read echo time
  long duration = pulseIn(echoPin, HIGH, 25000);  // 25ms timeout
 
  // Calculate distance (sound speed = 343 m/s)
  float distance = (duration * 0.034) / 2;
 
  // Filter invalid values
  if (distance < 2 || distance > 400) {
    return currentDistance;  // Return previous value if measurement is invalid
  }
 
  currentDistance = distance;
  return distance;
}

bool checkProximityRep() {
  float distance = measureDistance();
  bool currentlyClose = (distance >= 5.0 && distance <= 25.0);
  bool repDetected = false;
 
  // Rep counted when transitioning from not close to close
  if (currentlyClose && !wasClose && (millis() - lastRepTime > repCooldown)) {
    repDetected = true;
    repCount++;
    lastRepTime = millis();
   
    Serial.print("Rep detected! Total: ");
    Serial.println(repCount);
   
    // Visual and audio feedback
    tone(buzzerPin, NOTE_A4, 200);
  }
 
  wasClose = currentlyClose;
  return repDetected;
}

bool checkProximityRep2() {
  float distance = measureDistance();
  bool currentlyClose = (distance <= 5.0);
  bool repDetected = false;
 
  // Rep counted when transitioning from not close to close
  if (currentlyClose && !wasClose && (millis() - lastRepTime > repCooldown)) {
    repDetected = true;
    repCount++;
    lastRepTime = millis();
   
    Serial.print("Rep detected! Total: ");
    Serial.println(repCount);
   
    // Visual and audio feedback
    tone(buzzerPin, NOTE_A4, 200);
  }
 
  wasClose = currentlyClose;
  return repDetected;
}

void showMainMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ready to Train!");  // Better message
  lcd.setCursor(0, 1);
  lcd.print(workoutMenu[menuSelection]);
 
  Serial.println("Smart Training Menu:");
  for (int i = 0; i < menuSize; i++) {
    Serial.println(workoutMenu[i]);
  }
  Serial.println("Press button to select");
}

void updateMainMenuDisplay() {
  // Show that system is ready instead of distance
  lcd.setCursor(12, 0);
}

void handleButtonPress() {
  tone(buzzerPin, NOTE_E4, 100);
 
  if (waitingForStart) {
    // After workout complete, start new workout
    waitingForStart = false;
    menuSelection = (menuSelection + 1) % menuSize;
    showMainMenu();
    Serial.println("Ready for new workout selection");
   
  } else if (!isTraining) {
    // Start workout
    startSelectedWorkout();
   
  } else if (isTraining) {
    // Stop workout immediately
    stopWorkout();
  }
}

void startSelectedWorkout() {
  isTraining = true;
  systemReady = false;
  repCount = 0;
  wasClose = false;
 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Starting...");
  lcd.setCursor(0, 1);
  lcd.print(workoutMenu[menuSelection]);
 
  playWorkoutStartSound();
  delay(1500);
 
  Serial.print("Starting workout: ");
  Serial.println(workoutMenu[menuSelection]);
 
  switch (menuSelection) {
    case 0:
      countTouchesWorkout();
      break;
    case 1:
      timedTouchesWorkout();
      break;
    case 2:
      holdDurationWorkout();
      break;
  }
 
  if (isTraining) {  // Only if not stopped
    workoutComplete();
  }
}

void stopWorkout() {
  isTraining = false;
  systemReady = true;
 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Workout Stopped");
  lcd.setCursor(0, 1);
  lcd.print("Press for Menu");
 
  // Return servo to rest position
  smoothMove(currentAngle, 90, 1000);
 
  tone(buzzerPin, NOTE_D4, 500);
  delay(2000);
 
  // Go to next workout in menu
  menuSelection = (menuSelection + 1) % menuSize;
  showMainMenu();
 
  Serial.println("Workout stopped by user");
}

void countTouchesWorkout() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("**Do 10 push-ups");
  lcd.setCursor(0, 1);
  lcd.print("Target: 10");
 
  Serial.println("**Do 10 push-ups");
 
  int targetTouches = 10;
  repCount = 0;
 
  while (repCount < targetTouches && isTraining) {
    // Check for proximity touch
    checkProximityRep();
   
    // Update display
    lcd.setCursor(0, 0);
    lcd.print("Touches: ");
    lcd.print(repCount);
    lcd.print("/");
    lcd.print(targetTouches);
   
    // Show current distance
    float distance = measureDistance();
    lcd.setCursor(0, 1);
    lcd.print("Dist: ");
    lcd.print(distance, 1);
    lcd.print("cm   ");
   
    // Show hit indicator but NO servo movement during exercise
    if (distance <= 5.0) {
      lcd.setCursor(12, 1);
      lcd.print("HIT!");
    } else {
      lcd.setCursor(12, 1);
      lcd.print("    ");
    }
   
    delay(100);
  }
 
  if (isTraining) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Exercise Done!");
    lcd.setCursor(0, 1);
    lcd.print("Touches: ");
    lcd.print(repCount);
   
    playSuccessSound();
    delay(2000);
   
    // Rest period
    restPeriod(8);
  }
}

void timedTouchesWorkout() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Timed Touches");
  lcd.setCursor(0, 1);
  lcd.print("15 Seconds");
 
  Serial.println("Timed touches workout - count touches in 15 seconds");
 
  unsigned long startTime = millis();
  int exerciseTime = 15000; // 15 seconds
  repCount = 0;
 
  while ((millis() - startTime < exerciseTime) && isTraining) {
  // Check for proximity touch
  checkProximityRep2();
 
  // Update display
  int timeLeft = (exerciseTime - (millis() - startTime)) / 1000;
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  lcd.print(timeLeft);
  lcd.print("s T:");
 
  // נקה והדפס את repCount
  lcd.print("   ");  // ניקוי מקום למספר
  lcd.setCursor(10, 0);
  lcd.print(repCount);
 
  // Show current distance
  float distance = measureDistance();
  lcd.setCursor(0, 1);
  lcd.print("Dist: ");
  lcd.print(distance, 1);
  lcd.print("cm   ");
 
  // Show hit indicator
  if (distance <= 5.0) {
    lcd.setCursor(12, 1);
    lcd.print("HIT!");
  } else {
    lcd.setCursor(12, 1);
    lcd.print("    ");
  }
 
  delay(100);
}
 
  if (isTraining) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Time Complete!");
    lcd.setCursor(0, 1);
    lcd.print("Total: ");
    lcd.print(repCount);
   
    playSuccessSound();
    delay(2000);
   
    // Rest period
    restPeriod(8);
  }
}

void holdDurationWorkout() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hold Duration");
  lcd.setCursor(0, 1);
  lcd.print("Stay Close!");
 
  Serial.println("Do 30s plank");
 
  unsigned long totalHoldTime = 0;
  unsigned long holdStartTime = 0;
  bool currentlyHolding = false;
  unsigned long exerciseStartTime = millis();
  int exerciseTime = 30000; // 30 seconds total exercise time
 
  while ((millis() - exerciseStartTime < exerciseTime) && isTraining) {
    float distance = measureDistance();
    bool closeNow = (distance >= 5.0 && distance <= 25.0);
   
    // Track continuous hold time
    if (closeNow && !currentlyHolding) {
      // Started holding
      currentlyHolding = true;
      holdStartTime = millis();
    } else if (!closeNow && currentlyHolding) {
      // Stopped holding
      currentlyHolding = false;
      totalHoldTime += (millis() - holdStartTime);
    }
   
    // Calculate current session time if still holding
    unsigned long currentSessionTime = 0;
    if (currentlyHolding) {
      currentSessionTime = millis() - holdStartTime;
    }
   
    // Update display
    int timeLeft = (exerciseTime - (millis() - exerciseStartTime)) / 1000;
    lcd.setCursor(0, 0);
    lcd.print("Time: ");
    lcd.print(timeLeft);
    lcd.print("s       ");
   
    // Show total hold time + current session
    unsigned long displayTime = totalHoldTime + currentSessionTime;
    lcd.setCursor(0, 1);
    lcd.print("Hold: ");
    lcd.print(displayTime / 1000);
    lcd.print(".");
    lcd.print((displayTime % 1000) / 100);
    lcd.print("s");
   
    // Show current status
    if (closeNow) {
      lcd.setCursor(12, 1);
      lcd.print("HOLD");
      // Optional: soft beep while holding
      if (millis() % 1000 < 50) {
        tone(buzzerPin, NOTE_F4, 50);
      }
    } else {
      lcd.setCursor(12, 1);
      lcd.print("    ");
    }
   
    delay(100);
  }
 
  // Add final hold time if still holding at end
  if (currentlyHolding) {
    totalHoldTime += (millis() - holdStartTime);
  }
 
  if (isTraining) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Exercise Done!");
    lcd.setCursor(0, 1);
    lcd.print("Hold: ");
    lcd.print(totalHoldTime / 1000);
    lcd.print(".");
    lcd.print((totalHoldTime % 1000) / 100);
    lcd.print("s");
   
    Serial.print("Total hold time: ");
    Serial.print(totalHoldTime / 1000);
    Serial.print(".");
    Serial.print((totalHoldTime % 1000) / 100);
    Serial.println(" seconds");
   
    playSuccessSound();
    delay(3000);
   
    // Rest period
    restPeriod(10);
  }
}

void restPeriod(int seconds) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Rest Time");
  lcd.setCursor(0, 1);
  lcd.print("Relax...");
 
  Serial.println("Rest period - relax and breathe");
 
  // Normal gentle servo movement during rest (not fast)
  unsigned long restStartTime = millis();
  unsigned long restDuration = seconds * 1000; // Convert to milliseconds
 
  while ((millis() - restStartTime < restDuration) && isTraining) {
    // Gentle breathing motion - slow movement
    for (int cycle = 0; cycle < 3 && isTraining; cycle++) {
      // Slow gentle movement - like breathing
      smoothMove(currentAngle, 150, 200);  // Slow movement to 100 degrees
      if (!isTraining) break;
      smoothMove(150, 30, 300);   // Slow movement to 80 degrees  
      if (!isTraining) break;
      smoothMove(30, 90, 300);    // Return to center
      if (!isTraining) break;
    }
   
    // Update remaining time
    int timeLeft = (restDuration - (millis() - restStartTime)) / 1000;
    lcd.setCursor(0, 1);
    lcd.print("Rest: ");
    lcd.print(timeLeft);
    lcd.print("s   ");
   
    // Occasional soft relaxing sound
    if (timeLeft % 4 == 0 && (millis() % 4000 < 50)) {
      tone(buzzerPin, NOTE_F4, 200);
    }
  }
 
  // Return to rest position
  if (isTraining) {
    smoothMove(currentAngle, 90, 800);
   
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Rest Complete");
    lcd.setCursor(0, 1);
    lcd.print("Ready to Go!");
   
    tone(buzzerPin, NOTE_A4, 300);
    delay(1500);
  }
}

void testServoQuick() {
  for (int angle = 90; angle <= 110; angle += 10) {
    trainingServo.write(angle);
    delay(100);
  }
  for (int angle = 110; angle >= 70; angle -= 10) {
    trainingServo.write(angle);
    delay(100);
  }
  for (int angle = 70; angle <= 90; angle += 10) {
    trainingServo.write(angle);
    delay(100);
  }
  currentAngle = 90;
}

void workoutComplete() {
  isTraining = false;
  systemReady = true;
  waitingForStart = true;
 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Workout Done!");
  lcd.setCursor(0, 1);
  lcd.print("Press for New");
 
  smoothMove(currentAngle, 90, 1000);
  playSuccessSound();
 
  Serial.println("Workout completed! Press button for new workout.");
}

void smoothMove(int fromAngle, int toAngle, int duration) {
  int steps = abs(toAngle - fromAngle);
  if (steps == 0) return;
 
  int stepDelay = duration / steps;
  stepDelay = max(stepDelay, 10);  // Minimum delay to prevent overload
 
  if (fromAngle < toAngle) {
    for (int angle = fromAngle; angle <= toAngle; angle++) {
      if (!isTraining) return;  // Stop movement if training stopped
      trainingServo.write(angle);
      currentAngle = angle;
      delay(stepDelay);
    }
  } else {
    for (int angle = fromAngle; angle >= toAngle; angle--) {
      if (!isTraining) return;  // Stop movement if training stopped
      trainingServo.write(angle);
      currentAngle = angle;
      delay(stepDelay);
    }
  }
}

void playWorkoutStartSound() {
  int startMelody[] = {NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5};
  int startDurations[] = {200, 200, 200, 400};
 
  for (int i = 0; i < 4; i++) {
    tone(buzzerPin, startMelody[i], startDurations[i]);
    delay(startDurations[i] + 50);
  }
}

void playSuccessSound() {
  int successMelody[] = {NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5, NOTE_G4, NOTE_C5};
  int successDurations[] = {150, 150, 150, 300, 150, 400};
 
  for (int i = 0; i < 6; i++) {
    tone(buzzerPin, successMelody[i], successDurations[i]);
    delay(successDurations[i] + 50);
  }
}