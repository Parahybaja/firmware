//-----includes------
#include<Wire.h>
#include<LiquidCrystal_I2C.h>

//-----defines------
#define max_rpm 40  // Maximum value of rpm
#define min_rpm 0

#define I2C_adress_1 0x27
#define I2C_adress_2 0x27

//-------displays------------
LiquidCrystal_I2C lcd(I2C_adress_1, 16, 2); //RPM_Speed_Display
LiquidCrystal_I2C lcd2(I2C_adress_2, 16, 2); //Temp_Timer_Battery_Display

//------Bits-----

byte Btemp[8]={B00100,B01010,B01010,B01010,B01010,B10001,B10001,B01110};
byte Btimer[8]={B00000,B01110,B10001,B10101,B10111,B10001,B01110,B00000};
byte Bbattery[8]={B00100,B01100,B11100,B11110,B01111,B00111,B00110,B00100};
byte Bcelcium[8]={B00000,B00110,B00110,B00000,B00000,B00000,B00000,B00000};

void display_setup() {
  //----display initialization----
  lcd.init(); // Initializes the interface to the LCD screen
  lcd.backlight();

  lcd2.init(); // Initializes the interface to the LCD screen
  lcd2.backlight();

  //----byte creation----
  lcd2.createChar(1 , Btemp);         //TEMPERATURE                     
  lcd2.createChar(2 , Btimer);        //TIMER
  lcd2.createChar(3 , Bbattery);      //BATTERY
  lcd2.createChar(4 , Bcelcium);      //CelsiumUnit

  // ------------- Display Speed/RPM -----------
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
  
  // ------------- Display Temp/Battery/Tempo total -----------
  // First Line
  lcd2.setCursor(0,0);
  lcd2.write(1);
  lcd2.print(random(40));
  lcd2.write(4);
  lcd2.print("C");
  lcd2.setCursor(11,0);
  lcd2.write(3);
  lcd2.print(random(100));
  lcd2.print("%");
  // Second Line
  lcd2.setCursor(6, 1);
  lcd2.write(2);
  lcd2.print(random(240));
  lcd2.print("m");
}


void display_RPM(float value){
  lcd.setCursor(0, 1);
  int grafico = map(value,min_rpm,max_rpm,0,16);
  for (int nL=0; nL < grafico; nL++)  lcd.print(char(255));
  for (int nL=grafico; nL < 16; nL++) lcd.print(" ");  
}

void display_speed(float value){
  if(value >= 10){
    lcd.setCursor(0,0);
    lcd.print(value);
    lcd.print(" km/h");
  }  
  else {
    lcd.setCursor(0,0);
    lcd.print(" ");
    lcd.print(value);
    lcd.print(" km/h");
  }
}

void display_temp(float value){
  if(value >= 10){
    lcd2.setCursor(0,0);
    lcd2.write(1);
    lcd2.print(value);
    lcd2.write(4);
    lcd2.print("C");
  }
  else {
    lcd2.setCursor(0,0);
    lcd2.write(1);
    lcd2.print(" ");
    lcd2.print(value);
    lcd2.write(4);
    lcd2.print("C");
  }  
}

void display_timer(float value){
  if(value >= 100){
    lcd2.setCursor(6, 1);
    lcd2.write(2);
    lcd2.print(value);
    lcd2.print("m");
  }
  else if(value >= 10){
    lcd2.setCursor(6, 1);
    lcd2.write(2);
    lcd2.print(" ");
    lcd2.print(value);
    lcd2.print("m");
  }  
  else {
    lcd2.setCursor(6, 1);
    lcd2.write(2);
    lcd2.print("  ");
    lcd2.print(value);
    lcd2.print("m");
  }
}

void display_battery(float value){
  if(value == 100){
    lcd2.setCursor(11,0);
    lcd2.write(3);
    lcd2.print(value);
    lcd2.print("%");
  }
  else if(value >= 10){
    lcd2.setCursor(11,0);
    lcd2.write(3);
    lcd2.print(" ");
    lcd2.print(value);
    lcd2.print("%");
  }  
  else {
    lcd2.setCursor(11,0);
    lcd2.write(3);
    lcd2.print("  ");
    lcd2.print(value);
    lcd2.print("%");
  }
}
