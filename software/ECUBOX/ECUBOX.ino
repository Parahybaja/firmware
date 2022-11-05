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
#include <CayenneMQTTESP32.h>
#include <esp_now.h>

// -----include external files-----
#include "configs.h"      // general configs
#include "var_global.h"   // global variables must be declared before functions definitions
#include "prototypes.h"   // functions prototypes must be explicitly declared 
// #include "tasks.h"        // task functions definitions
#include "system.h"       // system functions
#include "esp-now.h"      // esp-now functions
#include "cayennelib.h"   // cayenne-configs
 
void setup() {
    init_system();
    
    init_espnow();
}

void loop(){
  Cayenne.loop(100); // Função que precisa ta no loop

  if ((millis() - timer_cayenne) > 2200){
    timer_cayenne += 2200;
    Cayenne.virtualWrite(CANAL_CYN_06, random(4000));// RPM
    Cayenne.virtualWrite(CANAL_CYN_10, random(60));  // Speed
  }

  if ((millis() - timer_cayenne2) > 60000){
    timer_cayenne2 += 60000;
    Cayenne.virtualWrite(CANAL_CYN_05, random(40)); // Temp
    Cayenne.virtualWrite(CANAL_CYN_09, random(100));// Battery
  }
}
