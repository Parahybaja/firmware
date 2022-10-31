#include <CayenneMQTTESP32.h>
#include <esp_now.h>

// -----include external files-----
//#include "configs.h"      // general configs - falta fazer as configs
//#include "var_global.h"   // global variables must be declared before functions definitions
//#include "prototypes.h"   // functions prototypes must be explicitly declared 
// #include "tasks.h"     // task functions definitions
//#include "system.h"       // system functions
//#include "esp-now.h"      // esp-now functions

// As funcaos estão do lado dos defines com o que elas são e como chama-las. Para usa-las a minha ideia seria usar [ Cayenne.virtualWrite(CANAL, VALOR); ]
// No esp-now. Assim quando o valor fosse recebido por esp now ainda no OnRecv podemos mandar os dados para a dashboard para ser o mais precio possivel. 

#define CANAL_CYN_00   0 // Speed Graphic - Cayenne.virtualWrite(CANAL_CYN_00, value);
#define CANAL_CYN_01   1 // Emergency Fuel - Cayenne.virtualWrite(CANAL_CYN_01, value);
#define CANAL_CYN_02   2 // Timer 
#define CANAL_CYN_03   3 // New
#define CANAL_CYN_04   4 // Restart
#define CANAL_CYN_05   5 // Temperature - Cayenne.virtualWrite(CANAL_CYN_05, value);
#define CANAL_CYN_06   6 // RPM - Cayenne.virtualWrite(CANAL_CYN_06, value);
#define CANAL_CYN_07   7 // Rollover - Cayenne.virtualWrite(CANAL_CYN_07, value);
#define CANAL_CYN_08   8 
#define CANAL_CYN_09   9
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

char ssid[] = "PARAHYBAJA";
char password[] = "parahybaja98";

char username[] = "4ad99150-53b2-11ed-bf0a-bb4ba43bd3f6";
char mqtt_passwork[] = "6d350b9f302a61c659578279262ae22a73c5047e";
char client_id[] = "71438170-551f-11ed-bf0a-bb4ba43bd3f6";

int randomNumber;

void setup() {

//  init_system();
    
//  init_espnow();
  
  Cayenne.begin(username, mqtt_passwork, client_id, ssid, password);
}

void loop() {
  Cayenne.loop(); // Função que precisa ta no loop
  randomNumber = random(60);
  Cayenne.virtualWrite(CANAL_CYN_00, randomNumber);
  Cayenne.virtualWrite(CANAL_CYN_06, random(4000));
  Cayenne.virtualWrite(CANAL_CYN_07, random(1));
  Cayenne.virtualWrite(CANAL_CYN_10, randomNumber);

  delay(1000);
}

/*CAYENNE_IN(CANAL_CYN_02){ 
   int value = getValue.asInt();  

   //falta como vai ser o codigo para o timer em si, como ta um slide pode passar qualquer valor para o codigo do display
}

CAYENNE_IN(CANAL_CYN_03){ // Quando se usa o botao com o nome New do dashboard, toda vez essa função é chamada e é feito o que esta entre colchetes
   cmd_t config = {BOARDID, CMD_NEW_FILE};
   esp_now_send(address_sender, (uint8_t *) &config, sizeof(cmd_t));
   esp_now_send(address_receiver, (uint8_t *) &config, sizeof(cmd_t));

   Serial.println("\n\n----- new file command sent -----");

   Cayenne.virtualWrite(CANAL_CYN_03, 0);
}

CAYENNE_IN(CANAL_CYN_04){ 

   cmd_t config = {BOARDID, CMD_RESTART};
   esp_now_send(address_sender, (uint8_t *) &config, sizeof(cmd_t));
   esp_now_send(address_receiver, (uint8_t *) &config, sizeof(cmd_t));

   Serial.println("\n\n----- restart command sent -----");

   Cayenne.virtualWrite(CANAL_CYN_04, 0);
}


CAYENNE_IN(CANAL_CYN_12){ 
   int value = getValue.asInt();

   if (value == 1) {
    cmd_t config = {BOARDID, CMD_START};
    esp_now_send(address_sender, (uint8_t *) &config, sizeof(cmd_t));
    esp_now_send(address_receiver, (uint8_t *) &config, sizeof(cmd_t));

    Serial.println("\n\n----- start command sent -----");
   }
   else if (value == 0) {
    cmd_t config = {BOARDID, CMD_STOP};
    esp_now_send(address_sender, (uint8_t *) &config, sizeof(cmd_t));
    esp_now_send(address_receiver, (uint8_t *) &config, sizeof(cmd_t));

    Serial.println("\n\n----- stop command sent -----");
   }
}

*/
CAYENNE_OUT_DEFAULT() // envia dados periodicamente, usamos essa função para manter os dados que nao precisam ser atualizados constantemente 
{
  Cayenne.virtualWrite(CANAL_CYN_01, random(1));
  Cayenne.virtualWrite(CANAL_CYN_05, random(40));
}

// Modelo
/* void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len){
    if (len ==  sizeof(sensor_t)){
        sensor_t sensor = {};
        memcpy(&sensor, incomingData, sizeof(sensor));

        if (sensor.type == SENSOR_01){
            // -----send sensor 1 data through queue-----
            xQueueSend(qh_sensor_1, &sensor, pdMS_TO_TICKS(0));
            Cayenne.virtualWrite(CANAL_CYN_sensor_1, sensor_1);
        }
        else if (sensor.type == SENSOR_02){
            // -----send sensor 2 data through queue-----
            xQueueSend(qh_sensor_2, &sensor, pdMS_TO_TICKS(0));
            ayenne.virtualWrite(CANAL_CYN_sensor_2, sensor_2);
        }
        else {
            INFO("INFO_1: unknown sensor type");
        }
    }
    else {
        INFO("INFO_1: unknown incoming data");
    }
}*/
