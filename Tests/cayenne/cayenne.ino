#include <CayenneMQTTESP32.h>
#include <esp_now.h>

// -----include external files-----
#include "configs.h"      // general configs - falta fazer as configs
#include "cayennelib.h"

#define DEBUG_MODE         true
#define BOARDID_01         1
#define ESPNOW_BUFFER_SIZE 48

// -----command lookup table-----
#define CMD_START    0x01
#define CMD_STOP     0x02
#define CMD_NEW_FILE 0x03
#define CMD_RESTART  0X04

// As funcaos estão do lado dos defines com o que elas são e como chama-las. Para usa-las a minha ideia seria usar [ Cayenne.virtualWrite(CANAL, VALOR); ]
// No esp-now. Assim quando o valor fosse recebido por esp now ainda no OnRecv podemos mandar os dados para a dashboard para ser o mais precio possivel. 


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
  init_cayenne();

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
