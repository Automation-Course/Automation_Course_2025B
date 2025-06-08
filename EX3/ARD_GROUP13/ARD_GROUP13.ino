
#include <LiquidCrystal_I2C.h>
#include <IRremote.hpp>
#include <Servo.h>

// pin allocation
#define IR_PIN 7
#define BUZZER_PIN 4
#define YELLOW_LED_PIN 10
#define BUTTON_PLAYER1_PIN 2
#define BUTTON_PLAYER2_PIN 3
#define SERVO_PIN 6

// remote buttons
#define BUTTON_1 0xBA45FF00
#define BUTTON_2 0xB946FF00  
#define BUTTON_3 0xB847FF00
#define BUTTON_4 0xBB44FF00
#define BUTTON_5 0xBF40FF00
#define BUTTON_6 0xBC43FF00
#define BUTTON_7 0xF807FF00
#define BUTTON_8 0xEA15FF00
#define BUTTON_9 0xF609FF00

// objects definition
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo gameServo;

// global variables
int selectedTime = 0;
bool timeSelected = false;
bool gameStarted = false;
bool gameActive = false;
bool servoSpin = true;

// game variables
unsigned long gameStartTime = 0;
unsigned long player1PressTime = 0;
unsigned long player2PressTime = 0;
bool player1Pressed = false;
bool player2Pressed = false;

// servo variables
int servoAngle = 90;
bool servoDirection = true;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting Game Setup...");
 
  // initialize components
  lcd.init();
  lcd.backlight();
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(BUTTON_PLAYER1_PIN, INPUT_PULLUP);
  pinMode(BUTTON_PLAYER2_PIN, INPUT_PULLUP);

  gameServo.attach(SERVO_PIN);
  gameServo.write(90);  // Center position (90 degrees)
  delay(500);  // Give the servo time to reach the position
 
  // Turn off LEDs at start
  digitalWrite(YELLOW_LED_PIN, LOW);
 
  // IR initialization
  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);
 
  // Display main screen
  showTimeSelectionScreen();
}

void loop() {
  if (!timeSelected) {
    // Still waiting for time selection
    handleIRInput();
  } else if (!gameStarted) {
    // Time selected, starting game
    startGame();
  } else if (gameActive) {
    // Game active - waiting for player presses
    handlePlayerButtons();
   
    moveServoRandomly();
  }
 
  delay(10); // Small delay
}

// Display time selection screen
void showTimeSelectionScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Please choose");
  lcd.setCursor(0, 1);
  lcd.print("game time (1-9)");
 
  Serial.println("Screen: Please choose game time (1-9)");
  Serial.println("Waiting for IR remote input...");
}

// Handle IR input
void handleIRInput() {
  if (IrReceiver.decode()) {
    if (IrReceiver.decodedIRData.decodedRawData != 0) {
      uint32_t rawData = IrReceiver.decodedIRData.decodedRawData;
     
      Serial.print("IR Raw Data received: 0x");
      Serial.println(rawData, HEX);
     
      // Convert IR command to number (1-9)
      int number = getNumberFromIR(rawData);
     
      if (number >= 1 && number <= 9) {
        // Valid number selected!
        selectedTime = number;
        timeSelected = true;
       
        Serial.print("Valid number selected: ");
        Serial.println(selectedTime);
       
        // Display confirmation
        showConfirmation();
       
      } else {
        // Invalid number
        Serial.println("Invalid number - please press 1-9");
      }
    }
   
    IrReceiver.resume(); // Ready for next command
  }
}

// Convert IR command to number (adapted to your remote)
int getNumberFromIR(uint32_t rawData) {
  switch(rawData) {
    case BUTTON_1: return 1;
    case BUTTON_2: return 2;
    case BUTTON_3: return 3;
    case BUTTON_4: return 4;
    case BUTTON_5: return 5;
    case BUTTON_6: return 6;
    case BUTTON_7: return 7;
    case BUTTON_8: return 8;
    case BUTTON_9: return 9;
    default:       return 0;
  }
}

// Display confirmation
void showConfirmation() {

  taylor();
 
  // Display on screen
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("You Chose");
  lcd.setCursor(0, 1);
  lcd.print(selectedTime);
  lcd.print(" Seconds");
 
  Serial.print("Confirmation: You chose ");
  Serial.print(selectedTime);
  Serial.println(" seconds");
 
  Serial.println("Time selection completed!");
 
  delay(2000); // Wait 2 seconds
}

// Start the game
void startGame() {
  Serial.println("Starting game sequence...");
 
  // Turn on yellow LED
  digitalWrite(YELLOW_LED_PIN, HIGH);
 
  // Display waiting message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wait for light");
  lcd.setCursor(0, 1);
  lcd.print("to turn off!");
 
  Serial.println("Yellow LED ON - Get ready phase");
 
  // Draw random wait time (1-5 seconds)
  int waitTime = random(1, 6);
  Serial.print("Random wait time: ");
  Serial.print(waitTime);
  Serial.println(" seconds");
 
  delay(waitTime * 1000); // Random wait
 
  // Turn off yellow LED
  digitalWrite(YELLOW_LED_PIN, LOW);
 
  // Game starts
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("GAME STARTED!");
  lcd.setCursor(0, 1);
  lcd.print("Count & Press!");
 
  gameStartTime = millis();
  gameActive = true;
  gameStarted = true;
 
  // Reset player variables
  player1Pressed = false;
  player2Pressed = false;
  player1PressTime = 0;
  player2PressTime = 0;
}

void moveServoRandomly() {
  static unsigned long lastServoMove = 0;
 
  // Move every 100ms
  if (millis() - lastServoMove > 50) {
    if (servoDirection) {
      servoAngle += 2;  // Move 2 degrees
      if (servoAngle >= 180) {
        servoDirection = false;
      }
    } else {
      servoAngle -= 2;  // Move 2 degrees back
      if (servoAngle <= 0) {
        servoDirection = true;
      }
    }
   
    gameServo.write(servoAngle);
    lastServoMove = millis();
  }
}

// Handle player button presses
void handlePlayerButtons() {
  unsigned long currentTime = millis();
 
  // Check player 1 button
  if (!player1Pressed && digitalRead(BUTTON_PLAYER1_PIN) == LOW) {
    player1PressTime = currentTime - gameStartTime;
    player1Pressed = true;
    Serial.print("Player 1 pressed at: ");
    Serial.print(player1PressTime);
    Serial.println(" ms");
    delay(50); // Debounce
  }
 
  // Check player 2 button
  if (!player2Pressed && digitalRead(BUTTON_PLAYER2_PIN) == LOW) {
    player2PressTime = currentTime - gameStartTime;
    player2Pressed = true;
    Serial.print("Player 2 pressed at: ");
    Serial.print(player2PressTime);
    Serial.println(" ms");
    delay(50); // Debounce
  }
 
  // Check if both players pressed
  if (player1Pressed && player2Pressed) {
    gameActive = false;
    determineWinner();
  }
}

// Determine the winner
void determineWinner() {
  Serial.println("Determining winner...");

    // Returning to zero position
  Serial.println("Servo returning to center (90 deg)");
  gameServo.write(90);
  delay(1000);  // Give servo time to return to center
 
  unsigned long targetTime = selectedTime * 1000; // Target in milliseconds
 
  // Calculate the difference (absolute value)
  unsigned long player1Diff = abs((long)(player1PressTime - targetTime));
  unsigned long player2Diff = abs((long)(player2PressTime - targetTime));
 
  Serial.print("Target time: ");
  Serial.print(targetTime);
  Serial.println(" ms");
 
  Serial.print("Player 1 difference: ");
  Serial.print(player1Diff);
  Serial.println(" ms");
 
  Serial.print("Player 2 difference: ");
  Serial.print(player2Diff);
  Serial.println(" ms");
 
  // Display results
  lcd.clear();
  lcd.setCursor(0, 0);
 
  if (player1Diff < player2Diff) {
    lcd.print("THE WINNER IS...");
    lcd.setCursor(0, 1);
    lcd.print("PLAYER NO. 1");
    Serial.println("WINNER: Player 1!");

    Serial.println("Servo pointing to Player 1 (LEFT)");
    gameServo.write(45);  // 45 degrees = left
    delay(1500);
    gameServo.detach();    // Turn off PWM
   

  } else if (player2Diff < player1Diff) {
    lcd.print("THE WINNER IS...");
    lcd.setCursor(0, 1);
    lcd.print("PLAYER NO. 2");
    Serial.println("WINNER: Player 2!");

    Serial.println("Servo pointing to Player 2 (RIGHT)");
    gameServo.write(135);  // 135 degrees = right
    delay(1500);
    gameServo.detach();    // Turn off PWM
   
  } else {
    lcd.print("IT'S A TIE!");
    lcd.setCursor(0, 1);
    lcd.print("AMAZING!");
    Serial.println("IT'S A TIE!");

    Serial.println("Servo back to center");
    gameServo.write(90);  // 90 degrees = center
  }
 
  // Winner buzzer
  playWinnerBuzzer();
}




// Winner buzzer
void playWinnerBuzzer() {
  Serial.println("Playing winner buzzer...");
 
  // Victory melody
  tone(BUZZER_PIN, 523, 200); // C
  delay(250);
  tone(BUZZER_PIN, 659, 200); // E
  delay(250);
  tone(BUZZER_PIN, 784, 200); // G
  delay(250);
  tone(BUZZER_PIN, 1047, 400); // High C
  delay(500);
 
  Serial.println("Winner buzzer finished");
}

void playGameTimeLockedIn(){
  tone(BUZZER_PIN, 440, 150);  // A
  delay(200);
  tone(BUZZER_PIN, 523, 150);  // C
  delay(200);
  tone(BUZZER_PIN, 659, 150);  // E
  delay(200);
  tone(BUZZER_PIN, 880, 400);  // High A - loud
}

void taylor(){
 
  tone(BUZZER_PIN, 659, 300); delay(350); // E
  tone(BUZZER_PIN, 698, 300); delay(350); // F
  tone(BUZZER_PIN, 784, 300); delay(350); // G
  tone(BUZZER_PIN, 880, 600); delay(650); // A
  tone(BUZZER_PIN, 784, 300); delay(350); // G
  tone(BUZZER_PIN, 698, 300); delay(350); // F
  tone(BUZZER_PIN, 659, 600); delay(650); // E

}