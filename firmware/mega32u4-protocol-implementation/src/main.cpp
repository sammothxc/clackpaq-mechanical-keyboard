/*
* Protocol Test Keyboard Matrix
* For Elite-C Pro Micro (ATmega32U4)
* 
* Scans a 7x10 key matrix and outputs XT scancodes
* 
* License: CC BY-NC-SA 4.0
* https://github.com/sammothxc/clackpaq
*/

#include <Arduino.h>
#include "config.h"

// state vars
uint8_t keyState[ROWS][COLS];          // current debounced state
uint8_t lastKeyState[ROWS][COLS];      // previous state for edge detection
uint32_t lastDebounceTime[ROWS][COLS]; // debounce timers
bool fnPressed = false;                // fn layer active

// line control, open collector style
inline void clkHigh() {
  pinMode(XT_CLK, INPUT_PULLUP);
}

inline void clkLow() {
  pinMode(XT_CLK, OUTPUT);
  digitalWrite(XT_CLK, LOW);
}

inline void dataHigh() {
  pinMode(XT_DATA, INPUT_PULLUP);
}

inline void dataLow() {
  pinMode(XT_DATA, OUTPUT);
  digitalWrite(XT_DATA, LOW);
}

inline bool readClk() {
  pinMode(XT_CLK, INPUT_PULLUP);
  return digitalRead(XT_CLK);
}

void xt_write(uint8_t value, bool isBreak) {
  // wait for host to release CLK (not inhibiting)
  uint16_t timeout = 10000;
  while (!readClk() && timeout--) {
    delayMicroseconds(10);
  }
  if (timeout == 0) {
    Serial.println(F("Timeout: CLK held low by host"));
    return;
  }

  // ensure idle state
  clkHigh();
  dataHigh();
  delayMicroseconds(10);
  clkLow();
  delayMicroseconds(T_FIRST_CLK_LOW);

  // prep bit 0
  if (value & 0x01) {
    dataHigh();
  } else {
    dataLow();
  }
  clkHigh();
  value >>= 1;
  delayMicroseconds(T_CLK_HIGH);
    
  for (uint8_t i = 0; i < 7; i++) {
    clkLow();
    delayMicroseconds(T_CLK_LOW);

    // prep next bit
    if (value & 0x01) {
      dataHigh();
    } else {
      dataLow();
    }
    clkHigh();
    value >>= 1;
    delayMicroseconds(T_CLK_HIGH);
  }

  clkLow();
  delayMicroseconds(T_CLK_LOW);

  // DATA goes LOW for post-transmission hold
  dataLow();
  clkHigh();
    
  if (isBreak) {
    delayMicroseconds(T_POST_BREAK); // 500us for break codes
  } else {
    delayMicroseconds(T_POST_MAKE); // 640us for make codes
  }

  // return to idle
  dataHigh();
  delayMicroseconds(T_INTER_BYTE);
}

// wrappers
void xt_make(uint8_t scancode) {
    if (scancode == SC_NONE || scancode == SC_FN) return;
    xt_write(scancode, false);
}

void xt_break(uint8_t scancode) {
    if (scancode == SC_NONE || scancode == SC_FN) return;
    xt_write(scancode | 0x80, true);
}

void initMatrix() {
  for (uint8_t r = 0; r < ROWS; r++) {
    pinMode(rowPins[r], INPUT_PULLUP);
  }
  
  for (uint8_t c = 0; c < COLS; c++) {
    pinMode(colPins[c], INPUT_PULLUP);
  }
  
  for (uint8_t r = 0; r < ROWS; r++) {
    for (uint8_t c = 0; c < COLS; c++) {
      keyState[r][c] = 0;
      lastKeyState[r][c] = 0;
      lastDebounceTime[r][c] = 0;
    }
  }
}

void scanMatrix() {
  uint32_t now = millis();
  
  for (uint8_t r = 0; r < ROWS; r++) {
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], LOW);
    delayMicroseconds(5); // roughly the settle time
    
    for (uint8_t c = 0; c < COLS; c++) {
      uint8_t keyNum = keyMatrix[r][c];
      if (keyNum == 0) continue; // skip NC
      
      uint8_t reading = !digitalRead(colPins[c]);
      if (reading != lastKeyState[r][c]) {
        lastDebounceTime[r][c] = now;
      }
      
      if ((now - lastDebounceTime[r][c]) > DEBOUNCE_MS) {
        // state has been stable enough
        if (reading != keyState[r][c]) {
          keyState[r][c] = reading;
          
          uint8_t scancode;
          if (fnPressed && keyToScancodesFn[keyNum] != SC_NONE) {
            scancode = keyToScancodesFn[keyNum];
          } else {
            scancode = keyToScancode[keyNum];
          }
          
          // deal with fn key
          if (keyNum == 63) {
            fnPressed = reading;
          }
          else if (reading) {
            xt_make(scancode);
          } else {
            xt_break(scancode);
          }
        }
      }
      lastKeyState[r][c] = reading;
    }
    pinMode(rowPins[r], INPUT_PULLUP);
  }
}

void setup() {
  Serial.begin(9600);
  clkHigh();
  dataHigh();
  initMatrix();
  Serial.println(F("Clackpaq Keyboard Matrix Test Firmware"));
  delay(1000);
  xt_write(0xAA, false);
  Serial.println(F("Sent BAT (0xAA)"));
}

void loop() {
  // check if host is holding CLK low for >12.5ms, reset request
  if (!readClk()) {
    delay(15);
    if (!readClk()) {
      Serial.println(F("Host reset"));
      while (!readClk());
      delay(300);
      xt_write(0xAA, false);
    }
  }
  scanMatrix();
}
