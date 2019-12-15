// ST7735 library axample
// MultiLCD Color RTC Clock
// Requires RRE font library
// (C)2019 Pawel A. Hernik
// YouTube video: https://youtu.be/tBNnuXBP5rg 

/*
 ST7735 128x160 1.8" LCD pinout (header at the top, from left):
 #1 LED   -> 3.3V
 #2 SCK   -> SCL/PA5
 #3 SDA   -> MOSI/PA7
 #4 A0/DC -> PA1  or any digital
 #5 RESET -> PA0  or any digital
 #6 CS    -> PA2/PA3/PA4/PB0 or any digital
 #7 GND   -> GND
 #8 VCC   -> 3.3V
*/

#include <SPI.h>
#include <Adafruit_GFX.h>

#if (__STM32F1__) // bluepill
#define TFT_CS1  PA2
#define TFT_CS2  PA3
#define TFT_CS3  PA4
#define TFT_CS4  PB0
#define TFT_DC   PA1
#define TFT_RST  PA0
#include <Arduino_ST7735_STM.h>
#else
#define TFT_CS1 7
#define TFT_CS2 6
#define TFT_CS3 5
#define TFT_CS4 4
#define TFT_DC  8
#define TFT_RST 9
//#include <Arduino_ST7735_Fast.h>
#endif

#define SCR_WD 128
#define SCR_HT 160
Arduino_ST7735 lcd1 = Arduino_ST7735(TFT_DC, TFT_RST, TFT_CS1); // only 1 reset line
Arduino_ST7735 lcd2 = Arduino_ST7735(TFT_DC, -1, TFT_CS2);
Arduino_ST7735 lcd3 = Arduino_ST7735(TFT_DC, -1, TFT_CS3);
Arduino_ST7735 lcd4 = Arduino_ST7735(TFT_DC, -1, TFT_CS4);

static Arduino_ST7735 *lcd = &lcd1;  // current LCD for RRE fillRect callbacks

#include <RTClock.h>
RTClock rtclock(RTCSEL_LSE);  // RTC set to LSE 32768Hz clock, counts seconds
tm_t curTime;

uint8_t txt2num(const char* p) 
{
  return 10*(*p-'0') + *(p+1)-'0';
}

uint8_t hh = txt2num(__TIME__+0);
uint8_t mm = txt2num(__TIME__+3);
uint8_t ss = txt2num(__TIME__+6);

#include "RREFont.h"
#include "rre_bold13x20h.h"
#include "rre_kx16x26h.h"
#include "rre_kx9x14h.h"
#include "rre_arialdig72nh.h"
#include "rre_arialdig150b.h"

#include "pat1.h"
#include "pat2.h"
#include "pat3.h"
#include "pat4.h"
#include "pat5.h"
#include "pat6.h"
#include "pat7.h"
#include "pat8.h"

RREFont font;

// -------------------------
// regular 1 color fillRect
void customRect(int x, int y, int w, int h, int c) { lcd->fillRect(x, y, w, h, c); }
// -------------------------
int yy0,colH=13;
int wheelStart = 0, wheelMul = 1;
void setWheel(int st, int mul)
{
  wheelStart = st;
  wheelMul = mul;
}
// RGB wheel rainbow fillRect
void customRectWheel(int x, int y, int w, int h, int c)
{
  int yy = y-yy0;
  for(int i=0;i<h;i++) lcd->fillRect(x, y+i, w, 1, lcd->rgbWheel(wheelStart+(yy+i)*wheelMul)); 
}
// -------------------------
uint8_t col1R=0,col1G=130,col1B=130;
uint8_t col2R=0,col2G=240,col2B=200;
uint8_t col3R=0,col3G=130,col3B=130;

void set2Cols(int r1,int g1,int b1,int r2,int g2,int b2,int h)
{
  col1R=r1,col1G=g1,col1B=b1;
  col2R=r2,col2G=g2,col2B=b2; 
  colH=h;
}

void set3Cols(int r1,int g1,int b1,int r2,int g2,int b2,int r3,int g3,int b3,int h)
{
  col1R=r1,col1G=g1,col1B=b1;
  col2R=r2,col2G=g2,col2B=b2; 
  col3R=r3,col3G=g3,col3B=b3; 
  colH=h;
}

// smooth transition between 2 colors (top/bottom)
void customRect2Cols(int x, int y, int w, int h, int c)
{
  int yy = y-yy0;
  uint16_t cc;
  for(int i=0;i<h;i++) {
    cc = RGBto565(col1R+(col2R-col1R)*(yy+i)/colH,col1G+(col2G-col1G)*(yy+i)/colH,col1B+(col2B-col1B)*(yy+i)/colH);
    lcd->fillRect(x, y+i, w, 1, cc); 
  }
}

// smooth transition between 3 colors (top/middle/bottom)
void customRect3Cols(int x, int y, int w, int h, int c)
{
  int yy,h2 = colH/2;
  uint16_t cc;
  for(int i=0;i<h;i++) {
    yy = y-yy0+i;
    if(yy<h2)
      cc = RGBto565(col1R+(col2R-col1R)*yy/h2, col1G+(col2G-col1G)*yy/h2, col1B+(col2B-col1B)*yy/h2);
    else
      cc = RGBto565(col2R+(col3R-col2R)*(yy-colH/2)/h2, col2G+(col3G-col2G)*(yy-colH/2)/h2, col2B+(col3B-col2B)*(yy-colH/2)/h2);
    lcd->fillRect(x, y+i, w, 1, cc); 
  }
}
// -------------------------
const uint16_t *fontPat = pat6+3;
// RRE font filled with RGB pattern
void customRectPat(int x, int y, int w, int h, int c)
{
  uint16_t pat[128];
  for(int j=0;j<h;j++) {
    for(int i=0;i<w;i++) pat[i]=pgm_read_word(&fontPat[((i+x)&0x1f)+((j+y)&0x1f)*32]);
    lcd->drawImage(x, y+j, w, 1, pat); 
  }
}
// -------------------------

char buf[100];
unsigned long cnt;

// simplified outline, requires only 4 draws
void printOutline(int x, int y, char *txt, int mode=0, int outl=1)
{
  font.setFillRectFun(customRect);
  if(outl>0) {
    font.printStr(x-outl,y-outl,txt);
    font.printStr(x+outl,y-outl,txt);
    font.printStr(x-outl,y+outl,txt);
    font.printStr(x+outl,y+outl,txt);
  }
  if((mode&7)==0) font.setFillRectFun(customRectPat); else
  if((mode&7)==1) font.setFillRectFun(customRectWheel); else
  if((mode&7)==2) font.setFillRectFun(customRect2Cols); else
  if((mode&7)==3) font.setFillRectFun(customRect3Cols);
  font.printStr(x,y,txt);
}

void fillPatternAll(const uint16_t *pat)
{
  for(int j=0;j<SCR_HT;j+=32) for(int i=0;i<SCR_WD;i+=32) {
    lcd1.drawImageF(i,j,pat);
    lcd2.drawImageF(i,j,pat);
    lcd3.drawImageF(i,j,pat);
    lcd4.drawImageF(i,j,pat);
  }
}

void fillPattern(const uint16_t *pat)
{
  for(int j=0;j<SCR_HT;j+=32) for(int i=0;i<SCR_WD;i+=32) lcd->drawImageF(i,j,pat);
}

// --------------------------------------------------------------------------
const char *months[] = {"???", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
const char *delim = " :";
char bld[40];

uint8_t str2month(const char * d)
{
  uint8_t i = 13;
  while((--i) && strcmp(months[i], d));
  return i;
}

void setBuildTime(struct tm_t & mt)
{
  // Timestamp format: "Mar 3 2019 12:34:56"
  snprintf(bld,40,"%s %s\n", __DATE__, __TIME__);
  char *token = strtok(bld, delim);
  while(token) {
    int m = str2month((const char*)token);
    if(m>0) {
      mt.month = m;
      token = strtok(NULL, delim);  mt.day = atoi(token);
      token = strtok(NULL, delim);  mt.year = atoi(token) - 1970;
      token = strtok(NULL, delim);  mt.hour = atoi(token);
      token = strtok(NULL, delim);  mt.minute = atoi(token);
      token = strtok(NULL, delim);  mt.second = atoi(token);
    }
    token = strtok(NULL, delim);
  }
  //snprintf(bld,40,"Build: %02d-%02d-%02d %02d:%02d:%02d\n",mt.year+1970,mt.month,mt.day,mt.hour,mt.minute,mt.second); Serial.println(bld);
  rtclock.setTime(rtclock.makeTime(mt)+10);
}
//-----------------------------------------------------------------------------

void setup() 
{
  //Serial.begin(9600);
  lcd1.init();
  lcd2.init();
  lcd3.init();
  lcd4.init();
  lcd1.fillScreen(RED);
  lcd2.fillScreen(GREEN);
  lcd3.fillScreen(BLUE);
  lcd4.fillScreen(MAGENTA);
  //delay(1000);
  font.init(customRect, SCR_WD, SCR_HT); // custom fillRect function and screen width and height values
  font.setScale(1,1);

  rtclock.breakTime(rtclock.now(), curTime);
  if(curTime.year+1970<2019) setBuildTime(curTime);  //  <2019 - invalid year
}

void printStyle(int x, int y, int style)
{
  switch(style) {
    case 0: // RGB wheel
      setWheel(170,3);
      printOutline(x,y,buf,1);
      break;
    case 1: // cyan bright top
      set2Cols(0,230,230, 0,40,40, 146);
      printOutline(x,y,buf,2);
      break;
    case 2: // yellow-red
      set2Cols(240,240,0, 200,0,0, 146);
      printOutline(x,y,buf,2);
      break;
    case 3: // green-yellow
      set2Cols(0,240,0, 250,0,0, 146);
      printOutline(x,y,buf,2);
      break;
    case 4: // cyan
      set3Cols(0,90,90, 0,240,200, 0,90,90, 146);
      printOutline(x,y,buf,3);
      break;
    case 5: // red-yellow-red
      set3Cols(210,0,0, 250,250,0, 210,0,0, 146);
      printOutline(x,y,buf,3);
      break;
    case 6: // green-yellow-red
      set3Cols(0,190,0, 220,220,0, 190,0,0, 146);
      printOutline(x,y,buf,3);
      break;
    case 7:
      fontPat = pat1+3;
      printOutline(x,y,buf,0);
      break;
    case 8:
      fontPat = pat6+3;
      printOutline(x,y,buf,0);
      break;
    case 9:
      fontPat = pat5+3;
      printOutline(x,y,buf,0);
      break;
  }
}

int i=1;

void loop()
{
  rtclock.breakTime(rtclock.now(), curTime);
  hh=curTime.hour;
  mm=curTime.minute;
  ss=curTime.second;
  font.setColor(BLACK);
  yy0 = 6;
  font.setFont(&rre_ArialDig150b); font.setDigitMinWd(103); font.setCharMinWd(20);   font.setScale(1);

  buf[0] = '0'+hh/10;
  lcd = &lcd1;
  //fillPattern(i&1 ? pat2 : pat7); // no room for pat7 in 64KB version of STM32
  fillPattern(i&1 ? pat2 : pat5);
  printStyle(12,yy0,i);
  
  buf[0] = '0'+hh%10;
  lcd = &lcd2;
  fillPattern(i&1 ? pat2 : pat5);
  printStyle(12,yy0,i);

  buf[0] = '0'+mm/10;
  lcd = &lcd3;
  fillPattern(i&1 ? pat2 : pat5);
  printStyle(12,yy0,i);

  buf[0] = '0'+mm%10;
  lcd = &lcd4;
  fillPattern(i&1 ? pat2 : pat5);
  printStyle(12,yy0,i);

  delay(6000);
  if(++i>9) i=0;
}

