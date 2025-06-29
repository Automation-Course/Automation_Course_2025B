// Include necessary libraries
#include <Wire.h> 
#include <Adafruit_LiquidCrystal.h> // Library for LCD display
#include <Servo.h> // Library for controlling servo motors

Adafruit_LiquidCrystal lcd(0);

Servo myServo; // Create a servo object

// Pin definitions
const int redLed = 13;     // Digital pin for the red LED
const int greenLed = 7;    // Digital pin for the green LED
const int buttonPin = 2;   // Digital pin for the button
const int trigPin = A0;    // Analog pin for the ultrasonic sensor's Trigger pin
const int echoPin = A1;    // Analog pin for the ultrasonic sensor's Echo pin
const int servoPin = 10;   // Digital pin for the servo motor

// System states using an enumeration for clear state management
enum State { WAIT_TARGET, WAIT_START, TRAINING, DONE };
State systemState = WAIT_TARGET; // Initialize the system to the WAIT_TARGET state

int targetReps = 0;    // Variable to store the target number of repetitions
int repsDone = 0;      // Variable to track completed repetitions

// Variables for distance sensor (ultrasonic sensor)
long duration;          // Stores the time taken for the sound wave to travel
float distance;         // Stores the calculated distance in centimeters

// Flag to prevent double counting of a single repetition
bool alreadyCounted = false;

// Flag to track if training has started (though 'trainingStarted' is not used after initialization)
bool trainingStarted = false;

void setup()
{
  // Initialize serial communication at 9600 baud rate for debugging and input
  Serial.begin(9600);
  
  // Initialize the LCD with 16 columns and 2 rows
  lcd.begin(16, 2);
  
  // Set pin modes for all connected components
  pinMode(redLed, OUTPUT);     // Red LED as an output
  pinMode(greenLed, OUTPUT);   // Green LED as an output
  pinMode(buttonPin, INPUT);   // Button as an input
  pinMode(trigPin, OUTPUT);    // Ultrasonic sensor Trigger pin as an output
  pinMode(echoPin, INPUT);     // Ultrasonic sensor Echo pin as an input
  
  // Attach the servo object to the servo pin
  myServo.attach(servoPin);

  // Initial message printed to the Serial Monitor
  Serial.println("Enter desired reps target:");
  // Initial message displayed on the LCD
  lcd.setCursor(0, 0); // Set cursor to column 0, row 0
  lcd.print("Enter target reps");
}

void loop() {
  // Main state machine for the fitness tracker
  switch (systemState) {
    case WAIT_TARGET:
      // Wait for the user to enter the target repetitions via Serial Monitor
      if (Serial.available() > 0) {
        targetReps = Serial.parseInt(); // Read the integer from Serial input
        Serial.print("Target reps: ");
        Serial.println(targetReps);

        lcd.clear(); // Clear the LCD
        lcd.setCursor(0, 0); // Set cursor to column 0, row 0
        lcd.print("Target: ");
        lcd.print(targetReps);
        lcd.setCursor(0, 1); // Set cursor to column 0, row 1
        lcd.print("Press button");

        systemState = WAIT_START; // Transition to WAIT_START state
      }
      break;

    case WAIT_START:
      // Wait for the user to press the button to start training
      if (digitalRead(buttonPin) == HIGH) { // Check if the button is pressed
        Serial.println("Training started!");
        lcd.clear(); // Clear the LCD
        lcd.setCursor(0, 0); // Set cursor to column 0, row 0
        lcd.print("Reps: 0/");
        lcd.print(targetReps);

        myServo.write(0); // Move servo to the starting position (0 degrees)

        systemState = TRAINING; // Transition to TRAINING state
      }
      break;

    case TRAINING:
      readDistance(); // Call function to read distance from ultrasonic sensor
      
      // Check if distance is within range for a valid rep and not already counted
      if (distance < 10 && !alreadyCounted) {
        repsDone++; // Increment completed reps
        Serial.print("Valid rep: ");
        Serial.println(repsDone);

        lcd.setCursor(0, 0); // Update rep count on LCD
        lcd.print("Reps: ");
        lcd.print(repsDone);
        lcd.print("/");
        lcd.print(targetReps);

        digitalWrite(greenLed, HIGH); // Turn on green LED for valid rep
        digitalWrite(redLed, LOW);    // Turn off red LED

        alreadyCounted = true; // Set flag to prevent double counting
      }
      else if (distance >= 10) {
        // If distance is not low enough for a rep
        digitalWrite(greenLed, LOW);  // Turn off green LED
        digitalWrite(redLed, HIGH);   // Turn on red LED
      }
      
      // Reset the alreadyCounted flag when the object moves far enough back up
      if (distance > 15) {
        alreadyCounted = false;
      }
      
      // Check if the target number of repetitions has been reached
      if (repsDone >= targetReps) {
        myServo.write(180); // Move servo to the end position (180 degrees)
        Serial.println("Well done! Training complete!");

        lcd.setCursor(0, 1); // Display completion message on LCD
        lcd.print("Done! Good job!");

        digitalWrite(greenLed, HIGH); // Turn on green LED
        digitalWrite(redLed, LOW);    // Turn off red LED

        systemState = DONE; // Transition to DONE state
      }
      delay(200); // Small delay to stabilize readings and prevent rapid loops
      break;
      
    case DONE:
      // The system stays in the DONE state, waiting for a manual reset (e.g., power cycle)
      // No code executes continuously in this state
      break;
  }
}

// Function to read distance from the ultrasonic sensor
void readDistance() {
  // Clear the trigPin by setting it LOW for 2 microseconds
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Set the trigPin HIGH for 10 microseconds to send a pulse
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the duration of the pulse on the echoPin (time for sound to return)
  duration = pulseIn(echoPin, HIGH);
  // Calculate the distance in centimeters
  // Speed of sound in air is ~0.0343 cm/microsecond.
  // Divide by 2 because the sound travels to the object and back.
  distance = (duration * 0.0343) / 2; // distance in cm
}