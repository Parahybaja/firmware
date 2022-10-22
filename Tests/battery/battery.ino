#include <LiquidCrystal.h>

#define pinoAnalogico 0//determina o pino analogico que será utilizado

#define tensaoMax 3.3 //tensão máxima da bateria Vin
#define valueDigital 675 //value digital or voltage
#define voltageCoefficient 4.24 //soma dos valores dos resistores

// intervalo de tensão valido 12 - 13 


int sensortensaoPino = 0; // Vout tensão de saída do divisor de tensão
int cargabateria = 0;
float voltagem = 0;
float voltageFull = 0;
int cont = 0;

LiquidCrystal LCD(12, 11, 5, 4, 3, 2);

void sensorbateria( void ){
  
  voltagem = (tensaoMax * sensortensaoPino)/ valueDigital;
  
  voltageFull = voltagem * voltageCoefficient;
}

void cargaPorcentagem( void ){ //calcula a porcentagem da bateria
  
  if( voltageFull >=12 && voltageFull < 13){

      cargabateria = (voltageFull - 12) * 100;
  }
  if(voltageFull >= 13){

    cargabateria = 100;
  }
  if( voltageFull < 12 ){

    cargabateria = 0;
  }

  
  
}

void setup(){
  
  LCD.begin(16, 2); //define as dimensões do arduino
  
  LCD.setCursor(0,0); //envia o cursor para o primeiro ponto do display
  //LCD.print("bateria:");
  //tensaoPino = analogRead( pinoAnalogico);
}

void loop(){

  sensortensaoPino = analogRead(pinoAnalogico);
  
  sensorbateria();
  cargaPorcentagem();
  

  if(voltageFull < 13){

    LCD.print("charge:");
    LCD.print(cargabateria);
    LCD.print("%");

    LCD.setCursor(0,1);

    LCD.print("T(v):");
    LCD.print(voltageFull);
  }
  else{

    LCD.clear();

    LCD.print("load");
    LCD.print("    T:");
    LCD.print(voltageFull);
    LCD.print("v");
    
    LCD.setCursor(0,1);

    for(cont = 0; cont <= 15 ; cont++){
      delay(200);
      LCD.print("|");
    }
  }
  

  delay(250);
  LCD.clear();
  
}