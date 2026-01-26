/*
* Protocol Test Keyboard Matrix
* For Elite-C Pro Micro (ATmega32U4)
*
* Configuration Header
*
* License: CC BY-NC-SA 4.0
* https://github.com/sammothxc/clackpaq
*/

#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>

#define XT_CLK  5 // C6
#define XT_DATA 4 // D4
#define ROWS 7
#define COLS 10

const uint8_t rowPins[ROWS] = {
    18, // F7, row 1
    15, // B1, row 2
    16, // B2, row 3
    20, // F5, row 4
    19, // F6, row 5
    21, // F4, row 6
    14  // B3, row 7
};

const uint8_t colPins[COLS] = {
    7,   // E6, col 1
    8,   // B4, col 2
    9,   // B5, col 3
    11,  // B7, col 4?
    TXLED0,  // D5, col 5?
    13,   // C7, col 6?
    A4,  // F1, col 7?
    A5,  // F0, col 8?
    10,  // B6, col 9
    RXLED1   // B0, col 10?
};

// 0 = no key (NC)
const uint8_t keyMatrix[ROWS][COLS] = {
// col 1 col 2 col 3 col 4 col 5 col 6 col 7 col 8 col 9 col 10
    {  2,    3,    4,    5,    6,    7,    1,    16,   31,   45  }, // row 1
    {  8,    9,    10,   11,   12,   13,   59,   60,   61,   62  }, // row 2
    {  17,   18,   19,   20,   21,   22,   14,   29,   43,   56  }, // row 3
    {  23,   24,   25,   26,   27,   28,   15,   30,   49,   58  }, // row 4
    {  32,   33,   34,   35,   36,   37,   63,   64,   65,   66  }, // row 5
    {  38,   39,   40,   41,   42,   0,    0,    0,    67,   57  }, // row 6
    {  46,   47,   48,   44,   50,   51,   52,   53,   54,   55  }  // row 7
};

// XT scancodes
#define SC_ESC      0x01
#define SC_1        0x02
#define SC_2        0x03
#define SC_3        0x04
#define SC_4        0x05
#define SC_5        0x06
#define SC_6        0x07
#define SC_7        0x08
#define SC_8        0x09
#define SC_9        0x0A
#define SC_0        0x0B
#define SC_MINUS    0x0C
#define SC_EQUAL    0x0D
#define SC_BKSP     0x0E
#define SC_TAB      0x0F
#define SC_Q        0x10
#define SC_W        0x11
#define SC_E        0x12
#define SC_R        0x13
#define SC_T        0x14
#define SC_Y        0x15
#define SC_U        0x16
#define SC_I        0x17
#define SC_O        0x18
#define SC_P        0x19
#define SC_LBRACE   0x1A
#define SC_RBRACE   0x1B
#define SC_ENTER    0x1C
#define SC_CTRL     0x1D
#define SC_A        0x1E
#define SC_S        0x1F
#define SC_D        0x20
#define SC_F        0x21
#define SC_G        0x22
#define SC_H        0x23
#define SC_J        0x24
#define SC_K        0x25
#define SC_L        0x26
#define SC_SEMI     0x27
#define SC_QUOTE    0x28
#define SC_GRAVE    0x29
#define SC_LSHIFT   0x2A
#define SC_BSLASH   0x2B
#define SC_Z        0x2C
#define SC_X        0x2D
#define SC_C        0x2E
#define SC_V        0x2F
#define SC_B        0x30
#define SC_N        0x31
#define SC_M        0x32
#define SC_COMMA    0x33
#define SC_DOT      0x34
#define SC_SLASH    0x35
#define SC_RSHIFT   0x36
#define SC_PRTSC    0x37
#define SC_ALT      0x38
#define SC_SPACE    0x39
#define SC_CAPS     0x3A
#define SC_F1       0x3B
#define SC_F2       0x3C
#define SC_F3       0x3D
#define SC_F4       0x3E
#define SC_F5       0x3F
#define SC_F6       0x40
#define SC_F7       0x41
#define SC_F8       0x42
#define SC_F9       0x43
#define SC_F10      0x44
#define SC_NUMLOCK  0x45
#define SC_SCRLOCK  0x46
#define SC_HOME     0x47
#define SC_UP       0x48
#define SC_PGUP     0x49
#define SC_KP_MINUS 0x4A
#define SC_LEFT     0x4B
#define SC_KP_5     0x4C
#define SC_RIGHT    0x4D
#define SC_KP_PLUS  0x4E
#define SC_END      0x4F
#define SC_DOWN     0x50
#define SC_PGDN     0x51
#define SC_INS      0x52
#define SC_DEL      0x53

// special key markers
#define SC_NONE     0x00
#define SC_FN       0xFE  // fn layer toggle

// test keyboard number to XT scancode mapping
// index = key 1-67, value = XT scancode
const uint8_t keyToScancode[68] = {
    SC_NONE,    // 0 - not used
    SC_GRAVE,   // Key 1:  ~ / `
    SC_1,       // Key 2:  1
    SC_2,       // Key 3:  2
    SC_3,       // Key 4:  3
    SC_4,       // Key 5:  4
    SC_5,       // Key 6:  5
    SC_6,       // Key 7:  6
    SC_7,       // Key 8:  7
    SC_8,       // Key 9:  8
    SC_9,       // Key 10: 9
    SC_0,       // Key 11: 0
    SC_MINUS,   // Key 12: -
    SC_EQUAL,   // Key 13: =
    SC_BKSP,    // Key 14: Backspace
    SC_HOME,    // Key 15: Home
    SC_TAB,     // Key 16: Tab
    SC_Q,       // Key 17: Q
    SC_W,       // Key 18: W
    SC_E,       // Key 19: E
    SC_R,       // Key 20: R
    SC_T,       // Key 21: T
    SC_Y,       // Key 22: Y
    SC_U,       // Key 23: U
    SC_I,       // Key 24: I
    SC_O,       // Key 25: O
    SC_P,       // Key 26: P
    SC_LBRACE,  // Key 27: [
    SC_RBRACE,  // Key 28: ]
    SC_BSLASH,  // Key 29: Backslash
    SC_END,     // Key 30: End
    SC_CAPS,    // Key 31: Caps Lock
    SC_A,       // Key 32: A
    SC_S,       // Key 33: S
    SC_D,       // Key 34: D
    SC_F,       // Key 35: F
    SC_G,       // Key 36: G
    SC_H,       // Key 37: H
    SC_J,       // Key 38: J
    SC_K,       // Key 39: K
    SC_L,       // Key 40: L
    SC_SEMI,    // Key 41: ;
    SC_QUOTE,   // Key 42: '
    SC_ENTER,   // Key 43: Enter
    SC_PGUP,    // Key 44: Page Up
    SC_LSHIFT,  // Key 45: Left Shift
    SC_Z,       // Key 46: Z
    SC_X,       // Key 47: X
    SC_C,       // Key 48: C
    SC_V,       // Key 49: V
    SC_B,       // Key 50: B
    SC_N,       // Key 51: N
    SC_M,       // Key 52: M
    SC_COMMA,   // Key 53: ,
    SC_DOT,     // Key 54: .
    SC_SLASH,   // Key 55: /
    SC_RSHIFT,  // Key 56: Right Shift
    SC_UP,      // Key 57: Up Arrow
    SC_PGDN,    // Key 58: Page Down
    SC_CTRL,    // Key 59: Left Ctrl
    SC_NONE,    // Key 60: Meta (no XT equivalent, could map to something)
    SC_ALT,     // Key 61: Alt
    SC_SPACE,   // Key 62: Space
    SC_FN,      // Key 63: Fn
    SC_CTRL,    // Key 64: Right Ctrl (same as left on XT)
    SC_LEFT,    // Key 65: Left Arrow
    SC_DOWN,    // Key 66: Down Arrow
    SC_RIGHT    // Key 67: Right Arrow
};

// fn layer scancodes
const uint8_t keyToScancodesFn[68] = {
    SC_NONE,    // 0
    SC_ESC,     // Key 1:  Fn+~ = Esc
    SC_F1,      // Key 2:  Fn+1 = F1
    SC_F2,      // Key 3:  Fn+2 = F2
    SC_F3,      // Key 4:  Fn+3 = F3
    SC_F4,      // Key 5:  Fn+4 = F4
    SC_F5,      // Key 6:  Fn+5 = F5
    SC_F6,      // Key 7:  Fn+6 = F6
    SC_F7,      // Key 8:  Fn+7 = F7
    SC_F8,      // Key 9:  Fn+8 = F8
    SC_F9,      // Key 10: Fn+9 = F9
    SC_F10,     // Key 11: Fn+0 = F10
    SC_NONE,    // Key 12: Fn+- = F11 (no F11 on XT)
    SC_NONE,    // Key 13: Fn+= = F12 (no F12 on XT)
    SC_DEL,     // Key 14: Fn+Bksp = Delete
    SC_PRTSC,   // Key 15: Fn+Home = Print Screen
    SC_NONE,    // Key 16-30: same as base
    SC_NONE, SC_NONE, SC_NONE, SC_NONE, SC_NONE, SC_NONE,
    SC_NONE, SC_NONE, SC_NONE, SC_NONE, SC_NONE, SC_NONE, SC_NONE, SC_NONE,
    SC_SCRLOCK, // Key 31: Fn+Caps = Scroll Lock
    SC_NONE, SC_NONE, SC_NONE, SC_NONE, SC_NONE, SC_NONE,  // 32-37
    SC_NONE, SC_NONE, SC_NONE, SC_NONE, SC_NONE, SC_NONE, SC_NONE, SC_NONE,  // 38-45
    SC_NONE, SC_NONE, SC_NONE, SC_NONE, SC_NONE, SC_NONE, SC_NONE, SC_NONE,  // 46-53
    SC_NONE, SC_NONE, SC_NONE, SC_NONE, SC_NONE, SC_NONE, SC_NONE, SC_NONE,  // 54-61
    SC_NONE, SC_NONE, SC_NONE, SC_NONE, SC_NONE, SC_NONE   // 62-67
};

// timing constants in microseconds
#define T_FIRST_CLK_LOW   28
#define T_CLK_LOW         14
#define T_CLK_HIGH         7
#define T_POST_MAKE      640
#define T_POST_BREAK     500
#define T_INTER_BYTE     200
#define DEBOUNCE_MS        5 // ms

#endif // CONFIG_H