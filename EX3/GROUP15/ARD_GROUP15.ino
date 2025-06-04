#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h> // Include Servo library

// Set the LCD address to 0x27 for a 16 char, 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define the pin for the button
const int buttonPin = 2;

// Define ultrasonic sensor pins
const int trigPin = 10;
const int echoPin = 11;

// Define buzzer pin
const int buzzerPin = 8;

// Define servo pin
const int servoPin = 5; // The pin connected to the MG995 servo

// Create a Servo object
Servo myServo;

// Define variables
int pushUpCount = 0;
long duration;
int distance;
int lastDistance = 100; // Variable to store previous distance
bool isCounting = false; // To track when counting should start

// Servo variables
int angle = 0;
bool Direction = true;
unsigned long rotationStartTime = 0; // Store time when rotation starts
bool rotationInProgress = false; // Flag to indicate whether the rotation should still be happening

void setup() {
  // Initialize the LCD
  lcd.init();
  lcd.backlight();

  // Initialize Serial Monitor
  Serial.begin(9600);

  // Set button pin as input
  pinMode(buttonPin, INPUT);

  // Set ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Set buzzer pin as output
  pinMode(buzzerPin, OUTPUT);

  // Attach the servo to the specified pin
  myServo.attach(servoPin);
}

void loop() {
  // Read the button state
  int buttonState = digitalRead(buttonPin);

  // If the button is pressed (assuming active HIGH)
  if (buttonState == HIGH) {
    // Show "Start Training" for 5 seconds
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Start Training");
    Serial.println("Training started...");
    delay(5000); // Wait for 5 seconds

    // Reset push-up count and start training
    pushUpCount = 0;
    lastDistance = 100; // Set lastDistance to something higher than 10 cm to start
    isCounting = true;

    // Now start measuring the distance
    while (isCounting) {
      // Trigger the ultrasonic sensor
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);

      // Measure the time it takes for the echo to return
      duration = pulseIn(echoPin, HIGH);

      // Calculate the distance in cm
      distance = duration * 0.0344 / 2;

      // If the distance is less than 10 cm and lastDistance was greater than 10 cm
      if (distance < 10 && lastDistance >= 10) {
        // Increment push-up count
        pushUpCount++;

        // Print to Serial Monitor
        Serial.print("Push-up done! Current count: ");
        Serial.println(pushUpCount);

        // Display push-up count on LCD
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Push-ups Count: ");
        lcd.setCursor(0, 1);
        lcd.print(pushUpCount);

        // Play beep sound when counter goes up
        tone(buzzerPin, 1000, 200); // Beep with 1000 Hz for 200 milliseconds

        delay(1000); // Wait for 1 second (debounce)

        // When reaching 10 push-ups, display "Great job!"
        if (pushUpCount >= 10) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Great job!");
          Serial.println("Training complete. Great job!");

          // Play success music (a simple melody)
          successMusic();

          // Start rotating the servo (rotate back and forth)
          startRotation();

          isCounting = false; // Stop the training after 10 push-ups
        }
      }

      // Update the last distance for comparison
      lastDistance = distance;

      delay(100); // Small delay to avoid sensor noise
    }
  }

  // If rotation is in progress, update the servo movement
  if (rotationInProgress) {
    rotateServo();
  }
}

// Function to play a success music when "Great job!" is shown
void successMusic() {
  int melody[] = { 262, 294, 330, 349, 392, 440, 494, 523 }; // Frequencies (C4 to C5)
  int noteDuration = 300; // Duration of each note

  // Play a simple ascending melody
  for (int i = 0; i < 8; i++) {
    tone(buzzerPin, melody[i], noteDuration); // Play each note
    delay(noteDuration);
  }
  noTone(buzzerPin); // Stop the buzzer after the melody
}

// Function to start the servo rotation
void startRotation() {
  rotationStartTime = millis(); // Store the current time
  rotationInProgress = true;    // Enable rotation
}

// Function to rotate the servo back and forth for 5 seconds
void rotateServo() {
  unsigned long currentMillis = millis();

  // Check if 5 seconds have passed since rotation started
  if (currentMillis - rotationStartTime < 5000) {
    // If still within 5 seconds, keep rotating the servo
    if (Direction) {
      angle++;
      if (angle == 180) {
        Direction = false;
      }
    } else {
      angle--;
      if (angle == 0) {
        Direction = true;
      }
    }

    myServo.write(angle);
    delay(15); // Smooth rotation
  } else {
    // After 5 seconds, stop rotation and detach the servo
    rotationInProgress = false; // Stop the rotation
    myServo.write(angle); // Make sure the servo holds its last position
    myServo.detach(); // Detach the servo to stop the rotation
  }
}