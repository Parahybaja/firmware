#include <CayenneMQTTESP32.h>
#include <esp_now.h>

#define DEBUG_MODE         true
#define BOARDID_01         1
#define ESPNOW_BUFFER_SIZE 48

// -----command lookup table-----
#define CMD_START    0x01
#define CMD_STOP     0x02
#define CMD_NEW_FILE 0x03
#define CMD_RESTART  0X04

// -----include external files-----
//#include "configs.h"      // general configs - falta fazer as configs
//#include "var_global.h"   // global variables must be declared before functions definitions
//#include "prototypes.h"   // functions prototypes must be explicitly declared 
//#include "tasks.h"     // task functions definitions
//#include "system.h"       // system functions
//#include "esp-now.h"      // esp-now functions

// As funcaos estão do lado dos defines com o que elas são e como chama-las. Para usa-las a minha ideia seria usar [ Cayenne.virtualWrite(CANAL, VALOR); ]
// No esp-now. Assim quando o valor fosse recebido por esp now ainda no OnRecv podemos mandar os dados para a dashboard para ser o mais precio possivel. 

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

//---------WiFi----------
char ssid[] = "PARAHYBAJA";
char password[] = "parahybaja98";

//---------Cayenne Device------------
char username[] = "4ad99150-53b2-11ed-bf0a-bb4ba43bd3f6";
char mqtt_passwork[] = "6d350b9f302a61c659578279262ae22a73c5047e";
char client_id[] = "71438170-551f-11ed-bf0a-bb4ba43bd3f6";

int timer;
uint32_t timer_cayenne;
uint32_t timer_cayenne2;

//---------ESPNOW Structs and Address---------------
uint8_t address_base[] = {0x94, 0xB5, 0x55, 0x2D, 0x1E, 0x0C};

typedef struct set_up {
    uint8_t id;
    uint8_t command;
} set_up;

typedef struct debug_data{
    uint8_t id;
    char msg[ESPNOW_BUFFER_SIZE];
} debug_data;


void setup() {

//  init_system();
    
//  init_espnow();

Serial.begin(115200);

// -----ESPNOW settings-----
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if (esp_now_init() != ESP_OK)    // check if was initialized successfully
      Serial.println("Error initializing ESP-NOW");
  esp_now_register_send_cb(OnDataSent);

  // register peer
  esp_now_peer_info_t peerInfo = {};
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  memcpy(peerInfo.peer_addr, address_base, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
      Serial.println("ERROR: Failed to add peer");
  Serial.println("INFO: Peer added");
  delay(50); // give time to send the espnow message
  esp_now_register_recv_cb(OnDataRecv);

// -----Cayenne settings-----  
  Cayenne.begin(username, mqtt_passwork, client_id, ssid, password);

  // init timer
  timer_cayenne = millis();
  timer_cayenne2 = millis();
  timer = 0;
}

void loop() {
  Cayenne.loop(100); // Função que precisa ta no loop

  if ((millis() - timer_cayenne) > 2200){
    timer_cayenne += 2200;
    //3158
    Cayenne.virtualWrite(CANAL_CYN_06, random(4000));
    Cayenne.virtualWrite(CANAL_CYN_10, random(60));
  }

  if ((millis() - timer_cayenne2) > 60000){
    
    timer_cayenne2 += 60000;
    Cayenne.virtualWrite(CANAL_CYN_05, random(40));
    Cayenne.virtualWrite(CANAL_CYN_09, random(100));
    Serial.println("Teste");
    if (timer >= 1){
      timer = timer - 1;
      Cayenne.virtualWrite(CANAL_CYN_00, timer);
    }
  }
}

CAYENNE_IN(CANAL_CYN_02){ 
  
  int value = getValue.asInt();
  //comando do ESPNOW (A fazer)
  //esp_err_t result = esp_now_send(address_base, (uint8_t *) &config, sizeof(set_up));
  Serial.println("\n\n----- new time command sent -----");
  //Altera valor na dash
  timerNewValue(value);
}

CAYENNE_IN(CANAL_CYN_03){ // Quando se usa o botao com o nome New do dashboard, toda vez essa função é chamada e é feito o que esta entre colchetes
  if (getValue.asInt() == 1) {
    set_up config = {BOARDID_01, CMD_NEW_FILE};
    esp_err_t result = esp_now_send(address_base, (uint8_t *) &config, sizeof(set_up));
    Serial.println("\n\n----- new file command sent -----");
  }
}

CAYENNE_IN(CANAL_CYN_04){ 
  if (getValue.asInt() == 1) {
    set_up config = {BOARDID_01, CMD_RESTART};
    esp_err_t result = esp_now_send(address_base, (uint8_t *) &config, sizeof(set_up));
    Serial.println("\n\n----- restart command sent -----");
  }
}


CAYENNE_IN(CANAL_CYN_12){ 

   if (getValue.asInt() == 1) {
    set_up config = {BOARDID_01, CMD_START};
    esp_err_t result = esp_now_send(address_base, (uint8_t *) &config, sizeof(set_up));
    
    Serial.println("\n\n----- start command sent -----");
   }
   else if (getValue.asInt() == 0) {
    set_up config = {BOARDID_01, CMD_STOP};
    esp_err_t result = esp_now_send(address_base, (uint8_t *) &config, sizeof(set_up));

    Serial.println("\n\n----- stop command sent -----");
   }
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
#if DEBUG_MODE
    Serial.print("Send status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
#endif
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    #if DEBUG_MODE
        Serial.print("packet received size: ");
        Serial.println(len);
    #endif
}

void timerNewValue(int value){
  Cayenne.virtualWrite(CANAL_CYN_00, value);
  Serial.print(value);
  Serial.println("---Valor do slide");
  timer = value;
}
