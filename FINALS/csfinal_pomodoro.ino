#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ================= LCD =================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ================= PINS =================
const int greenLED = 5;
const int blueLED = 6;
const int yellowLED = 7;
const int redLED = 8;
const int buzzer = 9;

// ================= BUTTONS =================
const int startPauseBtn = 2;
const int resetBtn = 3;

// ================= TIMER =================
int studyTime = 25;
int breakTime = 5;
int seconds = 25;

String mode = "MID";

bool running = false;
bool isBreak = false;
bool transitioning = false;

unsigned long lastTime = 0;
unsigned long lastButtonTime = 0;
const int debounceDelay = 200;

String input = "";

// ================= TONES =================
void studyTone() {
  tone(buzzer, 1000, 150);
  delay(180);
}

void breakTone() {
  tone(buzzer, 600, 150);
  delay(180);
}

void transitionTone() {
  tone(buzzer, 1200, 120);
  delay(150);
}

// ================= UPDATE TIMER FROM PYTHON =================
void updateTimer(String data) {

  if (data.startsWith("T")) {

    int firstComma = data.indexOf(',');
    int secondComma = data.lastIndexOf(',');

    studyTime = data.substring(1, firstComma).toInt();
    breakTime = data.substring(firstComma + 1, secondComma).toInt();
    mode = data.substring(secondComma + 1);

    seconds = studyTime;
  }
}

// ================= LED =================
void updateLEDs() {

  if (!running) {
    digitalWrite(redLED, HIGH);
    return;
  }

  digitalWrite(redLED, LOW);

  if (!isBreak) {
    digitalWrite(greenLED, seconds > 10);
    digitalWrite(yellowLED, seconds <= 10);
    digitalWrite(blueLED, LOW);
  } else {
    digitalWrite(blueLED, seconds > 1);
    digitalWrite(yellowLED, seconds <= 1);
    digitalWrite(greenLED, LOW);
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  pinMode(startPauseBtn, INPUT_PULLUP);
  pinMode(resetBtn, INPUT_PULLUP);

  lcd.print("POMODORO READY");
}

// ================= LOOP =================
void loop() {

  // ================= SERIAL =================
  if (Serial.available()) {

    input = Serial.readStringUntil('\n');
    input.trim();

    // rating display
    if (input == "1") {
      lcd.clear();
      lcd.print("LOW ENERGY 😴");
      tone(buzzer, 400, 200);
    }
    else if (input == "2") {
      lcd.clear();
      lcd.print("MID STATE 🙂");
      tone(buzzer, 800, 150);
    }
    else if (input == "3") {
      lcd.clear();
      lcd.print("READY 😌");
      tone(buzzer, 1200, 150);
    }

    updateTimer(input);
  }

  // ================= BUTTONS (FIXED LCD OUTPUT) =================
  if (millis() - lastButtonTime > debounceDelay) {

    // START / PAUSE BUTTON
    if (digitalRead(startPauseBtn) == LOW) {
      lastButtonTime = millis();
      running = !running;

      lcd.clear();

      if (!running) {
        lcd.print("PAUSED");
      } else {
        lcd.print("STARTED");
      }
    }

    // RESET BUTTON
    if (digitalRead(resetBtn) == LOW) {
      lastButtonTime = millis();

      running = false;
      isBreak = false;
      seconds = studyTime;

      lcd.clear();
      lcd.print("RESET");
    }
  }

  // ================= TIMER =================
  if (running && !transitioning) {

    if (millis() - lastTime >= 1000) {
      lastTime = millis();

      seconds--;

      lcd.setCursor(0,0);
      lcd.print(mode + " MODE   ");

      lcd.setCursor(0,1);
      lcd.print((isBreak ? "BREAK " : "STUDY "));
      lcd.print(seconds);
      lcd.print("s   ");

      updateLEDs();

      if (seconds <= 0) {

        transitioning = true;
        running = false;

        transitionTone();
        delay(300);

        if (!isBreak) {
          isBreak = true;
          seconds = breakTime;
          breakTone();
        } else {
          isBreak = false;
          seconds = studyTime;
          studyTone();
        }

        delay(300);
        transitioning = false;
        running = true;
      }
    }
  }

  updateLEDs();
}