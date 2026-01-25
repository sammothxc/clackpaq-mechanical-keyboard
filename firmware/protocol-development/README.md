# Protocol Proof of Concept

This firmware emulates the original Compaq Portable keyboard protocol, allowing you to send keystrokes to a Compaq Portable I or Portable Plus via serial commands.

## Hardware

**MCU:** Arduino Nano (ATmega328P @ 16MHz)
**Connections:**
  - D3 → CLK
  - D2 → DATA
  - GND → GND

## Protocol

Based on IBM PC/XT keyboard protocol with Compaq-specific timing:

- Idle state: CLK=HIGH, DATA=HIGH
- 9 bits per transmission: 1 start bit (always HIGH) + 8 data bits (LSB first)
- Host reads DATA on CLK falling edge
- Timing: ~358µs per byte transmission

## Serial Commands

| Command | Description |
|---------|-------------|
| `BAT` | Send BAT completion code (0xAA) |
| `TEST` | Send 'A' key press and release |
| `SCOPE` | Send Enter make code (for oscilloscope testing) |
| `ENTER` | Send Enter key press and release |
| `MAKE xx` | Send make code (hex) |
| `BREAK xx` | Send break code (hex) |
| `RAW xx` | Send raw byte (hex) |
| `<text>` | Type text and press Enter |
