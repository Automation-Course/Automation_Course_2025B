#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// יצירת אובייקט סרוו
Servo myServo;

// הגדרת פינים לרכיבים
const int trigPin = 11;
const int echoPin = 12;
const int startButtonPin = 6;
const int targetButtonPin = 7;
const int buzzerPin = 9;

// הגדרת ספים לגובה ירידה/עלייה בזיהוי שכיבה
const int pushupLowThreshold = 10;
const int pushupHighThreshold = 30;

// משתנים למעקב אחרי מצב המערכת
int pushupCount = 0;
int pushupTarget = 5;
bool reachedBottom = false;
bool inTargetSelection = false;
bool inSet = false;

// אפשרויות ליעדי שכיבות סמיכה
int targetOptions[] = {5, 10, 15, 20};
int targetIndex = 0;

// יצירת אובייקט למסך
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // התחלת תקשורת ל־Serial Monitor
  Serial.begin(9600);

  // הגדרת תצורת פינים
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(startButtonPin, INPUT_PULLUP);
  pinMode(targetButtonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, HIGH); // הבאזר כבוי בהתחלה

  // אתחול מסך
  lcd.init();
  lcd.backlight();
  showWelcome();

  Serial.println("System initialized. Waiting for start...");
}

void loop() {
  float distance = measureDistance();  // קריאת מרחק מהחיישן

  // טיפול בלחיצה על כפתור התחלה (6)
  if (digitalRead(startButtonPin) == LOW) {
    delay(250); // מניעת רעידות

    // התחלת סט חדש (כניסה למצב בחירת מטרה)
    if (!inTargetSelection && !inSet) {
      Serial.println("Start button pressed → entering target selection");
      resetToTargetSelection();
      return;
    }

    // אישור מטרה והתחלת סט בפועל
    if (inTargetSelection && !inSet) {
      Serial.print("Target selected: ");
      Serial.println(pushupTarget);
      Serial.println("Set started.");

      inTargetSelection = false;
      inSet = true;
      pushupCount = 0;
      reachedBottom = false;

      lcd.clear();
      lcd.print("Set started!");
      delay(1000);
      lcd.clear();
      lcd.print("Pushups: 0");
      return;
    }
  }

  // שינוי מטרה עם כפתור 7
  if (digitalRead(targetButtonPin) == LOW && inTargetSelection) {
    delay(250);
    targetIndex = (targetIndex + 1) % 4;
    pushupTarget = targetOptions[targetIndex];

    Serial.print("Target changed to: ");
    Serial.println(pushupTarget);

    lcd.clear();
    lcd.print("Select target:");
    lcd.setCursor(0, 1);
    lcd.print(pushupTarget);
  }

  // אם בתוך סט – סופרים שכיבות סמיכה
  if (inSet) {
    if (distance < pushupLowThreshold) {
      reachedBottom = true;  // המשתמש ירד
    }

    if (distance > pushupHighThreshold && reachedBottom) {
      // המשתמש עלה → חזרה שלמה נרשמת
      pushupCount++;
      reachedBottom = false;

      Serial.print("Pushup counted! Total: ");
      Serial.println(pushupCount);

      // צפצוף באזר
      digitalWrite(buzzerPin, LOW);
      delay(100);
      digitalWrite(buzzerPin, HIGH);

      // עדכון מסך
      lcd.clear();
      lcd.print("Pushups: ");
      lcd.print(pushupCount);
      delay(500);

      // אם היעד הושלם
      if (pushupCount >= pushupTarget) {
        Serial.println("Target reached! Activating servo...");

        // הפעלת מנוע סרוו כסימן לסיום
        myServo.attach(10);
        myServo.write(90);
        delay(500);
        myServo.write(0);
        delay(3000);
        myServo.detach();

        lcd.clear();
        lcd.print("Target reached!");
        lcd.setCursor(0, 1);
        lcd.print("Great job!");
        delay(3000);
        showWelcome();
        inSet = false;

        Serial.println("Set complete. System reset.");
      }
    }
  }
}

// פונקציה למדידת מרחק (בס"מ)
float measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2.0;
}

// הצגת מסך פתיחה
void showWelcome() {
  lcd.clear();
  lcd.print("Pushup Counter");
  lcd.setCursor(0, 1);
  lcd.print("Press Btn 6");
  inSet = false;
  inTargetSelection = false;
}

// מעבר לבחירת מטרה חדשה
void resetToTargetSelection() {
  inSet = false;
  inTargetSelection = true;
  pushupCount = 0;
  targetIndex = 0;
  pushupTarget = targetOptions[targetIndex];
  reachedBottom = false;

  lcd.clear();
  lcd.print("Select target:");
  lcd.setCursor(0, 1);
  lcd.print(pushupTarget);

  Serial.println("Target selection mode active.");
}
