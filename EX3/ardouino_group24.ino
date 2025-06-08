// Include libraries
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <IRremote.h>

// Pin definitions
#define BUTTON_PIN 2
#define IR_RECEIVE_PIN 4
#define TRIG_PIN 7
#define ECHO_PIN 8
#define SERVO_PIN 9
#define BUZZER_PIN 10  // Currently disconnected but kept in code
#define GREEN_LED_PIN 11
#define RED_LED_PIN 12

// IR remote button codes
#define IR_BUTTON_1 0xC
#define IR_BUTTON_2 0x18
#define IR_BUTTON_OK 0xD

// Special LCD characters
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

byte checkChar[8] = {
  0b00000,
  0b00001,
  0b00011,
  0b10110,
  0b11100,
  0b01000,
  0b00000,
  0b00000
};

byte muscleChar[8] = {
  0b00000,
  0b01100,
  0b11110,
  0b11110,
  0b01100,
  0b01100,
  0b01100,
  0b00000
};

// Initialize objects
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servoMotor;

// Button state variables
int buttonState = HIGH;
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

// Ultrasonic sensor variables
long duration;
int distance;
int lastDistance = 0;
bool repDetected = false;

// Distance thresholds for different exercises
int squatThreshold = 30;    // Distance in cm for squat detection
int pushupThreshold = 20;   // Distance in cm for pushup detection
int activeThreshold;        // Current active threshold

// Servo variables (for continuous rotation servo)
const int SERVO_STOP = 90;       // Value to stop the servo
const int SERVO_SPEED = 70;      // Speed for rotation (value between 0-89 for one direction, 91-180 for the other)
unsigned long servoStartTime = 0; // When servo started moving
unsigned long servoMoveTime = 0;  // How long to move in ms
bool servoActive = false;         // Is servo currently active

// LED control variables
unsigned long lastLedToggleTime = 0;

// Training mode variables
enum WorkoutMode {
  WELCOME,
  WORKOUT_SELECT,
  PHYSICAL_SELECT,
  COUNTDOWN,
  PHYSICAL_WORKOUT,
  PHYSICAL_REST,
  TABATA_WORKOUT,
  RESULT,
  CELEBRATION
};

WorkoutMode currentMode = WELCOME;
bool isTabata = false;
bool isSquat = false;
unsigned long modeStartTime = 0;
int repCount = 0;
int setCount = 0;
int bestRepCount = 0;  // Track personal best

// Tabata specific variables
unsigned long tabataStartTime = 0;
int tabataRound = 0;
int lastTabataRound = -1;
bool isTabataWork = true;
const int TABATA_WORK_TIME = 20;    // 20 seconds work
const int TABATA_REST_TIME = 10;    // 10 seconds rest
const int TABATA_TOTAL_ROUNDS = 3;  // 3 rounds total

// Rest period for physical workout
const int REST_PERIOD = 5;          // 5 seconds rest between sets
unsigned long restStartTime = 0;

// System flags
bool soundEnabled = false;  // Set to false since buzzer is disconnected
bool welcomeShown = false;
bool resultServoMoved = false;
bool newPersonalBest = false;

// Motivational messages for various stages
const char* startMessages[] = {
  "Let's crush it!",
  "You got this!",
  "Time to shine!",
  "Beast mode ON"
};

const char* endMessages[] = {
  "Awesome work!",
  "Crushed it!",
  "You're a star!",
  "Feel the burn!"
};

const char* motivationalQuotes[] = {
  "Never give up!",
  "One more rep!",
  "Stay strong!",
  "Push yourself!"
};

// LCD update timing
unsigned long lastLCDUpdate = 0;
const unsigned long LCD_UPDATE_INTERVAL = 1000;  // Update LCD every second to reduce flicker

// Animation variables
unsigned long lastAnimTime = 0;
int animFrame = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  Serial.println("Starting Smart Workout System v2.0");
  
  // Initialize pins
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  
  // Initialize IR receiver
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("IR Receiver enabled");
  
  // Initialize servo and ensure it's stopped
  servoMotor.attach(SERVO_PIN);
  servoMotor.write(SERVO_STOP);
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  
  // Create custom characters
  lcd.createChar(0, heartChar);
  lcd.createChar(1, checkChar);
  lcd.createChar(2, muscleChar);
  
  // Set initial mode
  changeMode(WELCOME);
  
  // Initial LED test
  digitalWrite(GREEN_LED_PIN, HIGH);
  digitalWrite(RED_LED_PIN, HIGH);
  delay(500);
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, HIGH);
  delay(500);
  digitalWrite(GREEN_LED_PIN, HIGH);
  digitalWrite(RED_LED_PIN, LOW);
  delay(500);
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
}

void loop() {
  // Check for IR remote commands
  checkIRCommands();
  
  // Check physical button
  checkButton();
  
  // Measure distance with ultrasonic sensor
  measureDistance();
  
  // Main state machine for workout modes
  updateWorkoutMode();
  
  // Update servo if needed
  updateServo();
  
  // Short delay for stability
  delay(50);
}

// Check for IR remote commands
void checkIRCommands() {
  if (IrReceiver.decode()) {
    uint16_t command = IrReceiver.decodedIRData.command;
    
    Serial.print("IR Command received: 0x");
    Serial.println(command, HEX);
    
    // Process commands based on current mode
    switch (currentMode) {
      case WELCOME:
        if (command == IR_BUTTON_1 || command == IR_BUTTON_2) {
          changeMode(WORKOUT_SELECT);
        }
        break;
        
      case WORKOUT_SELECT:
        if (command == IR_BUTTON_1) {
          isTabata = false;
          changeMode(PHYSICAL_SELECT);
        } else if (command == IR_BUTTON_2) {
          isTabata = true;
          changeMode(COUNTDOWN);
        }
        break;
        
      case PHYSICAL_SELECT:
        if (command == IR_BUTTON_1) {
          isSquat = true;
          activeThreshold = squatThreshold;
          changeMode(COUNTDOWN);
        } else if (command == IR_BUTTON_2) {
          isSquat = false;
          activeThreshold = pushupThreshold;
          changeMode(COUNTDOWN);
        }
        break;
        
      case COUNTDOWN:
        // No IR commands processed during countdown
        break;
        
      case PHYSICAL_WORKOUT:
      case TABATA_WORKOUT:
        if (command == IR_BUTTON_OK) {
          // Emergency stop workout
          if (currentMode == PHYSICAL_WORKOUT) {
            // End the physical workout set
            setCount++; // Count this as a completed set
            changeMode(CELEBRATION);
          } else {
            changeMode(CELEBRATION);
          }
        }
        break;
        
      case PHYSICAL_REST:
        if (command == IR_BUTTON_1) {
          // Skip rest period
          changeMode(PHYSICAL_WORKOUT);
        } else if (command == IR_BUTTON_OK) {
          // End workout
          changeMode(CELEBRATION);
        }
        break;
        
      case RESULT:
      case CELEBRATION:
        if (command == IR_BUTTON_1) {
          // Reset everything and go back to welcome
          resetWorkout();
          changeMode(WELCOME);
        }
        break;
    }
    
    IrReceiver.resume(); // Receive the next value
  }
}

// Check physical button state
void checkButton() {
  int reading = digitalRead(BUTTON_PIN);
  
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      
      if (buttonState == LOW) {
        // Button is pressed
        Serial.println("Physical button pressed");
        
        // Handle button press based on current mode
        if (currentMode == PHYSICAL_WORKOUT) {
          // Complete current set and go to rest or celebration
          setCount++;
          Serial.print("Set completed! Set count: ");
          Serial.println(setCount);
          
          if (setCount >= 3) {
            // If 3+ sets completed, show celebration
            changeMode(CELEBRATION);
          } else {
            // Otherwise, take a rest
            changeMode(PHYSICAL_REST);
          }
        } else if (currentMode == PHYSICAL_REST) {
          // Skip rest period
          changeMode(PHYSICAL_WORKOUT);
        } else if (currentMode == RESULT || currentMode == CELEBRATION) {
          // Reset and start new workout
          resetWorkout();
          changeMode(WELCOME);
        }
      }
    }
  }
  
  lastButtonState = reading;
}

// Measure distance with ultrasonic sensor
void measureDistance() {
  // Clear the TRIG pin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  // Set the TRIG pin HIGH for 10 microseconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Read the ECHO pin, calculate the distance
  duration = pulseIn(ECHO_PIN, HIGH);
  
  // Sound travels at 343 meters per second, so 0.0343 cm/µs
  // Distance = (Time × Speed) / 2 (go and return)
  int newDistance = duration * 0.0343 / 2;
  
  // Apply simple smoothing for distance readings
  if (abs(newDistance - distance) < 50 && newDistance < 300) {
    distance = newDistance;
  }
}

// Detect repetitions based on distance threshold
void detectRepetition() {
  // Only detect reps during workout modes
  if (currentMode != PHYSICAL_WORKOUT && currentMode != TABATA_WORKOUT) return;
  
  static bool wasUnderThreshold = false;
  
  // Check if user has gone below threshold
  if (distance < activeThreshold && !wasUnderThreshold) {
    wasUnderThreshold = true;
    digitalWrite(GREEN_LED_PIN, HIGH);
    Serial.println("Movement started");
  }
  // Check if user has gone back above threshold
  else if (distance > activeThreshold + 5 && wasUnderThreshold) {
    wasUnderThreshold = false;
    repCount++;
    
    // Play sound for rep completed
    if (soundEnabled) {
      tone(BUZZER_PIN, 880, 100);
    }
    
    Serial.print("Repetition detected! Count: ");
    Serial.println(repCount);
    
    // Check if new personal best
    if (repCount > bestRepCount) {
      bestRepCount = repCount;
      newPersonalBest = true;
    }
    
    // Turn off LED briefly for visual feedback
    digitalWrite(GREEN_LED_PIN, LOW);
    delay(100);
    
    // If in tabata workout, don't turn LED back on during rest
    if (currentMode == PHYSICAL_WORKOUT || (currentMode == TABATA_WORKOUT && isTabataWork)) {
      digitalWrite(GREEN_LED_PIN, HIGH);
    }
  }
}

// Start servo movement for a specific time
void startServoMove(int direction, unsigned long moveTimeMs) {
  // Only start if not already active
  if (!servoActive) {
    Serial.print("Starting servo movement, direction: ");
    Serial.print(direction);
    Serial.print(", time: ");
    Serial.println(moveTimeMs);
    
    servoActive = true;
    servoStartTime = millis();
    servoMoveTime = moveTimeMs;
    
    // Set servo speed based on direction
    if (direction > 0) {
      servoMotor.write(180 - SERVO_SPEED); // Clockwise rotation
    } else {
      servoMotor.write(SERVO_SPEED);       // Counter-clockwise rotation
    }
  }
}

// Update servo state
void updateServo() {
  // If servo is active, check if it's time to stop
  if (servoActive) {
    if (millis() - servoStartTime >= servoMoveTime) {
      // Time to stop the servo
      servoMotor.write(SERVO_STOP);
      servoActive = false;
      Serial.println("Servo movement stopped");
    }
  }
}

// Move servo for tabata round completion
void moveServoForTabataRound() {
  // Only move for new rounds
  if (tabataRound > lastTabataRound) {
    lastTabataRound = tabataRound;
    Serial.print("Moving servo for tabata round: ");
    Serial.println(tabataRound);
    
    // Move for about 1 second - enough time for 1/3 rotation
    startServoMove(1, 1000);
  }
}

// Move servo for physical workout completion
void moveServoForPhysicalCompletion() {
  if (!resultServoMoved) {
    resultServoMoved = true;
    Serial.println("Moving servo for physical workout completion");
    
    // Move for about 3 seconds - enough for full rotation
    startServoMove(1, 3000);
  }
}

// Display animated loading bar
void showLoadingAnimation(int progress) {
  // progress should be 0-100
  int barLength = map(progress, 0, 100, 0, 16);
  
  lcd.setCursor(0, 1);
  lcd.print("[");
  
  for (int i = 0; i < 14; i++) {
    if (i < barLength) {
      lcd.print("=");
    } else {
      lcd.print(" ");
    }
  }
  
  lcd.print("]");
}

// Update workout mode state machine
void updateWorkoutMode() {
  unsigned long currentTime = millis();
  
  // Update LCD only at intervals to prevent flicker
  bool updateLCD = (currentTime - lastLCDUpdate > LCD_UPDATE_INTERVAL);
  if (updateLCD) {
    lastLCDUpdate = currentTime;
  }
  
  // Update animation frames
  bool updateAnim = (currentTime - lastAnimTime > 300);
  if (updateAnim) {
    animFrame = (animFrame + 1) % 4;
    lastAnimTime = currentTime;
  }
  
  switch (currentMode) {
    case WELCOME:
      if (!welcomeShown) {
        showWelcomeScreen();
        welcomeShown = true;
      }
      
      if (updateAnim) {
        // Animate the heart symbol
        lcd.setCursor(15, 0);
        if (animFrame % 2 == 0) {
          lcd.write(byte(0)); // Heart
        } else {
          lcd.print(" ");
        }
      }
      
      // Auto advance after 10 seconds
      if (currentTime - modeStartTime > 10000) {
        changeMode(WORKOUT_SELECT);
      }
      break;
    
    case WORKOUT_SELECT:
      if (updateLCD) {
        showWorkoutSelectScreen();
      }
      
      if (updateAnim) {
        // Animate the workout icons
        lcd.setCursor(15, 0);
        lcd.write(byte(animFrame % 2 == 0 ? 2 : 0)); // Alternate muscle and heart
      }
      break;
      
    case PHYSICAL_SELECT:
      if (updateLCD) {
        showPhysicalSelectScreen();
      }
      
      if (updateAnim) {
        // Animate the exercise type
        lcd.setCursor(15, 1);
        lcd.write(byte(animFrame % 2 == 0 ? 2 : 1)); // Alternate muscle and check
      }
      break;
      
    case COUNTDOWN:
      // Run 5 second countdown, then start workout
      if (updateLCD || updateAnim) {
        int secondsLeft = 5 - ((currentTime - modeStartTime) / 1000);
        
        // Handle the transition when countdown reaches 0
        if (secondsLeft <= 0) {
          if (isTabata) {
            changeMode(TABATA_WORKOUT);
          } else {
            changeMode(PHYSICAL_WORKOUT);
          }
        } else {
          // Update countdown display with animation
          lcd.clear();
          lcd.setCursor(0, 0);
          
          // Get a random motivational message
          int msgIndex = random(0, 4);
          lcd.print(startMessages[msgIndex]);
          
          // Display countdown in center of second line
          lcd.setCursor(7, 1);
          lcd.print(secondsLeft);
          
          // Show animated loading bar
          int progress = (5 - secondsLeft) * 20; // 0-100 range
          showLoadingAnimation(progress);
        }
      }
      break;
      
    case PHYSICAL_WORKOUT:
      if (updateLCD) {
        // Update display for physical workout
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(isSquat ? "Squat " : "Pushup");
        lcd.write(byte(2)); // Muscle icon
        lcd.print(" Set:");
        lcd.print(setCount + 1);
        
        lcd.setCursor(0, 1);
        lcd.print("Reps: ");
        lcd.print(repCount);
        
        // Show a motivational message that changes
        if (repCount > 0 && repCount % 5 == 0) {
          lcd.setCursor(8, 1);
          lcd.print(motivationalQuotes[repCount / 5 % 4]);
        }
      }
      
      // Check for repetitions
      detectRepetition();
      
      // Green light stays on during workout
      digitalWrite(GREEN_LED_PIN, HIGH);
      digitalWrite(RED_LED_PIN, LOW);
      break;
      
    case PHYSICAL_REST:
      {
        int secondsLeft = REST_PERIOD - ((currentTime - restStartTime) / 1000);
        
        if (secondsLeft <= 0) {
          // Rest period over, back to workout
          changeMode(PHYSICAL_WORKOUT);
        }
        
        if (updateLCD) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Rest ");
          lcd.write(byte(0)); // Heart
          lcd.print(" ");
          lcd.print(secondsLeft);
          lcd.print("s left");
          
          lcd.setCursor(0, 1);
          lcd.print("Next set: ");
          lcd.print(setCount + 1);
          lcd.print("/3");
        }
        
        // Blink red LED during rest
        if (currentTime - lastLedToggleTime > 500) {
          digitalWrite(RED_LED_PIN, !digitalRead(RED_LED_PIN));
          lastLedToggleTime = currentTime;
        }
      }
      break;
      
    case TABATA_WORKOUT:
      // Calculate current tabata phase
      unsigned long elapsedTabataTime = currentTime - tabataStartTime;
      int totalCycleTime = (TABATA_WORK_TIME + TABATA_REST_TIME) * 1000;
      int currentCycleTime = elapsedTabataTime % totalCycleTime;
      bool newTabataWork = currentCycleTime < (TABATA_WORK_TIME * 1000);
      
      // Calculate current round (0-based)
      int newTabataRound = elapsedTabataTime / totalCycleTime;
      
      // Check if we've switched work/rest phase
      if (newTabataWork != isTabataWork) {
        isTabataWork = newTabataWork;
        if (!isTabataWork) {
          // Switched from work to rest
          digitalWrite(GREEN_LED_PIN, LOW);
          digitalWrite(RED_LED_PIN, HIGH);
          if (soundEnabled) {
            tone(BUZZER_PIN, 440, 300);
          }
        } else {
          // Switched from rest to work
          digitalWrite(GREEN_LED_PIN, HIGH);
          digitalWrite(RED_LED_PIN, LOW);
          if (soundEnabled) {
            tone(BUZZER_PIN, 880, 300);
          }
        }
      }
      
      // Check if we've completed all rounds
      if (newTabataRound >= TABATA_TOTAL_ROUNDS) {
        changeMode(CELEBRATION);
      } else if (newTabataRound != tabataRound) {
        // Update round and move servo on round change
        tabataRound = newTabataRound;
        moveServoForTabataRound();
      }
      
      if (updateLCD) {
        // Update display for tabata workout
        lcd.clear();
        lcd.setCursor(0, 0);
        if (isTabataWork) {
          lcd.print("WORK! ");
          lcd.write(byte(2)); // Muscle icon
        } else {
          lcd.print("REST ");
          lcd.write(byte(0)); // Heart icon
        }
        lcd.print(" Rd:");
        lcd.print(tabataRound + 1);
        lcd.print("/");
        lcd.print(TABATA_TOTAL_ROUNDS);
        
        // Display countdown timer
        int secondsRemaining;
        if (isTabataWork) {
          secondsRemaining = TABATA_WORK_TIME - (currentCycleTime / 1000);
        } else {
          secondsRemaining = TABATA_REST_TIME - ((currentCycleTime - (TABATA_WORK_TIME * 1000)) / 1000);
        }
        
        lcd.setCursor(0, 1);
        lcd.print("Time: ");
        lcd.print(secondsRemaining);
        lcd.print("s");
        lcd.setCursor(10, 1);
        lcd.print("Reps:");
        lcd.print(repCount);
      }
      
      // Check for repetitions during work phase
      if (isTabataWork) {
        detectRepetition();
      }
      break;
      
    case CELEBRATION:
      if (updateLCD || updateAnim) {
        // Show animated celebration screen
        lcd.clear();
        lcd.setCursor(0, 0);
        
        // Get a random end message
        int msgIndex = random(0, 4);
        lcd.print(endMessages[msgIndex]);
        lcd.write(byte(1)); // Check mark
        
        // Animate the second line
        lcd.setCursor(0, 1);
        switch (animFrame) {
          case 0:
            lcd.print("* You Did It! *");
            break;
          case 1:
            lcd.print("\\o/ AMAZING \\o/");
            break;
          case 2:
            lcd.print("** AWESOME! **");
            break;
          case 3:
            lcd.print("> WELL DONE! <");
            break;
        }
      }
      
      // Blink both LEDs for celebration
      if (currentTime - lastLedToggleTime > 300) {
        digitalWrite(GREEN_LED_PIN, !digitalRead(GREEN_LED_PIN));
        digitalWrite(RED_LED_PIN, !digitalRead(RED_LED_PIN));
        lastLedToggleTime = currentTime;
      }
      
      // Move servo as reward if not already moved
      if (!resultServoMoved) {
        moveServoForPhysicalCompletion();
      }
      
      // After 5 seconds, go to result
      if (currentTime - modeStartTime > 5000) {
        changeMode(RESULT);
      }
      break;
      
    case RESULT:
      if (updateLCD) {
        // Show workout results
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Total ");
        lcd.write(byte(2)); // Muscle
        lcd.print(" ");
        lcd.print(repCount);
        lcd.print(" reps");
        
        lcd.setCursor(0, 1);
        if (newPersonalBest) {
          lcd.print("NEW RECORD! ");
          lcd.write(byte(0)); // Heart
        } else {
          lcd.print("Sets: ");
          lcd.print(setCount);
          lcd.print(" Press 1=New");
        }
      }
      
      // Blink green LED if new record
      if (newPersonalBest && currentTime - lastLedToggleTime > 500) {
        digitalWrite(GREEN_LED_PIN, !digitalRead(GREEN_LED_PIN));
        digitalWrite(RED_LED_PIN, LOW);
        lastLedToggleTime = currentTime;
      }
      break;
  }
}

// Change the workout mode
void changeMode(WorkoutMode newMode) {
  WorkoutMode oldMode = currentMode;
  currentMode = newMode;
  modeStartTime = millis();
  
  // Reset LEDs when changing modes
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  
  // Stop servo in any mode change
  servoMotor.write(SERVO_STOP);
  servoActive = false;
  
  // Reset LCD update timer to force immediate update
  lastLCDUpdate = 0;
  
  // Handle mode-specific initialization
  switch (newMode) {
    case WELCOME:
      // Play welcome sound
      if (soundEnabled) {
        tone(BUZZER_PIN, 880, 100);
        delay(150);
        tone(BUZZER_PIN, 988, 100);
        delay(150);
        tone(BUZZER_PIN, 1047, 150);
      }
      break;
      
    case TABATA_WORKOUT:
      tabataStartTime = millis();
      tabataRound = 0;
      lastTabataRound = -1;  // Reset to ensure first round triggers servo
      isTabataWork = true;
      digitalWrite(GREEN_LED_PIN, HIGH);  // Start with green LED on for work phase
      break;
      
    case PHYSICAL_WORKOUT:
      // If coming from rest, don't reset the rep count
      if (oldMode != PHYSICAL_REST) {
        repCount = 0; // Reset rep count for new set
      }
      break;
      
    case PHYSICAL_REST:
      restStartTime = millis();
      break;
      
    case CELEBRATION:
      // Play victory tune
      if (soundEnabled) {
        for (int i = 0; i < 3; i++) {
          tone(BUZZER_PIN, 1047 + i*200, 150);
          delay(200);
        }
      }
      // Reset the flag for servo motion
      resultServoMoved = false;
      break;
      
    case RESULT:
      // If not coming from celebration, ensure servo moves
      if (oldMode != CELEBRATION) {
        resultServoMoved = false;
      }
      break;
  }
  
  Serial.print("Mode changed from ");
  Serial.print(oldMode);
  Serial.print(" to ");
  Serial.println(newMode);
}

// Reset all workout variables
void resetWorkout() {
  repCount = 0;
  setCount = 0;
  tabataRound = 0;
  lastTabataRound = -1;
  isTabataWork = true;
  welcomeShown = false;
  resultServoMoved = false;
  newPersonalBest = false;
  
  // Ensure servo is stopped
  servoMotor.write(SERVO_STOP);
  servoActive = false;
}

// Display welcome screen
void showWelcomeScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smart Workout");
  lcd.setCursor(0, 1);
  lcd.print("System v2.0 ");
  lcd.write(byte(0)); // Heart icon
  
  Serial.println("Welcome screen displayed");
}

// Display workout selection screen
void showWorkoutSelectScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Choose workout:");
  lcd.setCursor(0, 1);
  lcd.print("1:Gym  2:Tabata");
  
  Serial.println("Workout selection screen displayed");
}

// Display physical exercise selection screen
void showPhysicalSelectScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Choose exercise:");
  lcd.setCursor(0, 1);
  lcd.print("1:Squat  2:Pushup");
  
  Serial.println("Physical exercise selection screen displayed");
}