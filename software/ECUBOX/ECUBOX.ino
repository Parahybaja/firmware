/**
 * @file ECUBOX.ino
 * @author jefferson lopes (jefferson.lopes@ee.ufcg.edu.br)
 * @brief 
 * @version 0.1
 * @date 2022-10-30
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <ESP8266WiFi.h>
#include <espnow.h>

// -----include external files-----
#include "configs.h"      // general configs
#include "var_global.h"   // global variables must be declared before functions definitions
#include "prototypes.h"   // functions prototypes must be explicitly declared 
// #include "tasks.h"        // task functions definitions
#include "system.h"       // system functions
#include "esp-now.h"      // esp-now functions
 
void setup() {
    init_system();
    
    init_espnow();
}

void loop(){
    while(Serial.available()) 
        msg += Serial.readString();
    
    if (msg == "start\n"){
        //clean all variables
        msg = "";
        
        cmd_t config = {BOARDID, CMD_START};
        esp_now_send(address_sender, (uint8_t *) &config, sizeof(cmd_t));
        esp_now_send(address_receiver, (uint8_t *) &config, sizeof(cmd_t));

        Serial.println("\n\n----- start command sent -----");
    }
    else if (msg == "stop\n"){
        //clean all variables
        msg = "";
        
        cmd_t config = {BOARDID, CMD_STOP};
        esp_now_send(address_sender, (uint8_t *) &config, sizeof(cmd_t));
        esp_now_send(address_receiver, (uint8_t *) &config, sizeof(cmd_t));

        Serial.println("\n\n----- stop command sent -----");
    }
    else if (msg == "new\n"){
        //clean all variables
        msg = "";
        
        cmd_t config = {BOARDID, CMD_NEW_FILE};
        esp_now_send(address_sender, (uint8_t *) &config, sizeof(cmd_t));
        esp_now_send(address_receiver, (uint8_t *) &config, sizeof(cmd_t));;

        Serial.println("\n\n----- new file command sent -----");
    }
    else if (msg == "restart\n"){
        //clean all variables
        msg = "";
        
        cmd_t config = {BOARDID, CMD_RESTART};
        esp_now_send(address_sender, (uint8_t *) &config, sizeof(cmd_t));
        esp_now_send(address_receiver, (uint8_t *) &config, sizeof(cmd_t));

        Serial.println("\n\n----- restart command sent -----");
    }
    else if (msg != "") {
        msg = "";
        Serial.println("\n\n----- command does not exist -----");
    }
}
