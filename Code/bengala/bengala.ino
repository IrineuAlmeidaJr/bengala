/*
 * 
 *    --> Depois já programar o correto, dispar um tipo de alarme por 30 segundos, para contar os segundos
 *    terei que criar uma variável que conta os milesegundo, quando der 1000 por exemplo que seria 1 segundo
 *    dispara o Buzzer com um som e zera a variável que conta os milesegundo, mas ai terei um contador de segundos,
 *    que começa em zero e vai aumentando a cada disparo do alarme, se o número de disparos feitos (segundos) for menor
 *    que 30, significa que tocou 30 vezes ou 60 vezes se programar para tocar a cada meio segundo (isso ver melhor depois).
 *    Depois de 30 tocar até 240 segundos (4 minutos)s mais muda o som de novo e só uma toque por segundo aqui. 
 *    
 *    *** Se quiser e der tempo implementar o botão para cancelar envio da mensagem, simular o envio da mensagem por um som ou algo do
 *    tipo 
 *    
 *    Sensor GSM -> https://www.youtube.com/watch?v=GbVXixOUUPM
 *     
 *    *** ALARME DISPARA 
 *    I   - 2 m de distância, o levando a emitir alertas sonoros mais amenos, com repetições moderadas;
 *    II  - 80 cm do mesmo, esse alerta passa a se repetir com um menor intervalo de tempo;
 *    III - 50 cm do objeto em questão, se aumentará a intensidade e as repetições seguidas do alerta sonoro;
 *    IV  - 30 cm do usuário da bengala, a mesma emitirá alertas muito frequentes e intensos;
 *    
 *    --------------------------------------------
 *    ---> FAZ ISSO na SEGUNDA-FEIRA 
 *    I   - verificar se não é melhor usar a função "millis()" ao invés do delay();
 *    A função milis() retorna o tempo que o processo ficou em execução contando tudo, posso utiliza para saber os segundos que a 
 *    bengala caiu, abaixo um exemplo:
 *    
 *      long previousMillis = 0;        // Variável de controle do tempo
 *      long redLedInterval = 1000;     // Tempo em ms do intervalo a ser executado
 *     
 *       (...)
 *       void loop()
 *       {
 *         unsigned long currentMillis = millis();    //Tempo atual em ms
 *       
 *         //Lógica de verificação do tempo
 *         if (currentMillis - previousMillis > redLedInterval) { 
 *           previousMillis = currentMillis;    // Salva o tempo atual
 *       
 *           //E aqui muda o estado do led
 *           //verificando como ele estava anteriormente
 *           if (redLedState == LOW) {
 *             redLedState = HIGH;
 *           } else {
 *             redLedState = LOW;
 *           }
 *       
 *           digitalWrite(redLedPin, redLedState);
 *         }
 *       }
 *    
 *    
 *    
 *    
 *    III - 50 cm do objeto em questão, se aumentará a intensidade e as repetições seguidas do alerta sonoro;
 *    IV  - 30 cm do usuário da bengala, a mesma emitirá alertas muito frequentes e intensos;
 *    
 *    --------------------------------------------
 *    ---> FAZ ISSO na SEXTA-FEIRA
 *    I   - 
 *    II  - 
 */
#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"
SMSGSM sms;

int numdata;
boolean started=false;
char smsbuffer[160];
char n[20];



// *** Variáveis Sensores
// - Sensor de inclinação
int sensorIncli = 7;
// - Sensor ultrassônico A
int PinTrigger_A = 3; // Pino usado para disparar os pulsos do sensor
int PinEcho_A = 2; // Pino usado para ler a saida do sensor
// - Sensor ultrassônico B
int PinTrigger_B = 5; // Pino usado para disparar os pulsos do sensor
int PinEcho_B = 4; // pino usado para ler a saida do sensor
// - Sensor ultrassônico C
//int PinTrigger_C = 3; // Pino usado para disparar os pulsos do sensor
//int PinEcho_C = 2; // pino usado para ler a saida do sensor
// Buzzer 
const int buzzer = 8;

// Variáveis do Programa
long tempoAnterior = 0;
long tempoAtual = 0;
int tempo = 0; /// Talvez tira essa
int segundos = 0;
float tempoEcho_A = 0;
float tempoEcho_B = 0;
float tempoEcho_C = 0;
float distancia_A = 0;
float distancia_B = 0;
float distancia_C = 0;
const float VelocidadeSom_mporus = 0.00034029; // velocidade do som em metros por microsegundo 
//-> se der erro colocar velocida => 0.000340

void setup(){
  // - Prepara buzzer 
  pinMode(buzzer,OUTPUT); 
  
  // - Prepara sensor ultrassônico A
  pinMode(PinTrigger_A, OUTPUT);
  digitalWrite(PinTrigger_A, LOW);
  pinMode(PinEcho_A, INPUT); 
  
  // - Prepara sensor ultrassônico B
  pinMode(PinTrigger_B, OUTPUT);
  digitalWrite(PinTrigger_B, LOW);
  pinMode(PinEcho_B, INPUT);
  
//  // - Prepara sensor ultrassônico C
//  pinMode(PinTrigger_C, OUTPUT);
//  digitalWrite(PinTrigger_C, LOW);
//  pinMode(PinEcho_C, INPUT);

  // - Prepara sensor de inclinação 
   pinMode(sensorIncli, INPUT);

  // - Prepara GSM
  //Serial connection.
  Serial.begin(9600);
  Serial.println("GSM Shield testing.");
  //Start configuration of shield with baudrate.
  //For http uses is raccomanded to use 4800 or slower.
  if (gsm.begin(2400)){
    Serial.println("\nstatus=READY");
    started=true;  
  }
  else Serial.println("\nstatus=IDLE");
  
  
   

  // - Prepara botao
  //pinMode(buttonPin, INPUT); 
  
  // Inicializa a porta serial
  Serial.begin(4800);
  delay(100);

  // Se o BUZZER não funcionar é pq coloquei ele como primeiro e ele deve ser aqui -->
  // - Prepara buzzer ***VER SE Posso colocar antes de inicializar porta SERIAL
//  pinMode(buzzer,OUTPUT); 
//  Se o BUZZER Funcionar tirar todo esse comentario 
}

// Calcular a distancia em centímetros - ***Para Controle
float CalculaDistancia(float tempo_us){
  return((tempo_us*VelocidadeSom_mporus)/2) * 100;
}

// Ativa pulso do sensor ultrassônico
void DisparaPulsoUltrassonico(int PinTrigger){
  digitalWrite(PinTrigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(PinTrigger, LOW);
}

void loop(){
 
  // *** IMPORTANTE: tem que ler e fazer o que for um por vez dos sensores, se não da erro
  // - Sensor ultrassônico A
  DisparaPulsoUltrassonico(PinTrigger_A); // Envia pulso para o disparar o sensor ultrassônico A
  tempoEcho_A = pulseIn(PinEcho_A, HIGH); // Mede o tempo de duração do sinal no pino de leitura(us)
  distancia_A = CalculaDistancia(tempoEcho_A);  
  
  // - Sensor ultrassônico B
//  DisparaPulsoUltrassonico(PinTrigger_B); // Envia pulso para o disparar o sensor ultrassônico B
//  tempoEcho_B = pulseIn(PinEcho_B, HIGH); // Mede o tempo de duração do sinal no pino de leitura(us)
//  distancia_B = CalculaDistancia(tempoEcho_B); 
  
//  // - Sensor ultrassônico C
//  DisparaPulsoUltrassonico(PinTrigger_C); // Envia pulso para o disparar o sensor ultrassônico C
//  tempoEcho_C = pulseIn(PinEcho_C, HIGH); // Mede o tempo de duração do sinal no pino de leitura(us)
//  distancia_C = CalculaDistancia(tempoEcho_C); 

noTone(buzzer);  
  // *** Apenas para controle, saber a distância 
  Serial.print("\nDistancia em centimetros distancia_A: ");
  Serial.print(distancia_A);
  Serial.print("\nDistancia em centimetros distancia_B: ");
  Serial.print(distancia_B);
//  Serial.print("\nDistancia em centimetros distancia_C: ");
//  Serial.print(distancia_C);

  if(distancia_A <  10) {
     tone(buzzer,1200);   
  } else {   
    if(distancia_A <  30 ) {
      tone(buzzer,1200);
       for(int i = 0; i < 6; i++) {       
         tone(buzzer,1200);
         delay(50);
         noTone(buzzer);  
       }           
    } else {
        if(distancia_A <  50) {
          tone(buzzer,1500);
//          for(int i = 0; i < 4; i++) {       
//           tone(buzzer,1500);
//           noTone(buzzer);  
//          }    
      } else {
        if(distancia_A <  80) {
          tone(buzzer,1500);
//          for(int i = 0; i < 2; i++) {       
//           tone(buzzer,1500);
//           noTone(buzzer);  
//          } 
        } else {
          if(distancia_A  < 100) { 
            tone(buzzer,1500);    
          } else {
            noTone(buzzer);
          }
        }
      }
    }
  }

  // Dados do Sensor de Inclinação
  if(digitalRead(sensorIncli)) {
    Serial.println("\n\n----------------"); // Depois tirar isso
    tempoAnterior = millis();
    tempoAtual = 0;
    segundos = 0;
  } else {
    Serial.println("\n\nInclinado"); // Depois tirar isso
    tempoAtual = millis() - tempoAnterior;
  }

  if(tempoAtual >= 1000) {
    tempoAnterior = millis();
    tempoAtual = 0;
    segundos++;
  }

  if(segundos > 30) {
    tone(buzzer,700);
     if(started){
    //Read if there are messages on SIM card and print them.
    if(gsm.readSMS(smsbuffer, 160, n, 20))
    {
      Serial.println(n);
      Serial.println(smsbuffer);
    }
    if(started){
    //Enable this two lines if you want to send an SMS.
    if (sms.SendSMS("18991609780", "Cai estou na localizacao X ... Y"))
      Serial.println("\nSMS sent OK");
  }
    }
  }

  // ----- TALVEZ TIRA OU MANTEM se o de cina não funcionar ----->
   // Dados do Sensor de Inlcinação
//  if(digitalRead(sensorIncli)) {
//    Serial.println("\n\n----------------");
//    tempoAtual = 0;
//    segundos = 0;
//  } else {
//    Serial.println("\n\nInclinado");
//    tempo += 50; // Se funcionar o de cima tira esse 
//  }
//
//  if(tempoAtual >= 1000) {
//    segundos++;
//    tempo = 0;
//  }
//  
//  if(tempo >= 60) {
//    segundos++;
//    tempo = 0;
//  }
//
//  if(segundos >= 30) {
//    
//  }

  // ------- TIRAR ATÉ AQUI --------<<<<<

  Serial.print("\n\nTempo segundo: ");
  Serial.print(segundos);
  Serial.print("\n\n");
  
  
  // Delay para fazer refazer o código 
  delay(500);

  //TESTE
}
