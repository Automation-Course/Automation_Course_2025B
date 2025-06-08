#define TRIG_PIN 10
#define ECHO_PIN 9

const int numTests = 10;             // Total number of tests
const int maxDistanceCM = 15;        // Max valid distance
const unsigned long testInterval = 5000;  // Time between tests

int currentTest = 0;
unsigned long lastTestTime = 0;
bool testStarted = false;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.println("=== Ultrasonic Distance Test ===");
  Serial.print("Preparing to measure ");
  Serial.print(numTests);
  Serial.println(" distances (up to 15 cm)");
  Serial.println("Starting in 3 seconds...");
  delay(3000);
  lastTestTime = millis();
  testStarted = true;
}

void loop() {
  if (!testStarted) return;
  
  if (currentTest >= numTests) {
    Serial.println("=== Distance test completed ===");
    testStarted = false;
    return;
  }

  unsigned long now = millis();
  if (now - lastTestTime >= testInterval) {
    currentTest++;
    Serial.println("------------------------------");
    Serial.print("Test #");
    Serial.println(currentTest);

    long duration, distance;

    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    duration = pulseIn(ECHO_PIN, HIGH);
    distance = duration * 0.034 / 2;

    if (distance > 0 && distance <= maxDistanceCM) {
      Serial.print("Measured distance: ");
      Serial.print(distance);
      Serial.println(" cm");
    } else {
      Serial.println("Out of range (>15 cm) or no object detected.");
    }

    Serial.println("Prepare for the next measurement...");
    lastTestTime = now;
  }
}