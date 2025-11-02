// Built in Platformio

#include <Arduino.h>
#include <LiquidCrystal.h>

// --- Pin Definitions ---
const int LED_PIN = 2;
const int POT_A_PIN = 34;
const int POT_B_PIN = 35;
const int SWITCH_PIN = 33;

// --- LCD Pins (RS, E, D4, D5, D6, D7) ---
LiquidCrystal lcd(12, 13, 14, 27, 26, 25);

// --- Variables ---
int rateA = 0, rateB = 0;
int activeDelay = 0;
bool sliderA_active = true;
int ledState = LOW;
unsigned long prevBlinkMillis = 0;
unsigned long prevLcdMillis = 0;

// LCD update every 100 ms
const unsigned long LCD_INTERVAL = 100;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Slider Control");
  delay(1000);
  lcd.clear();
}

void loop() {
  unsigned long currentMillis = millis();

  // --- Read potentiometers ---
  rateA = map(analogRead(POT_A_PIN), 0, 4095, 50, 500);
  rateB = map(analogRead(POT_B_PIN), 0, 4095, 50, 500);

  // --- Switch logic (toggle switch) ---
  sliderA_active = (digitalRead(SWITCH_PIN) == HIGH);

  // --- Choose active delay ---
  activeDelay = sliderA_active ? rateA : rateB;

  // --- Non-blocking LED blink ---
  if (currentMillis - prevBlinkMillis >= activeDelay) {
    prevBlinkMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
  }

  // --- Throttled LCD update ---
  if (currentMillis - prevLcdMillis >= LCD_INTERVAL) {
    prevLcdMillis = currentMillis;

    lcd.setCursor(0, 0);
    lcd.print("A:");
    lcd.print(rateA);
    lcd.print(sliderA_active ? "* " : "  ");

    lcd.setCursor(8, 0);
    lcd.print("B:");
    lcd.print(rateB);
    lcd.print(!sliderA_active ? "*" : " ");

    lcd.setCursor(0, 1);
    lcd.print("Delay:");
    lcd.print(activeDelay);
    lcd.print(" ms   ");

    Serial.printf("Active: %c | rateA=%d | rateB=%d | delay=%d\n",
                  (sliderA_active ? 'A' : 'B'), rateA, rateB, activeDelay);
  }
}
