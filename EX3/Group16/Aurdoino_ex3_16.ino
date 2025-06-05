
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>  // Include the Servo library

// Pin definitions
const int buttonPin = 2;
const int buzzerPin = 8;
const int ledPin = 11;
const int greenLedPin = 10;
const int signalPin = 4;
const int servoPin = 6;

// Variables
bool lastButtonState = HIGH;
bool isBeeping = false;
unsigned long previousMillis = 0;
const long interval = 1000;
unsigned long startMillis = 0;
bool timeStarted = false;

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myServo;

// Setup function: Initializes hardware and serial communication
void setup() {
  // Initialize button, buzzer, LEDs, and motion sensor pins
  pinMode(buttonPin, INPUT_PULLUP);  
  pinMode(buzzerPin, OUTPUT);        
  pinMode(ledPin, OUTPUT);           
  pinMode(greenLedPin, OUTPUT);      
  pinMode(signalPin, INPUT);         

  // Attach the servo motor to its pin
  myServo.attach(servoPin);  

  // Initialize LCD display
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Push to Run...");

  // Initialize serial communication for debugging
  Serial.begin(9600);
  Serial.println("Setup complete!");
}

// Main loop function: Handles button press, motion detection, and actions
void loop() {
  // Read the current button state
  bool buttonState = digitalRead(buttonPin);

  // If button is pressed and time hasn't started yet, begin the countdown
  if (buttonState == LOW && lastButtonState == HIGH && !timeStarted) {
    isBeeping = true;
    previousMillis = millis();
    startMillis = millis();  
    timeStarted = true;
    lcd.clear();
    lcd.print("Running...");
    Serial.println("Button pressed. Time started.");
    playBeeps();  // Start the beeping sequence
  }

  // If time has started, check for motion detection
  if (timeStarted) {
    bool motionDetected = digitalRead(signalPin);

    // If motion is detected, process the elapsed time and trigger actions
    if (motionDetected == HIGH) {
      unsigned long elapsedTime = millis() - startMillis;
      String message = "Time: " + String(elapsedTime / 1000) + "s";
      displayMessage(message.c_str());  // Display elapsed time on LCD

      Serial.print("Motion detected. Elapsed time: ");
      Serial.print(elapsedTime / 1000);
      Serial.println(" seconds.");

      // Turn on the main LED for 3 seconds
      digitalWrite(ledPin, 255);  
      Serial.println("LED ON for 2 seconds.");
      delay(2000);  // Wait for 3 seconds
      digitalWrite(ledPin, 0);    
      Serial.println("LED OFF.");

      // Perform the flag swinging motion with the servo
      swingFlag();

      // Stop the countdown and reset the system
      timeStarted = false;  
      Serial.println("Finished Race.");

      lcd.clear();
      lcd.print("Push to Run...");
    }
  }

  // Update the button state for the next loop iteration
  lastButtonState = buttonState;

  // If the beeping sequence is active, display the next message on the LCD
  if (isBeeping) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      displayNextMessage();  // Update the message during the beep phase
    }
  }
}

// Function: Displays the next message during the beep phase
void displayNextMessage() {
  static int messageStep = 0;
  switch (messageStep) {
    case 0:
      break;
    case 1:
      break;
    case 2:
      displayMessage("Run!!!");
      break;
  }
  messageStep++;
  if (messageStep > 2) {
    isBeeping = false;  // End the beeping sequence after the last message
  }
}

// Function: Displays a message on the LCD
void displayMessage(const char* message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
}

// Function: Handles the beeping sound and LED actions
void playBeeps() {
  int beepFreq = 1000;
  int longBeepFreq = 1500;

  // First beep and LED action
  tone(buzzerPin, beepFreq);
  digitalWrite(ledPin, 255);
  Serial.println("Buzzer beeping...");
  delay(300);
  noTone(buzzerPin);
  digitalWrite(ledPin, 0);
  delay(500);

  // Second beep and LED action
  tone(buzzerPin, beepFreq);
  digitalWrite(ledPin, 255);
  Serial.println("Buzzer beeping again...");
  delay(300);
  noTone(buzzerPin);
  digitalWrite(ledPin, 0);
  delay(600);

  // Final long beep with green LED
  tone(buzzerPin, longBeepFreq);
  digitalWrite(greenLedPin, 255);
  Serial.println("Long beep and green LED ON...");
  displayMessage("Run!!!");
  delay(1000);
  noTone(buzzerPin);
  digitalWrite(greenLedPin, 0);
  Serial.println("Green LED OFF.");
}

// Function: Simulates a flag swinging movement using the servo
void swingFlag() {
  // Perform a sequence of servo movements to simulate flag waving
  Serial.println("Servo turning clockwise...");
  myServo.write(110);  
  delay(2000);          

  Serial.println("Servo turning counterclockwise...");
  myServo.write(70);    
  delay(2000);          

  Serial.println("Servo turning clockwise...");
  myServo.write(110);  
  delay(2000);          

  Serial.println("Servo turning counterclockwise...");
  myServo.write(70);    
  delay(2000);          

  Serial.println("Servo turning clockwise...");
  myServo.write(110);  
  delay(2000);          

  Serial.println("Servo turning counterclockwise...");
  myServo.write(70);    
  delay(2000);          

  // After the flag has swung, set the servo to neutral position
  myServo.write(90);  
  Serial.println("Servo stopped at 90 degrees.");
}
