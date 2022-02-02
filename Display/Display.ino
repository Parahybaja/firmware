//LIBRARIES
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <queue.h>
#include <semphr.h>

//DISPLAY
LiquidCrystal_I2C lcd(0x27,20,4); 

//QUEUES
QueueHandle_t xQueue_LCD;

//TASKS
void task_lcd( void *pvParameters );
void task_readind( void *pvParameters );

//AUXILIARY VARIABLES
int batMin = 0;
int batMax = 100;
int rpmMin = 0;
int rpmMax = 3000;
float fuelMin = 0.0;
float fuelMax = 3.0;
unsigned long millis_task = millis();

//SPECIAL CHARACTERS
byte Fuel_Simbol[8] = {
  B00000,
  B11111,
  B10001,
  B10001,
  B11111,
  B11111,
  B00000,
  B11111
};

//FOR THE FUEL GAUGE
byte Fuel_1[8] = {
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};
byte Fuel_2[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111
};
byte Fuel_3[8] = {
  B00000,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};
byte Fuel_4[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111,
  B11111
};
byte Fuel_5[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111
};
byte Fuel_6[8] = {
  B00000,
  B00000,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};
byte Fuel_7[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111
};
byte Fuel_8[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111
};

//FOR THE BATTERY
byte bat1[8] = {
  B11100,
  B11100,
  B11111,
  B11111,
  B11111,
  B11111,
  B11100,
  B11100
};
byte bat2[8] = {
  B11100,
  B11100,
  B11111,
  B11101,
  B11101,
  B11111,
  B11100,
  B11100
};
byte bat3[8] = {
  B11100,
  B10100,
  B10111,
  B10001,
  B10001,
  B10111,
  B10100,
  B11100
};
byte bat4[8] = {
  B11100,
  B00100,
  B00111,
  B00001,
  B00001,
  B00111,
  B00100,
  B11100
};
byte bat5[8] = {
  B11111,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11111
};
byte bat6[8] = {
  B11111,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11111
};
byte bat7[8] = {
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111
};
byte bat8[8] = {
  B11111,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11111
};
byte bat9[8] = {
  B11111,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11111
};
byte bat10[8] = {
  B11111,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B11111
};

//READING TASK
void task_readind( void *pvParameters ){

  (void) pvParameters;
  
  //SIMULATION VALUES
  int battery = 60;
  float fuel = 0.5;
  int reserve = 1;
  int mode_display = 1;
  int velocity = 16;
  int mileage = 78;
  int operating_time = 29;
  int range = 35;
  int rpm = 3000;
     
  while(1){                     
    xQueueSend(xQueue_LCD, battery);
    xQueueSend(xQueue_LCD, fuel);
    xQueueSend(xQueue_LCD, reserve);
    xQueueSend(xQueue_LCD, mode_display);
    xQueueSend(xQueue_LCD, velocity);
    xQueueSend(xQueue_LCD, mileage);
    xQueueSend(xQueue_LCD, operating_time);
    xQueueSend(xQueue_LCD, range); 
    xQueueSend(xQueue_LCD, rpm);    
  }
}

//DISPLAY TASK
void task_lcd(void *arg) {

  (void) pvParameters;

  int battery;
  float fuel;
  int reserve;
  int mode_display;
  int velocity;
  int mileage;
  int operating_time;
  int range;
  int rpm;
  
  while(1){

    xQueueReceive(xQueue_LCD, (void *)&battery, portMAX_DELAY);
    xQueueReceive(xQueue_LCD, (void *)&fuel, portMAX_DELAY);
    xQueueReceive(xQueue_LCD, (void *)&reserve, portMAX_DELAY);
    xQueueReceive(xQueue_LCD, (void *)&mode_display, portMAX_DELAY); 
    xQueueReceive(xQueue_LCD, (void *)&velocity, portMAX_DELAY);
    xQueueReceive(xQueue_LCD, (void *)&mileage, portMAX_DELAY); 
    xQueueReceive(xQueue_LCD, (void *)&operating_time, portMAX_DELAY);  
    xQueueReceive(xQueue_LCD, (void *)&range, portMAX_DELAY);  
    xQueueReceive(xQueue_LCD, (void *)&rpm, portMAX_DELAY); 
     
    //BATTERY
    int bat_simbol = map(battery,batMin,batMax,0,8);
    lcd.setCursor(0, 0);
    switch(bat_simbol){   
      case 8: 
        lcd.print(char(255));
        lcd.print(char(255));
        lcd.write(11);
        break;
      case 7: 
        lcd.print(char(255));
        lcd.print(char(255));
        lcd.write(12);
        break;
      case 6: 
        lcd.print(char(255));
        lcd.print(char(255));
        lcd.write(13);
        break;
      case 5: 
        lcd.print(char(255));
        lcd.write(15);
        lcd.write(14);
        break;  
      case 4: 
        lcd.print(char(255));
        lcd.write(16);
        lcd.write(14);
        break;
      case 3: 
        lcd.print(char(255));
        lcd.write(17);
        lcd.write(14);
        break;
      case 2: 
        lcd.write(18);
        lcd.write(17);
        lcd.write(14);
        break;
      case 1: 
        lcd.write(19);
        lcd.write(17);
        lcd.write(14);
        break;
      case 0: 
        lcd.write(20);
        lcd.write(17);
        lcd.write(14);
        break;
    }
    lcd.print(" ");
    lcd.print(battery); 
    lcd.print("%  ");
    
    //FUEL
    lcd.setCursor(19, 3);
    lcd.write(10);
    int grafico_fuel = map(fuel,fuelMin,fuelMax,0,8);
    switch(grafico_fuel){
      case 8: 
        lcd.setCursor(19, 0);
        lcd.print(char(255));
        lcd.setCursor(19, 1);
        lcd.print(char(255));
        lcd.setCursor(19, 2);
        lcd.print(char(255));
        break;
      case 7: 
        lcd.setCursor(19, 0);
        lcd.write(1);
        lcd.setCursor(19, 1);
        lcd.print(char(255));
        lcd.setCursor(19, 2);
        lcd.print(char(255));
        break;
      case 6: 
        lcd.setCursor(19, 0);
        lcd.write(2);
        lcd.setCursor(19, 1);
        lcd.print(char(255));
        lcd.setCursor(19, 2);
        lcd.print(char(255));
        break;
      case 5: 
        lcd.setCursor(19, 0);
        lcd.print(" ");
        lcd.setCursor(19, 1);
        lcd.write(3);
        lcd.setCursor(19, 2);
        lcd.print(char(255));
        break;
      case 4: 
        lcd.setCursor(19, 0);
        lcd.print(" ");
        lcd.setCursor(19, 1);
        lcd.write(4);
        lcd.setCursor(19, 2);
        lcd.print(char(255));
        break;
      case 3: 
        lcd.setCursor(19, 0);
        lcd.print(" ");
        lcd.setCursor(19, 1);
        lcd.write(5);
        lcd.setCursor(19, 2);
        lcd.print(char(255));
        break;
      case 2: 
        lcd.setCursor(19, 0);
        lcd.print(" ");
        lcd.setCursor(19, 1);
        lcd.print(" ");
        lcd.setCursor(19, 2);
        lcd.write(6);
        break;
      case 1: 
        lcd.setCursor(19, 0);
        lcd.print(" ");
        lcd.setCursor(19, 1);
        lcd.print(" ");
        lcd.setCursor(19, 2);
        lcd.write(7);
        break;
      case 0: 
        lcd.setCursor(19, 0);
        lcd.print(" ");
        lcd.setCursor(19, 1);
        lcd.print(" ");
        lcd.setCursor(19, 2);
        lcd.write(8);
        break;
    } 
    if(reserve == 1){
      lcd.setCursor(10, 0);
      if( (millis() - millis_task) - 500 ){        
        lcd.print("RESERVE!");
        if((millis() - millis_task) - 1000){
          lcd.print("        ");
          millis_task = millis();
        }
      }
    }  
  
    //Mode Display 
    if(mode_display == 1){
      
      //VELOCITY  
      lcd.setCursor(0, 1);
      lcd.print( velocity ); 
      lcd.print("Km/h ");
    
      //MILEAGE 
      lcd.setCursor(10, 1);
      lcd.print( mileage ); 
      lcd.print("Km ");
    
      //OPERATING TIME
      lcd.setCursor(0, 2);
      lcd.print("T:");
      lcd.print( operating_time ); 
      lcd.print("min ");
    
      //RANGE
      lcd.setCursor(10, 2);
      lcd.print("R:");
      lcd.print( range ); 
      lcd.print("min ");
    
      //RPM 
      lcd.setCursor(0, 3);
      int grafico_rpm = map(rpm,rpmMin,rpmMax,0,13);
      for (int nL=0; nL < grafico_rpm; nL++)  lcd.print(char(255));
      for (int nL=grafico_rpm; nL < 14; nL++) lcd.print(" ");
      lcd.print(" RPM ");
    }
    else if(mode == 2){  
      lcd.setCursor(0, 1);
      lcd.print("RANGE:");
      lcd.setCursor(0, 2);
      lcd.print( range ); 
      lcd.print("min ");
    }
    else{
      lcd.setCursor(0, 2);
      lcd.print("ERROR!"); 
    }
  }
}


void setup() {

  Serial.begin(115200);
  
  lcd.init();      //START COMMUNICATION WITH DISPLAY
  lcd.backlight(); //TURN ON THE LIGHT OF DISPLAY
  lcd.clear();     //CLEAN THE DISPLAY 

  while (!Serial) {
    ; // Only continue when the serial be ready to work.
  }

  //Queue creation
  xQueue_LCD = xQueue_LCD( 9, sizeof( float ) );

  lcd.createChar(10, Fuel_Simbol);
  lcd.createChar(1, Fuel_1);
  lcd.createChar(2, Fuel_2);
  lcd.createChar(3, Fuel_3);
  lcd.createChar(4, Fuel_4);
  lcd.createChar(5, Fuel_5);
  lcd.createChar(6, Fuel_6);
  lcd.createChar(7, Fuel_7);
  lcd.createChar(8, Fuel_8);

  lcd.createChar(11, bat1);
  lcd.createChar(12, bat2);
  lcd.createChar(13, bat3);
  lcd.createChar(14, bat4);
  lcd.createChar(15, bat5);
  lcd.createChar(16, bat6);
  lcd.createChar(17, bat7);
  lcd.createChar(18, bat8);
  lcd.createChar(19, bat9);
  lcd.createChar(20, bat10);
 
     
  if (xQueue_LCD == NULL)
    {
       Serial.println("ERROR: IT IS NOT POSSIBLE TO CREATE THE QUEUE!");
       while(1); //Without the queue, the working of the display is impaired
    } 

  //TASK CREATION
  xTaskCreatePinnedToCore(task_lcd,        //Main function of the task.
                         "task_lcd",       //Task name.
                         2048,             //Task size, in other words, quantity of bytes that this function will use
                         NULL,             //Arguments to be passed to the task.
                         7,                //Task priority, bigger the number, bigger the priority. The RTOS of ESP offer 25 levels.
                         NULL,             //Place to store save the unique identifier (UID) of this task.
                         PRO_CPU_NUM);     //Task affinity, can have affinity with PRO_CPU, APP_CPU or "tskNO_AFFINITY" (RTOS decide).
   
  xTaskCreatePinnedToCore(task_readind,       
                         "task_readind",       
                         2048,                  
                         NULL,                 
                         6,                     
                         NULL,                  
                         APP_CPU_NUM);
                                 
}

void loop() {
  // Everything is executed in the tasks.
}
