//BIBLIOTECAS
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

//DEFINIÇÕES
LiquidCrystal_I2C lcd_1(0x27,20,4); //Display 1
LiquidCrystal_I2C lcd_2(0x3F,20,4); //Display 2

void TaskDisplay_1(void *arg) {
  
  lcd_1.setCursor(0, 1); //MUDAR CURSOR, (PRIMEIRA COLUNA, SEGUNDA LINHA)
  //INFORMA O VALOR DA VELOCIDADE NAS 10 PRIMEIRAS COLUNAS
  if ((velocidade >= velMin) && (velocidade <= velMax)) {
     lcd_1.print( velocidade ); 
     lcd_1.print("Km/h ");
  } else {
     lcd_1.print("*****     ");
  }
  //CRIA UM GRÁFICO AUXILIAR COM AS ULTIMAS 10 COLUNAS
  int grafico1 = map(velocidade,velMin,velMax,0,10);
  for (int nL=0; nL < grafico1; nL++)  lcd.print(char(255));
  for (int nL=grafico1; nL < 10; nL++) lcd.print(" ");

  lcd_1.setCursor(0, 2); //MUDAR CURSOR, (PRIMEIRA COLUNA, TERCEIRA LINHA)
  //INFORMA O VALOR DO RPM NAS 10 PRIMEIRAS COLUNAS
  if ((rpm >= rpmMin) && (rpm <= rpmMax)) {
     lcd_1.print( rpm ); 
     lcd_1.print("RPM ");
  } else {
     lcd_1.print("******    ");
  }
  //CRIA UM GRÁFICO AUXILIAR COM AS ULTIMAS 10 COLUNAS
  int grafico2 = map(rpm,rpmMin,rpmMax,0,10);
  for (int nL=0; nL < grafico2; nL++)  lcd.print(char(255));
  for (int nL=grafico2; nL < 10; nL++) lcd.print(" ");

  lcd_1.setCursor(0, 3); //MUDAR CURSOR, (PRIMEIRA COLUNA, QUARTA LINHA)
  lcd_1.print("FUEL ");
  //O COMBUSTIVEL SERÁ INFORMADO PELO GRÁFICO
   if (fuel > reserva) {
     int grafico3 = map(rpm,rpmMin,rpmMax,0,15);
     for (int nL=0; nL < grafico3; nL++)  lcd.print(char(255));
     for (int nL=grafico3; nL < 15; nL++) lcd.print(" ");
  } else {
     lcd_1.print("-- RESERVA!! --");
  } 
  
}

void TaskDisplay_2(void *arg) {
  
  lcd_2.setCursor(0, 1); //MUDAR CURSOR, (PRIMEIRA COLUNA, SEGUNDA LINHA)
  lcd_2.print("AUTONOMIA     ");
  lcd_2.print(autonomia);
  lcd_2.print("min");

  lcd_2.setCursor(0, 2); //MUDAR CURSOR, (PRIMEIRA COLUNA, TERCEIRA LINHA)
  lcd_2.print("QUILOMETRAGEM ");
  lcd_2.print(quilometragem);
  lcd_2.print("Km");

  lcd_2.setCursor(0, 3); //MUDAR CURSOR, (PRIMEIRA COLUNA, QUARTA LINHA)
  lcd_2.print("AUTONOMIA R.  ");
  lcd_2.print(autonomia_r);
  lcd_2.print("min");
  
}


void setup() {
  
  lcd_1.init();      //INICIAR COMUNIÇÃO DO DISPLAY
  lcd_1.backlight(); //LIGA A ILUMINAÇÃO DO DISPLAY
  lcd_1.clear();     //LIMPA O DISPLAY 
  lcd_1.print("---- PARAHYBAJA ----");

  lcd_2.init();      
  lcd_2.backlight(); 
  lcd_2.clear(); 
  lcd_2.print("---- PARAHYBAJA ----");
  
  xTaskCreatePinnedToCore(TaskDisplay_1,        //Função principal da sua task.
                         "TaskDisplay_1",       //Nome para essa task.
                         2048,                  //Tamanho de pilha para essa task, ou seja, espaço em bytes que essa função irá utilizar. 
                         NULL,                  //Argumento para ser passado a task.
                         7,                     //Prioridade dessa task,quanto maior o número, maior a prioridade, o RTOS do ESP oferece 25 níveis.
                         NULL,                  //Local para guardar o Identificador único para essa task.
                         PRO_CPU_NUM);          //Afinidade dessa task, podendo ter afinidade com PRO_CPU, APP_CPU ou "tskNO_AFFINITY" (RTOS decide).
   
  xTaskCreatePinnedToCore(TaskDisplay_2,       
                         "TaskDisplay_2",       
                         2048,                  
                         NULL,                 
                         6,                     
                         NULL,                  
                         APP_CPU_NUM); 
                                 
}

void loop() {
  
}
