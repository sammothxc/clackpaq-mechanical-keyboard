/*
* Protocol Test Keyboard Matrix
* For Raspberry Pi Pico (RP2040)
* 
* Scans a 7x10 key matrix and outputs XT scancodes
* 
* Protocol
*   - Idle: CLK and DATA idle at HIGH via host's 5v pullups
*   - 9 bits: 1 start bit is always HIGH + 8 data bits LSB first
*   - Host reads DATA on CLK falling edge
*   - DATA changes on CLK rising edge
*   - First CLK pulse is longer, 42us low
*   - Normal CLK: 26us low, 14us high
*   - After transmission, DATA stays LOW for 640us make or 500us break
* 
* Hardware
*   - Transistor based open-collector outputs, inverted logic
*   - GPIO HIGH = transistor ON = line pulled LOW
*   - GPIO LOW = transistor OFF = line released HIGH via host pullup
* 
* License: CC BY-NC-SA 4.0
* https://github.com/sammothxc/clackpaq
*/


#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "config.h"


static uint8_t keyState[ROWS][COLS];
static uint8_t lastKeyState[ROWS][COLS];
static uint32_t lastDebounceTime[ROWS][COLS];
static bool fnPressed = false;


// INVERTED LOGIC due to transistor drivers:
// GPIO HIGH = transistor ON  = line LOW
// GPIO LOW  = transistor OFF = line HIGH via host pullup(?)
static inline void clk_low(void) {
    gpio_put(XT_CLK, 1);
}


static inline void clk_high(void) {
    gpio_put(XT_CLK, 0);
}


static inline void data_low(void) {
    gpio_put(XT_DATA, 1);
}


static inline void data_high(void) {
    gpio_put(XT_DATA, 0);
}


void xt_write(uint8_t value, bool is_break) {
  // idle state = both lines HIGH, transistors off
  clk_high();
  data_high();
  sleep_us(10);
  
  // DATA is already HIGH, host reads start bit = 1
  clk_low();
  sleep_us(T_FIRST_CLK_LOW);
  
  // prep bit 0
  if (value & 0x01) {
    data_high();
  } else {
    data_low();
  }
  clk_high();
  value >>= 1;
  sleep_us(T_CLK_HIGH);
  
  // CLK pulses 2 thru 8, data bits 0 thru 6
  for (int i = 0; i < 7; i++) {
    clk_low();
    sleep_us(T_CLK_LOW);
    
    if (value & 0x01) {
      data_high();
    } else {
      data_low();
    }
    clk_high();
    value >>= 1;
    sleep_us(T_CLK_HIGH);
  }
  
  // CLK pulse 9, data bit 7
  clk_low();
  sleep_us(T_CLK_LOW);
  
  // post-transmission DATA LOW
  data_low();
  clk_high();
  
  if (is_break) {
    sleep_us(T_POST_BREAK);
  } else {
    sleep_us(T_POST_MAKE);
  }
  
  data_high();
  sleep_us(T_INTER_BYTE);
}


void xt_make(uint8_t scancode) {
  if (scancode == SC_NONE || scancode == SC_FN) return;
  xt_write(scancode, false);
}


void xt_break(uint8_t scancode) {
  if (scancode == SC_NONE || scancode == SC_FN) return;
  xt_write(scancode | 0x80, true);
}


void xt_key(uint8_t scancode) {
  xt_make(scancode);
  sleep_ms(10);
  xt_break(scancode);
}


void init_matrix(void) {
  for (int r = 0; r < ROWS; r++) {
    gpio_init(rowPins[r]);
    gpio_set_dir(rowPins[r], GPIO_IN);
    gpio_pull_up(rowPins[r]);
  }
  
  for (int c = 0; c < COLS; c++) {
    gpio_init(colPins[c]);
    gpio_set_dir(colPins[c], GPIO_IN);
    gpio_pull_up(colPins[c]);
  }
  
  for (int r = 0; r < ROWS; r++) {
    for (int c = 0; c < COLS; c++) {
      keyState[r][c] = 0;
      lastKeyState[r][c] = 0;
      lastDebounceTime[r][c] = 0;
    }
  }
}


void scan_matrix(void) {
  uint32_t now = to_ms_since_boot(get_absolute_time());
  
  for (int r = 0; r < ROWS; r++) {
    gpio_set_dir(rowPins[r], GPIO_OUT);
    gpio_put(rowPins[r], 0);
    sleep_us(5); // settle time
    
    for (int c = 0; c < COLS; c++) {
      uint8_t keyNum = keyMatrix[r][c];
      if (keyNum == 0) continue;
      
      uint8_t reading = !gpio_get(colPins[c]);
      
      if (reading != lastKeyState[r][c]) {
          lastDebounceTime[r][c] = now;
      }
      
      if ((now - lastDebounceTime[r][c]) > DEBOUNCE_MS) {
        if (reading != keyState[r][c]) {
          keyState[r][c] = reading;
          
          uint8_t scancode;
          if (fnPressed && keyToScancodesFn[keyNum] != SC_NONE) {
            scancode = keyToScancodesFn[keyNum];
          } else {
            scancode = keyToScancode[keyNum];
          }
          
          if (keyNum == 63) {
            fnPressed = reading;
            gpio_put(LED_PIN, reading); // led = fn state
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
    
    // release row
    gpio_set_dir(rowPins[r], GPIO_IN);
    gpio_pull_up(rowPins[r]);
  }
}


int main(void) {
  stdio_init_all();
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  gpio_put(LED_PIN, 0);
  gpio_init(XT_CLK);
  gpio_init(XT_DATA);
  gpio_set_dir(XT_CLK, GPIO_OUT);
  gpio_set_dir(XT_DATA, GPIO_OUT);
  
  // start with lines HIGH, transistors off
  clk_high();
  data_high();
  
  init_matrix();
  
  printf("Clackpaq RP2040 Keyboard\n");
  printf("CLK: GPIO%d, DATA: GPIO%d\n", XT_CLK, XT_DATA);
  
  // wait for host, then send BAT
  sleep_ms(1000);
  xt_write(0xAA, false);
  printf("Sent BAT (0xAA)\n");
  
  // ready
  for (int i = 0; i < 3; i++) {
    gpio_put(LED_PIN, 1);
    sleep_ms(100);
    gpio_put(LED_PIN, 0);
    sleep_ms(100);
  }
  
  while (true) {
    scan_matrix();
  }
  
  return 0;
}
