

#include <Wire.h>          // I2C for RTC
#include <RTClib.h>        // Real-time clock
#include <Adafruit_NeoPixel.h>  // WS2812 LEDs
#include "Arduino.h"

// --- Pins ---
#define BTN_POWER 18
#define BTN_MODE 0
#define BTN_INC 5
#define LED_PIN 9
#define NUM_LEDS 64
#define BUZZER_PIN 13

// --- Objects ---
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
RTC_DS3231 rtc;

int getIndex(int row, int col) {
  if (row < 0 || row > 7 || col < 0 || col > 7) return -1;

  static const uint8_t map[8][8] = {
    {  0,  1,  2,  3,  4,  5,  6,  7 },
    { 27, 28,  39, 40, 51, 52, 63, 8},
    { 26, 29, 38, 41, 50, 53, 62, 9 },
    { 25, 30, 37, 42, 49, 54, 61, 10 },
    { 24, 31, 36, 43, 48, 55, 60, 11 },
    { 23, 32, 35, 44, 47, 56, 59, 12 },
    { 22, 33, 34, 45, 46, 57, 58, 13 },
    { 21, 20, 19, 18, 17, 16, 15, 14 }
  };

  return map[row][col];
}

// --- Function Declarations ---
void handleButtons();
void displayClock();
void displayAlarmSet();
void checkAlarm();
void ledTesting();

// --- State Variables ---
bool deviceOn = false;
int currentMode = 0;  
int alarmHours = 0;    
int alarmMin = 0;  
bool alarmSettingMode = false;
bool isPressed = false;
bool incPressed = false;
bool editingHours = false;
int lastHours = -1;
int lastSeconds = -1;
int lastMinutes = -1;
bool powerButtonPressed = false;
bool alarmTriggered = false;
bool alarmRinging = false;

// --- Setup ---
void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.show();

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BTN_POWER, INPUT_PULLUP);
  pinMode(BTN_MODE, INPUT_PULLUP);
  pinMode(BTN_INC, INPUT_PULLUP);

  if (!rtc.begin()) {
    Serial.println("RTC not found!");
    while (1);
  }
}



// --- Loop ---
void loop() {

  handleButtons();

  if (deviceOn) {
    if (currentMode == 0) {
      displayClock();
    } 
    else if (currentMode == 1) {
      displayAlarmSet();
    }
  } 
  else {
    strip.clear();
    strip.show();
  }

  checkAlarm();
  if (digitalRead(BTN_POWER) == LOW && !alarmTriggered) {
    alarmTriggered = true;
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    strip.clear();
  }
}

// --- Functions ---
void handleButtons() {
  
  // POWER BUTTON
if (digitalRead(BTN_POWER) == LOW) {
  delay(200); // crude debounce

  if (alarmRinging) {
    // Stop the alarm
    alarmRinging = false;
    digitalWrite(BUZZER_PIN, LOW);
    strip.clear();
    strip.show();
    Serial.println("Alarm stopped");
  } else {
    // Toggle device state
    deviceOn = !deviceOn;
  }
}

  // --- MODE BUTTON ---
  static unsigned long pressStart = 0;
  static unsigned long pressDuration = 0;

  if (digitalRead(BTN_MODE) == LOW && !isPressed) {
    pressStart = millis();
    isPressed = true;
  }

  if (digitalRead(BTN_MODE) == HIGH && isPressed) {
    pressDuration = millis() - pressStart;
    isPressed = false;

    if (pressDuration < 1000) {
      // SHORT PRESS
      currentMode = (currentMode + 1) % 2;
      Serial.println("Short Press -> Mode change");
      if (currentMode == 0) Serial.println("Clock mode");
      if (currentMode == 1) Serial.println("Alarm mode");
    } else {
      // LONG PRESS
      if (currentMode == 1) {
        alarmSettingMode = !alarmSettingMode;
        Serial.println("Alarm setting toggled");
      }
    }
  }

  // --- INC BUTTON ---
  static unsigned long incPressStart = 0;
  static unsigned long incPressDuration = 0;

  if (alarmSettingMode) {
    if (digitalRead(BTN_INC) == LOW && !incPressed) {
      incPressStart = millis();
      incPressed = true;
    }

    if (digitalRead(BTN_INC) == HIGH && incPressed) {
      incPressDuration = millis() - incPressStart;
      incPressed = false;

      if (incPressDuration < 1000) {
        // short press -> increment hours or minutes
        if (editingHours) {
          alarmHours = (alarmHours + 1) % 24;
          Serial.print("Hour set to: "); Serial.println(alarmHours);
        } else {
          alarmMin = (alarmMin + 1) % 60;
          Serial.print("Minute set to: "); Serial.println(alarmMin);
        }
      } else {
        // long press -> toggle between hours/mins
        editingHours = !editingHours;
        Serial.println(editingHours ? "Editing Hours" : "Editing Minutes");
      }
    }
  }
}

const int digit_pattern[10][4][4] = {
    {//0 
    {1, 1, 1, 0},
    {1, 0, 1, 0},
    {1, 0, 1, 0},
    {1, 1, 1, 0}
    }, 
    {//1
      {0, 1, 0, 0},
      {1, 1, 0, 0},
      {0, 1, 0, 0},
      {1, 1, 1, 0}
    },
    {//2
      {1, 1, 0, 0},
      {0, 0, 1, 0},
      {1, 1, 0, 0},
      {1, 1, 1, 0}
    }, 
    {//3
      {1, 1, 1, 0},
      {0, 1, 1, 0},
      {0, 0, 1, 0},
      {1, 1, 1, 0}
    }, 
    {
      {0, 0, 1, 0},
      {0, 1, 1, 0},
      {1, 1, 1, 1},
      {0, 0, 1, 0}
    },
    {
      {1, 1, 1, 1},
      {1, 1, 0, 0},
      {0, 0, 1, 0},
      {1, 1, 0, 0}
    },
    {
      {1, 1, 1, 1},
      {1, 0, 0, 0},
      {1, 1, 1, 0},
      {1, 1, 1, 0}
    },
    {
      {1, 1, 1, 0},
      {0, 0, 1, 0},
      {0, 0, 1, 0},
      {0, 0, 1 ,0}
    },
    {
      {0, 1, 1, 1},
      {0, 0, 1, 0},
      {0, 1, 1, 1},
      {0, 1, 1, 1}
    },
    {
      {0, 1, 1, 1},
      {0, 1, 1, 1},
      {0, 0, 0, 1},
      {0, 0, 0, 1},
    }

  };
  void drawDigit(int digit, int startRow, int startCol) {
  for (int r = 0; r < 4; r++) {
    for (int c = 0; c < 4; c++) {
      if (digit_pattern[digit][r][c] == 1) {
        int ledIndex = getIndex(startRow + r, startCol + c);
        strip.setPixelColor(ledIndex, strip.Color(255, 0, 0));
      }
    }
  }
}

void displayClock() {
  strip.clear();

  
  DateTime now = rtc.now();
  int hours = now.hour();
  int minutes = now.minute();
  int seconds = now.second();

  if (hours != lastHours || minutes != lastMinutes || seconds != lastSeconds) {
    int time[3] = {hours, minutes, seconds};
    for (int i = 0; i < 3; i++) {
    if (time[i] < 10) {
      Serial.print("0");
    }
    Serial.print(time[i]);

    if (i < 2) {
      Serial.print(":");
    }
  }
  lastHours = hours;
  lastMinutes = minutes;
  lastSeconds = seconds;
  Serial.println();
  
}
int hTens = hours / 10;
int hOnes = hours % 10;
 int mTens = minutes / 10;
int mOnes = minutes % 10;

strip.clear();

drawDigit(hTens, 0, 0); // top-left
drawDigit(hOnes, 0, 4); // top-right
drawDigit(mTens, 4, 0); // bottom-left
drawDigit(mOnes, 4, 4); // bottom-right
strip.show();


}

void displayAlarmSet() {
  strip.clear();


  int hTens = alarmHours/ 10;
  int hOnes = alarmHours % 10;
  int mTens = alarmMin / 10;
  int mOnes = alarmMin % 10;

  drawDigit(hTens, 0, 0); // top-left
  drawDigit(hOnes, 0, 4); // top-right
  drawDigit(mTens, 4, 0); // bottom-left
  drawDigit(mOnes, 4, 4); // bottom-right
  strip.show();

}

void checkAlarm() {
  DateTime now = rtc.now();
  int curHours = now.hour();
  int curMin = now.minute();

  // Trigger alarm if time matches and not already triggered
  if (curHours == alarmHours && curMin == alarmMin && !alarmTriggered) {
    alarmTriggered = true;
    alarmRinging = true;   

    digitalWrite(BUZZER_PIN, HIGH);

    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 255)); // blue flash
    }
    strip.show();
  }

  // Keep alarm ringing until stopped
  if (alarmRinging) {
    digitalWrite(BUZZER_PIN, HIGH);
  }
}
