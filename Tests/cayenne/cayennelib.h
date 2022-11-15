// --------- defines -----------

#define CANAL_CYN_00   0 // Timer Viewer
#define CANAL_CYN_01   1 // Emergency Fuel - Cayenne.virtualWrite(CANAL_CYN_01, value);
#define CANAL_CYN_02   2 // Timer Definer
#define CANAL_CYN_03   3 // New
#define CANAL_CYN_04   4 // Restart
#define CANAL_CYN_05   5 // Temperature - Cayenne.virtualWrite(CANAL_CYN_05, value);
#define CANAL_CYN_06   6 // RPM - Cayenne.virtualWrite(CANAL_CYN_06, value);
#define CANAL_CYN_07   7 // Rollover - Cayenne.virtualWrite(CANAL_CYN_07, value);
#define CANAL_CYN_08   8 
#define CANAL_CYN_09   9 // Battery
#define CANAL_CYN_10   10 // DINAMIC SPEED
#define CANAL_CYN_11   11 
#define CANAL_CYN_12   12 // Start/Stop
#define CANAL_CYN_13   13
#define CANAL_CYN_14   14
#define CANAL_CYN_15   15
#define CANAL_CYN_16   16
#define CANAL_CYN_17   17
#define CANAL_CYN_18   18
#define CANAL_CYN_19   19
#define CANAL_CYN_20   20 
#define CANAL_CYN_21   21
#define CANAL_CYN_22   22
#define CANAL_CYN_23   23
#define CANAL_CYN_24   24
#define CANAL_CYN_25   25
#define CANAL_CYN_26   26
#define CANAL_CYN_27   27
#define CANAL_CYN_28   28
#define CANAL_CYN_29   29
#define CANAL_CYN_30   30
#define CANAL_CYN_31   31

//---------WiFi configs----------
char ssid[] = "CIMATEC PARK - VISITANTE";
char password[] = "";

//---------Cayenne Device------------
char username[] = "4ad99150-53b2-11ed-bf0a-bb4ba43bd3f6";
char mqtt_passwork[] = "6d350b9f302a61c659578279262ae22a73c5047e";
char client_id[] = "71438170-551f-11ed-bf0a-bb4ba43bd3f6";

//---------variables
uint32_t timer_cayenne;
uint32_t timer_cayenne2;
int timer;

void init_cayenne(){
  // -----Cayenne settings-----  
  Cayenne.begin(username, mqtt_passwork, client_id, ssid, password);

  // -----init time-control---------
  timer_cayenne = millis();
  timer_cayenne2 = millis();
  timer = 0;
}

void timerNewValue(int value){
  Cayenne.virtualWrite(CANAL_CYN_00, value);
  Serial.print(value);
  Serial.println("---Valor do slide");
  timer = value;
}
