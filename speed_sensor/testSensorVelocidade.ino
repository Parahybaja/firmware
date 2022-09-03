




//codigo sensor de velocidade
// le uma borda, quando 3 bordas são lidas isso indica que um giro no pneu foi dado
//lemos o tempo que levou para ocorrer um giro
//fazemos a seguinte operação: Velocidade = comprimento / tempo
// exibir velocidade no LCD


//comprimento é dado por: 2*pi*r
//comprimento do pneu: 1,8353184282271572099098762645119
//diametro do pneu = 23 polegadas - em metros: 0,5842
//raio: 0,5842 / 2
//#define raioDinamico = 0,2921 //raio dinamico do pneu do baja
//#define comprimentoPneu 1.8353184

#define CONST1          1.02  //constante raio dinamico
#define CONST2          0.47  //constante raio dinamico
#define DIAMETRO        0.5842  //diametro em metros        
#define WAIT_TIMEms     500
#define WAIT_TIMEs      0.5
#define GPIO            22 //porta digital utilizada

float velocidade = 0.0;
volatile uint16_t count = 0;
uint32_t timer_1;

void IRAM_ATTR speeD(){ count++; }

void setup() {
  pinMode(GPIO, INPUT_PULLUP);

  Serial.begin(115200);
  
  attachInterrupt(GPIO, speeD, CHANGE);

  // update timers - sempre no final do setup
  timer_1 = millis();
}

void loop() {
  if ((millis() - timer_1) >= WAIT_TIMEms){
    // update timer
    timer_1 += WAIT_TIMEms;

    // cria uma copia local e zera o counts
    int count_copy;
    noInterrupts();
    count_copy = count;
    count = false;
    interrupts();

    // calculo de velocidade
    float voltas = count_copy / 6.0;
    float raioDinamico = CONST1 * DIAMETRO * CONST2;
    float comprimentoPneu = 2 * PI * raioDinamico;
    float distance = voltas * comprimentoPneu;
    velocidade = distance / WAIT_TIMEs; // resultado em metros/segundo
  
    Serial.print("Velocidade: ");
    Serial.print(velocidade);
    Serial.println(" m/s");
  }
}
