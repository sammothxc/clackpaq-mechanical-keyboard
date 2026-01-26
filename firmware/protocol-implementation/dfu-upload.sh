#!/bin/bash
echo "Flashing..."
sudo dfu-programmer atmega32u4 erase && \
sudo dfu-programmer atmega32u4 flash .pio/build/elite_c/firmware.hex && \
sudo dfu-programmer atmega32u4 reset
echo "Done!"