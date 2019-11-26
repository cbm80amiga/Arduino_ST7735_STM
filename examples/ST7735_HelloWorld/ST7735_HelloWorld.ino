// ST7735 library example
// (c) 2019 Pawel A. Hernik

/*
 ST7735 128x160 1.8" LCD pinout (header at the top, from left):
 #1 LED   -> 3.3V
 #2 SCK   -> SCL/D13/PA5
 #3 SDA   -> MOSI/D11/PA7
 #4 A0/DC -> D8/PA1  or any digital
 #5 RESET -> D9/PA0  or any digital
 #6 CS    -> D10/PA2 or any digital
 #7 GND   -> GND
 #8 VCC   -> 3.3V
*/

#define SCR_WD   128
#define SCR_HT   160
#include <SPI.h>
#include <Adafruit_GFX.h>

#if (__STM32F1__) // bluepill
#define TFT_CS  PA2
#define TFT_DC  PA1
#define TFT_RST PA0
//#include <Arduino_ST7735_STM.h>
#else
#define TFT_CS 10
#define TFT_DC  8
#define TFT_RST 9
#include <Arduino_ST7735_Fast.h>
#endif

Arduino_ST7735 lcd = Arduino_ST7735(TFT_DC, TFT_RST, TFT_CS);

void setup(void) 
{
  Serial.begin(9600);
  lcd.init();
  lcd.fillScreen(BLACK);
  lcd.setTextColor(WHITE,BLUE);
  int xt=(SCR_WD-11*6)/2, yt=(SCR_HT-8)/2;
  lcd.setCursor(xt, yt);
  lcd.println("HELLO WORLD");
  lcd.drawRect(xt-10,yt-10,11*6+20,8+20,GREEN);
 }

void loop()
{
}

