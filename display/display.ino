//LIBRARIES
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

//DISPLAY
LiquidCrystal_I2C lcd(0x27,20,4); 

//QUEUE
QueueHandle_t xQueue_LCD;

//TASKS
void task_readind( void *pvParameters );
void task_lcd( void *pvParameters );

//AUXILIARY SETTINGS
#define batMin  0
#define batMax  100
#define rpmMin  0
#define rpmMax  3000
#define fuelMin 0
#define fuelMax 100
unsigned long millis_task = 0;

//AUXILIARY VARIABLES, BUTTON
#define pin_button 4
bool button;
static bool buttonBefore;
static bool buttonAux = true;
static unsigned long delayButton = 0;

//SPECIAL CHARACTERS
byte Fuel_S[8] = {B00000,B01110,B10001,B10001,B11111,B11011,B11011,B11111};
byte Fuel_1[8] = {B00000,B00000,B11111,B11111,B11111,B11111,B11111,B11111};
byte Fuel_2[8] = {B00000,B00000,B00000,B00000,B11111,B11111,B11111,B11111};
byte Fuel_3[8] = {B00000,B00000,B00000,B00000,B00000,B00000,B11111,B11111};
byte batt_1[8] = {B11100,B11100,B11111,B11111,B11111,B11111,B11100,B11100};
byte batt_2[8] = {B11100,B00100,B00111,B00001,B00001,B00111,B00100,B11100};
byte batt_3[8] = {B11111,B11000,B11000,B11000,B11000,B11000,B11000,B11111};
byte batt_4[8] = {B11111,B00000,B00000,B00000,B00000,B00000,B00000,B11111};

//AUXILIARY FUNCTIONS
void print_batSimbol(int);
void print_graphFuel(int);
bool button_state();

//Para testes com o potenciometro
#define pin_pot 32
#define max_pot 1500

void setup() {
    Serial.begin(115200);
    while (!Serial){
        ; // Only continue when the serial be ready to work.
    }
        
    //QUEUE CREATION
    xQueue_LCD = xQueueCreate( 9, sizeof( float ) );
    if (xQueue_LCD == NULL){
        Serial.println("ERROR: IT IS NOT POSSIBLE TO CREATE THE QUEUE!"); 
        while(1); //Without the queue, the working of the display is impaired. Nothing more must be done.
    } 

    //CREATION OF TASKS
    xTaskCreate(task_lcd,     //Main function of the task.
                "task_lcd",   //Task name.
                2048,         //Task size, in other words, quantity of bytes that this function will use
                NULL,         //Arguments to be passed to the task.
                7,            //Task priority, bigger the number, bigger the priority. The RTOS of ESP offer 25 levels.
                NULL);        //Place to store save the unique identifier (UID) of this task.
    
    xTaskCreate(task_readind,"task_readind",2048,NULL,6,NULL);                  
    
    pinMode(pin_button, INPUT_PULLUP);
}

//READING TASK
void task_readind( void *pvParameters ){

    float battery;
    float fuel;
    float velocity;
    float odometer;      
    float operating_time; 
    float range;     
    float rpm;          
    float reserve;      
    float mode_display;
    
    while(1){    
    
        battery        = map(analogRead(pin_pot),0,max_pot,batMin,batMax);
        fuel           = map(analogRead(pin_pot),0,max_pot,fuelMin,fuelMax);
        velocity       = map(analogRead(pin_pot),0,max_pot,0,80);
        odometer       = map(analogRead(pin_pot),0,max_pot,0,150);
        operating_time = map(analogRead(pin_pot),0,max_pot,0,150);
        range          = map(analogRead(pin_pot),0,max_pot,0,150);
        rpm            = map(analogRead(pin_pot),0,max_pot,rpmMin,rpmMax);
        reserve        = button_state();
        mode_display   = button_state();
                    
        xQueueSendToBack(xQueue_LCD, &battery, portMAX_DELAY);
        xQueueSendToBack(xQueue_LCD, &fuel, portMAX_DELAY);
        xQueueSendToBack(xQueue_LCD, &velocity, portMAX_DELAY);
        xQueueSendToBack(xQueue_LCD, &odometer, portMAX_DELAY);
        xQueueSendToBack(xQueue_LCD, &operating_time, portMAX_DELAY);
        xQueueSendToBack(xQueue_LCD, &range, portMAX_DELAY);
        xQueueSendToBack(xQueue_LCD, &rpm, portMAX_DELAY);
        xQueueSendToBack(xQueue_LCD, &reserve, portMAX_DELAY); 
        xQueueSendToBack(xQueue_LCD, &mode_display, portMAX_DELAY);    

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

//DISPLAY TASK
void task_lcd(void *pvParameters) {

    lcd.init();      //START COMMUNICATION WITH DISPLAY
    lcd.backlight(); //TURN ON THE LIGHT OF DISPLAY
    lcd.clear();     //CLEAN THE DISPLAY

    lcd.createChar(1, batt_1);
    lcd.createChar(2, batt_2);
    lcd.createChar(3, batt_3);
    lcd.createChar(4, batt_4);
    lcd.createChar(5, Fuel_S);
    lcd.createChar(6, Fuel_1);
    lcd.createChar(7, Fuel_2);
    lcd.createChar(8, Fuel_3);
    
    float battery;
    float fuel;
    float reserve;
    float mode_display;
    float velocity;
    float odometer;
    float mileage;
    float operating_time;
    float range;
    float rpm;
    
    while(1){
        xQueueReceive(xQueue_LCD, &battery, portMAX_DELAY);
        xQueueReceive(xQueue_LCD, &fuel, portMAX_DELAY);
        xQueueReceive(xQueue_LCD, &velocity, portMAX_DELAY);
        xQueueReceive(xQueue_LCD, &odometer, portMAX_DELAY); 
        xQueueReceive(xQueue_LCD, &operating_time, portMAX_DELAY);
        xQueueReceive(xQueue_LCD, &range, portMAX_DELAY); 
        xQueueReceive(xQueue_LCD, &rpm, portMAX_DELAY);  
        xQueueReceive(xQueue_LCD, &reserve, portMAX_DELAY);  
        xQueueReceive(xQueue_LCD, &mode_display, portMAX_DELAY); 

        //BATTERY
        int bat_simbol = map(battery,0,100,0,5);
        print_batSimbol(bat_simbol);
        lcd.print(" ");
        lcd.print(battery); 
        lcd.print("%  ");
        
        //FUEL
        lcd.setCursor(19, 3);
        lcd.write(5);    
        int graph_fuel = map(fuel,0,95,0,11);
        print_graphFuel(graph_fuel);
    
        //FLAG RESERVE
        if(reserve == 1){ 
            if( (millis() - millis_task) > 700 ){        
                lcd.setCursor(10, 0);
                lcd.print("RESERVE!");
                
                if((millis() - millis_task) > 1400){
                    lcd.setCursor(10, 0);
                    lcd.print("        ");
                    millis_task = millis();
                }
            }
        }
        else{
            lcd.setCursor(10, 0);
            lcd.print("        "); 
        }
    
        //MODE 1, DISPLAY
        if(mode_display == 0){
            //VELOCITY  
            lcd.setCursor(0, 1);
            lcd.print( velocity ); 
            lcd.print("Km/h ");
            
            //MILEAGE ODOMETER
            lcd.setCursor(10, 1);
            lcd.print( odometer ); 
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
            int grafico_rpm = map(rpm,rpmMin,rpmMax,0,15);
            for (int nL=0; nL < grafico_rpm; nL++)  lcd.print(char(255));
                for (int nL=grafico_rpm; nL < 15; nL++) lcd.print(" ");
                    lcd.setCursor(14, 3);
            
            lcd.print(" RPM ");
        }
            
        //MODE 2, DISPLAY
        else if(mode_display == 1){  
            lcd.setCursor(0, 2);
            lcd.print("RANGE:");
            lcd.print( range ); 
            lcd.print("min ");
        }
        else{
            lcd.setCursor(0, 2);
            lcd.print("ERROR!"); 
        }  
    } 
}

void loop() {
  // Everything is executed in the tasks.
}

bool button_state(){
    if((millis() - delayButton) > 50){
        button = digitalRead(pin_button);
        if(button && (button != buttonBefore)){
            buttonAux = !buttonAux;
            delayButton = millis();
            lcd.clear();
        }
        buttonBefore = button;
    }

    return buttonAux;
}

void print_batSimbol(int bat_simbol){
    lcd.setCursor(0, 0);
    
    switch(bat_simbol){   
        case 5: 
            lcd.print(char(255));
            lcd.print(char(255));
            lcd.write(1);
            break;      
        case 4: 
            lcd.print(char(255));
            lcd.print(char(255));
            lcd.write(1);
            break;
        case 3: 
            lcd.print(char(255));
            lcd.print(char(255));
            lcd.write(2);
            break;
        case 2: 
            lcd.print(char(255));
            lcd.write(3);
            lcd.write(2);
            break;  
        case 1: 
            lcd.print(char(255));
            lcd.write(4);
            lcd.write(2);
            break;
        case 0: 
            lcd.write(3);
            lcd.write(4);
            lcd.write(2);
            break;
    }
}

void print_graphFuel(int graph_fuel){
    switch(graph_fuel){           
        case 11: 
            lcd.setCursor(19, 0);
            lcd.print(char(255));
            lcd.setCursor(19, 1);
            lcd.print(char(255));
            lcd.setCursor(19, 2);
            lcd.print(char(255));
            break;
        case 10: 
            lcd.setCursor(19, 0);
            lcd.write(6);
            lcd.setCursor(19, 1);
            lcd.print(char(255));
            lcd.setCursor(19, 2);
            lcd.print(char(255));
            break;
        case 9: 
            lcd.setCursor(19, 0);
            lcd.write(7);
            lcd.setCursor(19, 1);
            lcd.print(char(255));
            lcd.setCursor(19, 2);
            lcd.print(char(255));
            break;
        case 8: 
            lcd.setCursor(19, 0);
            lcd.write(8);
            lcd.setCursor(19, 1);
            lcd.print(char(255));
            lcd.setCursor(19, 2);
            lcd.print(char(255));
            break;
        case 7: 
            lcd.setCursor(19, 0);
            lcd.print(" ");
            lcd.setCursor(19, 1);
            lcd.print(char(255));
            lcd.setCursor(19, 2);
            lcd.print(char(255));
            break;
        case 6: 
            lcd.setCursor(19, 0);
            lcd.print(" ");
            lcd.setCursor(19, 1);
            lcd.write(6);
            lcd.setCursor(19, 2);
            lcd.print(char(255));
            break;
        case 5: 
            lcd.setCursor(19, 0);
            lcd.print(" ");
            lcd.setCursor(19, 1);
            lcd.write(7);
            lcd.setCursor(19, 2);
            lcd.print(char(255));
            break;
        case 4: 
            lcd.setCursor(19, 0);
            lcd.print(" ");
            lcd.setCursor(19, 1);
            lcd.write(8);
            lcd.setCursor(19, 2);
            lcd.print(char(255));
            break;
        case 3: 
            lcd.setCursor(19, 0);
            lcd.print(" ");
            lcd.setCursor(19, 1);
            lcd.print(" ");
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
}
