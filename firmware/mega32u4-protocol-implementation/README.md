# Protocol Test Keyboard Matrix

Compaq Portable 1/Plus keyboard protocol emulator using a test keyboard matrix

## Hardware

- **MCU:** Elite-C ATmega32U4 (Pro Micro compatible)
- **Matrix:** 7 rows, 10 columns, 67 keys
- **XT Protocol Output:** 
  - CLK: Pin C6
  - DATA: Pin D4

## Pin Mapping

### Rows
| Row | Pin |
|-----|-----|
| 1 | F7 (A0) |
| 2 | B1 (15) |
| 3 | B2 (16) |
| 4 | F5 (A2) |
| 5 | F6 (A1) |
| 6 | F4 (A3) |
| 7 | B3 (14) |

### Columns
| Col | Pin |
|-----|-----|
| 1 | E6 (7) |
| 2 | B4 (8) |
| 3 | B5 (9) |
| 4 | B7 (11) |
| 5 | D5 |
| 6 | C7 (5) |
| 7 | F1 (A4) |
| 8 | F0 (A5) |
| 9 | B6 (10) |
| 10 | B0 |

### XT Interface
| Signal | Pin |
|--------|-----|
| CLK | C6 (5) |
| DATA | D4 (4) |
| GND | GND |


## Keymap

### Base Layer
Standard QWERTY layout with:
- Arrow keys (Up/Down/Left/Right)
- Home, End, Page Up, Page Down
- Function key (Fn) for layer switching

### Function Layer (Fn+)
- Fn + 1-0 = F1-F10
- Fn + ~ = Escape  
- Fn + Backspace = Delete
- Fn + Home = Print Screen
- Fn + Caps = Scroll Lock
