/*  
 *    *** ALARME DISPARA 
 *    I   - 2 m de distância, o levando a emitir alertas sonoros mais amenos, com repetições moderadas;
 *    II  - 80 cm do mesmo, esse alerta passa a se repetir com um menor intervalo de tempo;
 *    III - 50 cm do objeto em questão, se aumentará a intensidade e as repetições seguidas do alerta sonoro;
 *    IV  - 30 cm do usuário da bengala, a mesma emitirá alertas muito frequentes e intensos;
 */
#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"
SMSGSM sms;

int numdata;
boolean started=false;
char smsbuffer[160];
char n[20]; // Porta 50 e 51 mudamos na biblioteca



// *** Variáveis Sensores
// - Sensor de inclinação
const int sensorIncli = 31;
// - Sensor ultrassônico A
const int PinTrigger_A = 3; // Pino usado para disparar os pulsos do sensor
const int PinEcho_A = 2; // Pino usado para ler a saida do sensor
// - Sensor ultrassônico B
const int PinTrigger_B = 5; // Pino usado para disparar os pulsos do sensor
const int PinEcho_B = 4; // pino usado para ler a saida do sensor
// Buzzer 
const int buzzer = 8;
// Botão 
const int buttonPin = 11;

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
int estado;

boolean smsEnviado = false;

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

  // - Prepara sensor de inclinação 
   pinMode(sensorIncli, INPUT);

  // - Prepara GSM
//  //Serial connection.
//  Serial.println("GSM Shield testing.");
//  //Start configuration of shield with baudrate.
//  //For http uses is raccomanded to use 4800 or slower.
//  if (gsm.begin(2400)){
//    Serial.println("\nstatus=READY");
//    started=true;  
//  }
//  else Serial.println("\nstatus=IDLE");
  
  
   

  // - Prepara botao
  pinMode(buttonPin, INPUT); 
  
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
  DisparaPulsoUltrassonico(PinTrigger_B); // Envia pulso para o disparar o sensor ultrassônico B
  tempoEcho_B = pulseIn(PinEcho_B, HIGH); // Mede o tempo de duração do sinal no pino de leitura(us)
  distancia_B = CalculaDistancia(tempoEcho_B); 
  
//  // - Sensor ultrassônico C --- TALVEZ NÃO USAR
//  DisparaPulsoUltrassonico(PinTrigger_C); // Envia pulso para o disparar o sensor ultrassônico C
//  tempoEcho_C = pulseIn(PinEcho_C, HIGH); // Mede o tempo de duração do sinal no pino de leitura(us)
//  distancia_C = CalculaDistancia(tempoEcho_C); 


  // *** Apenas para controle, saber a distância 
  Serial.print("\nDistancia em centimetros distancia_A: ");
  Serial.print(distancia_A);
  Serial.print("\nDistancia em centimetros distancia_B: ");
  Serial.print(distancia_B);

  if(distancia_A <  10 || distancia_B < 20) {
     tone(buzzer,1200);   
  } else {   
    if(distancia_A <  30 || distancia_B < 60) {
      tone(buzzer,1200);  
      delay(110);
      noTone(buzzer);   
    } else {
        if(distancia_A <  50 || distancia_B < 80) {
          tone(buzzer,1200);  
          delay(200);
          noTone(buzzer);     
      } else {
        if(distancia_A <  80 || distancia_B < 110) {
          tone(buzzer,1200);  
          delay(600);
          noTone(buzzer);  
        } else {
          if(distancia_A  < 100 || distancia_B < 130) { 
            tone(buzzer,800);  
            delay(600);
            noTone(buzzer);  
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
    smsEnviado = false;
  } else {
    Serial.println("\n\nInclinado"); // Depois tirar isso
    tempoAtual = millis() - tempoAnterior;
  }

  if(tempoAtual >= 1000) {
    tempoAnterior = millis();
    tempoAtual = 0;
    segundos++;
  }

  if(segundos > 99999) { // Arrumar o TEMPO 
    tone(buzzer,700);
    if(!smsEnviado) { // *** TESTAR SE ISSO FAZ ENVIAR só uma MENSAGEM DEPOIS QUE CAIU
        if(started){
        //Read if there are messages on SIM card and print them.
        if(gsm.readSMS(smsbuffer, 160, n, 20)) {
          Serial.println(n);
          Serial.println(smsbuffer);
        }
        if(started){
          //Enable this two lines if you want to send an SMS.
          if (sms.SendSMS("18991609780", "Cai estou na localizacao X ... Y"))
          Serial.println("\nSMS sent OK");
        }
      }
      smsEnviado = true;
    }    
  }


  //Se o botao foi apertado
  if (digitalRead(buttonPin) == LOW){ // LOW = Botão precionado
        segundos = 0;
        smsEnviado = false;
        if(started){
          if(gsm.readSMS(smsbuffer, 160, n, 20)) {
            Serial.println(n);
            Serial.println(smsbuffer);
          }
          if(started){
            if (sms.SendSMS("18991609780", "Foi um alarme falso estou bem, apenas deixei a bengala cair"))
            Serial.println("\nSMS sent OK");
          }
        }
    }

  Serial.print("\n\nTempo segundo: ");
  Serial.print(segundos);
  Serial.print("\n\n");
  
  
  // Delay para fazer refazer o código 
  delay(50);

}
