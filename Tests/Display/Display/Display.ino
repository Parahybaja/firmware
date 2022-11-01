#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /* clock=*/ 18, /* data=*/ 23, /* CS=*/ 5, /* reset=*/ 17);

char timer [5];
char temp [5];
char Speed [5];
const char DEGREE_SYMBOL[] = { 0xB0, '\0' };

void setup(void) {

  u8g2.begin();
  //u8g2.enableUTF8Print();
}

void loop(void) {
  u8g2.firstPage();
  do {
    //draw();
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawStr(0,24,"Hello World!");
  } while (u8g2.nextPage());
}
/*
void draw(){
  u8g2.setFont(u8g2_font_ncenB10_tr);
  // battery 
  u8g2.drawFrame(6,5,16,23);  
  
  u8g2.drawRBox(8,7,14,9,1); //4
  u8g2.drawRBox(8,11,14,13,1); //3
  u8g2.drawRBox(8,15,14,17,1); //2
  u8g2.drawRBox(8,19,14,21,1); //1

  
  // timer --- minutes
  u8g2.drawFrame(9,51,48,60);
  float a = 120;
  dtostrf(a, 3, 0, timer);
  u8g2.drawStr(11, 52, timer);

  // temperature
  float b = 29;
  dtostrf(b, 3, 0, temp);
  u8g2.drawStr(6, 34, temp);
  u8g2.drawUTF8(41, 34, DEGREE_SYMBOL);
  
  // speed
  int c = 39;
  dtostrf(c, 3, 0, Speed);
  u8g2.drawStr(54,28,Speed);
  
  // FUEL
  //----fuel symbol
  u8g2.drawBox(33,10,38,19);
  u8g2.drawRBox(35,12,16,13,1);
  u8g2.drawPixel(29,14);
  u8g2.drawPixel(32,16);
  u8g2.drawHLine(30,13,3);
  u8g2.drawHLine(30,15,2);
  //----Fuel Level
  u8g2.drawRBox(42,10,95,19,1);

  // RPM

  u8g2.drawStr(97, 54, "RPM");

  u8g2.drawBox(118,2,125,61);
  
}

*/
