#include<Wire.h>
#include<LiquidCrystal_I2C.h>

#define max_rpm 40  // Maximum value of rpm

LiquidCrystal_I2C lcd(0x27, 16, 2); //RPM_Speed_Display
int rpm;

void setup() {
  lcd.init(); // Initializes the interface to the LCD screen
  lcd.backlight();
}
void loop() {
  // Speed
  lcd.setCursor(0,0);
  lcd.print(random(59));
  lcd.print(" km/h");
  // Draws bargraph on the second line of the LCD
  lcd.setCursor(0, 1);
  int grafico = map(random(40),0,max_rpm,0,16);
  for (int nL=0; nL < grafico; nL++)  lcd.print(char(255));
  for (int nL=grafico; nL < 16; nL++) lcd.print(" ");
  delay(500);
}
