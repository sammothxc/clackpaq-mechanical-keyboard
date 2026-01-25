/*
* Protocol Proof of Concept
* For Arduino Nano (ATmega328P)
* 
* Based on oscilloscope captures and IBM 5155 Technical Reference
*
* License: CC BY-NC-SA 4.0
* https://github.com/sammothxc/clackpaq
*/

#include <Arduino.h>

#define XT_CLK  3
#define XT_DATA 2

// timing constants in microseconds
// adjusted for arduino overhead, target 358us total transmission
// digitalWrite/pinMode adds ~4-5us overhead per call, wish I could bitbang this stuff
#define T_FIRST_CLK_LOW   28    // first CLK pulse low time (target 42us with overhead)
#define T_CLK_LOW         14    // normal CLK low time (target 26us with overhead)
#define T_CLK_HIGH         7    // CLK high time (target 14us with overhead)
#define T_POST_MAKE      640    // DATA stays low after make code
#define T_POST_BREAK     500    // DATA stays low after break code
#define T_INTER_BYTE     200    // pause between bytes

// XT scan codes for standard ASCII characters
const uint8_t asciiToXt[128] = {
  // 0x00-0x0F: control characters
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0x0E, 0x0F, 0x1C, 0xFF, 0xFF, 0x1C, 0xFF, 0xFF,  // BS=0x0E, TAB=0x0F, LF/CR=0x1C

  // 0x10-0x1F: more control characters  
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF,  // ESC=0x01

  // 0x20-0x2F: space and punctuation
  0x39, 0x02, 0x28, 0x04, 0x05, 0x06, 0x08, 0x28,  // SP ! " # $ % & '
  0x0A, 0x0B, 0x09, 0x0D, 0x33, 0x0C, 0x34, 0x35,  // ( ) * + , - . /

  // 0x30-0x39: numbers 0-9
  0x0B, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,

  // 0x3A-0x40: more punctuation
  0x27, 0x27, 0x33, 0x0D, 0x34, 0x35, 0x03,        // : ; < = > ? @

  // 0x41-0x5A: uppercase A-Z
  0x1E, 0x30, 0x2E, 0x20, 0x12, 0x21, 0x22, 0x23, 0x17, 0x24,
  0x25, 0x26, 0x32, 0x31, 0x18, 0x19, 0x10, 0x13, 0x1F, 0x14,
  0x16, 0x2F, 0x11, 0x2D, 0x15, 0x2C,

  // 0x5B-0x60: even more punctuation
  0x1A, 0x2B, 0x1B, 0x07, 0x0C, 0x29,              // [ \ ] ^ _ `

  // 0x61-0x7A: lowercase a-z (same scancodes as uppercase)
  0x1E, 0x30, 0x2E, 0x20, 0x12, 0x21, 0x22, 0x23, 0x17, 0x24,
  0x25, 0x26, 0x32, 0x31, 0x18, 0x19, 0x10, 0x13, 0x1F, 0x14,
  0x16, 0x2F, 0x11, 0x2D, 0x15, 0x2C,

  // 0x7B-0x7F: lastly, punctuation
  0x1A, 0x2B, 0x1B, 0x29, 0x0E                     // { | } ~ DEL
};

// chars that require shift key
const bool needsShift[128] = {
  // 0x00-0x1F: control chars (no shift)
  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,

  // 0x20-0x2F: space ! " # $ % & ' ( ) * + , - . /
  0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0,

  // 0x30-0x3F: 0-9 : ; < = > ?
  0,0,0,0,0,0,0,0,0,0, 1, 0, 1, 0, 1, 1,

  // 0x40-0x5A: @ and uppercase A-Z
  1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,

  // 0x5B-0x60: [ \ ] ^ _ `
  0, 0, 0, 1, 1, 0,

  // 0x61-0x7A: lowercase a-z
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

  // 0x7B-0x7F: { | } ~
  1, 1, 1, 1, 0
};

#define XT_SHIFT 0x2A

void xt_write(uint8_t value, bool isBreak);
void xt_make(uint8_t scancode);
void xt_break(uint8_t scancode);
void xt_key(uint8_t scancode);
void sendChar(char c);
void sendString(const char* str);

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
  xt_write(scancode, false);
}

void xt_break(uint8_t scancode) {
  xt_write(scancode | 0x80, true);
}

void xt_key(uint8_t scancode) {
  xt_make(scancode);
  delay(10);
  xt_break(scancode);
}

// send char as XT scancode
void sendChar(char c) {
  if (c < 0 || c > 127) return;

  uint8_t scancode = asciiToXt[(uint8_t)c];
  if (scancode == 0xFF) return;

  bool shift = needsShift[(uint8_t)c];

  if (shift) {
    xt_make(XT_SHIFT);
  }

  xt_key(scancode);

  if (shift) {
    xt_break(XT_SHIFT);
  }
}

void sendString(const char* str) {
  while (*str) {
    sendChar(*str++);
    delay(10);
  }
}

String inputBuffer = "";

void setup() {
  Serial.begin(9600);
  clkHigh();
  dataHigh();
  Serial.println(F("Clackpaq Compaq Portable XT Protocol Tester"));
  Serial.println(F("Idle: CLK=HIGH, DATA=HIGH"));
  Serial.println(F("Commands: BAT, RAW xx, MAKE xx, BREAK xx, TEST, SCOPE"));
  Serial.println(F("Or type text and press Enter"));
  Serial.println();
  delay(1000); // POR delay
  xt_write(0xAA, false); // BAT completion code
  Serial.println(F("Sent BAT (0xAA) - keyboard ready"));
}

void loop() {
  // check if host is holding CLK low for >12.5ms, reset request
  if (!readClk()) {
    delay(15);
    if (!readClk()) {
      Serial.println(F("Host reset detected"));
      while (!readClk()); // wait for release
      delay(300); // simulate BAT time
      xt_write(0xAA, false);
      Serial.println(F("Sent BAT (0xAA)"));
    }
  }

  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {
      inputBuffer.trim();

      if (inputBuffer.length() > 0) {
        if (inputBuffer.equalsIgnoreCase("BAT")) {
          xt_write(0xAA, false);
          Serial.println(F("Sent BAT (0xAA)"));
        }
        else if (inputBuffer.startsWith("RAW ") || inputBuffer.startsWith("raw ")) {
          String hexStr = inputBuffer.substring(4);
          hexStr.trim();
          uint8_t val = strtol(hexStr.c_str(), NULL, 16);
          bool isBreak = (val & 0x80) != 0;
          xt_write(val, isBreak);
          Serial.print(F("Sent raw: 0x"));
          Serial.println(val, HEX);
        }
        else if (inputBuffer.startsWith("MAKE ") || inputBuffer.startsWith("make ")) {
          String hexStr = inputBuffer.substring(5);
          hexStr.trim();
          uint8_t val = strtol(hexStr.c_str(), NULL, 16);
          xt_make(val);
          Serial.print(F("Sent make: 0x"));
          Serial.println(val, HEX);
        }
        else if (inputBuffer.startsWith("BREAK ") || inputBuffer.startsWith("break ")) {
          String hexStr = inputBuffer.substring(6);
          hexStr.trim();
          uint8_t val = strtol(hexStr.c_str(), NULL, 16);
          xt_break(val);
          Serial.print(F("Sent break: 0x"));
          Serial.println(val | 0x80, HEX);
        }
        else if (inputBuffer.equalsIgnoreCase("TEST")) {
          Serial.println(F("Sending 'A' (make 0x1E, break 0x9E)"));
          xt_key(0x1E);
          Serial.println(F("Done"));
        }
        else if (inputBuffer.equalsIgnoreCase("SCOPE")) {
          Serial.println(F("Sending Enter make (0x1C) for scope"));
          xt_make(0x1C);
          Serial.println(F("Done - compare with your reference!"));
        }
        else if (inputBuffer.equalsIgnoreCase("ENTER")) {
          Serial.println(F("Sending Enter key"));
          xt_key(0x1C);
          Serial.println(F("Done"));
        }
        else {
          Serial.print(F("Sending: "));
          Serial.println(inputBuffer);
          sendString(inputBuffer.c_str());
          xt_key(0x1C);
          Serial.println(F("Done"));
        }
      }
      inputBuffer = "";
    }
    else {
      inputBuffer += c;
    }
  }
}
