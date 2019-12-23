# Arduino_ST7735_STM
Fast STM32 SPI/DMA library for ST7735 1.8" 128x160

YouTube videos:

https://youtu.be/V1KBm99Qagw

https://youtu.be/rtnI4TEeBpA

Significantly optimized for STM32 boards. Supports 36MHz SPI and DMA channel

## Configuration

Use "#define COMPATIBILITY_MODE" - then the library doesn't use DMA

Use "#define CS_ALWAYS_LOW" for LCD boards where CS pin is internally connected to the ground, it gives better performance


## Extra Features
- invertDisplay()
- sleepDisplay()
- enableDisplay()
- idleDisplay() - saves power by limiting colors to 3 bit mode (8 colors)
- resetDisplay() - software reset
- partialDisplay() and setPartArea() - limiting display area for power saving
- setScrollArea() and setScroll() - smooth vertical scrolling
- fast drawImage() from RAM
- fast drawImageF() from flash (PROGMEM)

## Connections (header at the top):

|LCD pin|LCD pin name|STM32|
|--|--|--|
 |#01| LED| 3.3V|
 |#02| SCK |PA5/SCK|
 |#03| SCA |PA7/MOSI|
 |#04| A0/DC|PA1 or any digital
 |#05| RESET|PA0 or any digital|
 |#06| CS|PA2 or any digital|
 |#07| GND | GND|
 |#08| VCC | 3.3V|
 
 Tested with stm32duino and Arduino IDE 1.6.5
 
