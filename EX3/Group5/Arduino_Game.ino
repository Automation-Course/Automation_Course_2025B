#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Define pins
#define TRIG_PIN 10
#define ECHO_PIN 9
#define BUTTON_PIN1 2
#define BUTTON_PIN2 4
#define BUZZER_PIN 8
#define SERVO_PIN 5

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo gameServo;

// Game variables
int score1 = 0;
int score2 = 0;
int servoPos = 90;  // Centered start (for clarity)
int gameState = -1;  // Start from waiting state
int handCount = 0;
bool handWasThere = false;
unsigned long stateStart = 0;
int positionCounter = 0;
bool pointGiven = false;

void setup() {
  Serial.begin(9600);
  Serial.println("System started. Waiting for 2 hands to start the game...");

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUTTON_PIN1, INPUT);
  pinMode(BUTTON_PIN2, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Ready to play?");
  lcd.setCursor(0, 1);
  lcd.print("Wave your hand!");

}

void loop() {
  unsigned long now = millis();

  switch (gameState) {
    
    case -1:  // Waiting for hand wave (start state)
      lcd.setCursor(0, 0);
      lcd.print("Ready to play?");
      lcd.setCursor(0, 1);
      lcd.print("Wave your hand!");

      // Ultrasonic distance detection
      long duration, distance;
      digitalWrite(TRIG_PIN, LOW);
      delayMicroseconds(2);
      digitalWrite(TRIG_PIN, HIGH);
      delayMicroseconds(10);
      digitalWrite(TRIG_PIN, LOW);
      duration = pulseIn(ECHO_PIN, HIGH);
      distance = duration * 0.034 / 2;

      if (distance < 10 && !handWasThere) {
        handCount++;
        handWasThere = true;
        Serial.print("Hand detected: ");
        Serial.println(handCount);
        delay(1000);  // Debounce to prevent duplicates
      } else if (distance >= 10) {
        handWasThere = false;
      }

      if (handCount >= 2) {
        Serial.println("Both hands detected. Starting game...");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Battle begins!");
        lcd.setCursor(0, 1);
        lcd.print("Good luck :)");
        playStartMelody();
        delay(3000);
        lcd.clear();

        gameServo.attach(SERVO_PIN);
        gameServo.write(90);  // Center
        delay(500);

        handCount = 0;  // reset for future use
        gameState = 0;
      }
      break;

    case 0: // Show scores
      lcd.setCursor(0, 0);
      lcd.print("P1:");
      lcd.print(score1);
      lcd.setCursor(9, 0);
      lcd.print("P2:");
      lcd.print(score2);
      stateStart = now;
      gameState = 1;
      break;

    case 1: // Wait random delay
      if (now - stateStart >= 1) {
        stateStart = now;
        delay(random(1500, 3000));
        gameState = 2;
      }
      break;

    case 2: // Play beep
      Serial.println(">> Beep triggered, get ready to press!");
      tone(BUZZER_PIN, 1000, 200);
      stateStart = now;
      pointGiven = false;
      gameState = 3;
      break;

    case 3: // Wait for button
      if (!pointGiven && digitalRead(BUTTON_PIN1) == HIGH) {
        Serial.println("Player 1 pressed first!");
        positionCounter--;        
        score1++;
        moveServoLeft();
        pointGiven = true;
        stateStart = now;
        gameState = 4;
      }
      else if (!pointGiven && digitalRead(BUTTON_PIN2) == HIGH) {
        Serial.println("Player 2 pressed first!");
        positionCounter++; 
        score2++;
        moveServoRight();
        pointGiven = true;
        stateStart = now;
        gameState = 4;
      }
      else if (now - stateStart > 2000) {
        Serial.println("No button was pressed. Restarting round...");
        gameState = 0;  // No one pressed
      }
      break;

    case 4: // Check win
      if (positionCounter <= -3 || positionCounter >= 3) {
        lcd.clear();
        lcd.setCursor(0, 0);
        if (positionCounter <= -3) {
          lcd.print("Player 1 wins!");
          Serial.println(">>> Player 1 wins the game!");
        } else {
          lcd.print("Player 2 wins!");
          Serial.println(">>> Player 2 wins the game!");
        }

        lcd.setCursor(0, 1);
        lcd.print("Score ");
        lcd.print(score1);
        lcd.print(":");
        lcd.print(score2);

        Serial.println("Final score:");
        Serial.print("Player 1: "); Serial.println(score1);
        Serial.print("Player 2: "); Serial.println(score2);

        playVictoryMelody();

        for (int i = 0; i < 6; i++) {
          gameServo.write(120); delay(500);
          gameServo.write(60);  delay(500);
        }
        gameServo.write(90);
        Serial.println("Game ended. Returning to standby mode.");
        gameState = 5; // Go to reset state
      } else {
        gameState = 0;
      }
      break;

    case 5: // Reset state after game ended
      score1 = 0;
      score2 = 0;
      positionCounter = 0;
      handCount = 0;
      handWasThere = false;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Play again?");
      lcd.setCursor(0, 1);
      lcd.print("Wave your hand!");

      Serial.println("System reset. Waiting for new hand wave...");
      delay(1000); // small delay before returning to wait
      gameState = -1;
      break;
  }
}

// Move servo left briefly (Player 1)
void moveServoLeft() {
  gameServo.write(60);  // turn left
  delay(300);
  gameServo.write(90);  // stop
}

// Move servo right briefly (Player 2)
void moveServoRight() {
  gameServo.write(120);  // turn right
  delay(300);
  gameServo.write(90);   // stop
}


// Start melody
void playStartMelody() {
  tone(BUZZER_PIN, 880); delay(150);
  tone(BUZZER_PIN, 660); delay(150);
  tone(BUZZER_PIN, 990); delay(300);
  noTone(BUZZER_PIN);
}

// Victory melody - We Are The Champions 
void playVictoryMelody() {
  tone(BUZZER_PIN, 349, 600); // F4 
  delay(600);
  tone(BUZZER_PIN, 330, 200); // E4
  delay(200);
  tone(BUZZER_PIN, 349, 300); // F4
  delay(300);
  tone(BUZZER_PIN, 330, 400); // E4 
  delay(400);
  tone(BUZZER_PIN, 262, 400); // C4 
  delay(400);
  tone(BUZZER_PIN, 220, 200); // A3 
  delay(200);
  tone(BUZZER_PIN, 294, 400); // D4
  delay(400);
  tone(BUZZER_PIN, 220, 200); // A3
  delay(500);
  noTone(BUZZER_PIN);
}

