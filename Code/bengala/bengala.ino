#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"
SMSGSM sms;

int numdata;
boolean started = false;
char smsbuffer[160];
char n[20]; // Porta 50 e 51 foi mudado na biblioteca

// -- Constantes Sensores
const int sensorIncli = 31;
const int pinTrigger_A = 3; 
const int pinEcho_A = 2; 
const int pinTrigger_B = 5; 
const int pinEcho_B = 4; 
const int buzzer = 8;
const int botao = 11;

// --- Variáveis do Programa
long tempoAnterior = 0;
long tempoAtual = 0;
//int tempo = 0; /// Talvez tira essa VERIFICAR, acho foi variável usada quem mudei para tempoAnterior e tempo Atual
int segundos = 0;
float tempoEcho_A = 0;
float tempoEcho_B = 0;
float distancia_A = 0;
float distancia_B = 0;
const float velocidadeSom_ms = 0.00034029; 
boolean smsEnviado = false;
boolean alarmeQueda = false;

void setup(){
  
  pinMode(buzzer,OUTPUT); 
  
  pinMode(botao, INPUT);
  
  pinMode(pinTrigger_A, OUTPUT);
  digitalWrite(pinTrigger_A, LOW);
  pinMode(pinEcho_A, INPUT); 
  

  pinMode(pinTrigger_B, OUTPUT);
  digitalWrite(pinTrigger_B, LOW);
  pinMode(pinEcho_B, INPUT);
  
   pinMode(sensorIncli, INPUT);

  // - Prepara GSM
  Serial.println("GSM Shield testing.");
  if (gsm.begin(2400)){
    Serial.println("\nstatus=READY");
    started=true;  
  }
  else Serial.println("\nstatus=IDLE");
  
  Serial.begin(4800);
  delay(100);

}

float calculaDistancia(float tempo_us){
  return((tempo_us*velocidadeSom_ms)/2) * 100;
}

void disparaPulsoUltrassonico(int pinTrigger){
  digitalWrite(pinTrigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrigger, LOW);
}

void leituraSensorUltrassonico(int pinTrigger, int pinEcho, float tempoEcho, float distancia) {
  disparaPulsoUltrassonico(pinTrigger); 
  tempoEcho = pulseIn(pinEcho, HIGH); 
  distancia = calculaDistancia(tempoEcho);  
}

void alertaProximidade() {
  if(!alarmeQueda) {
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
  }    
}

void infoSenInclinacao() {
  if(digitalRead(sensorIncli)) {
    Serial.println("\n\n----------------");
    tempoAnterior = millis();
    tempoAtual = 0;
    segundos = 0;
    alarmeAjuda = false;
  } else {
    Serial.println("\n\nInclinado");
    tempoAtual = millis() - tempoAnterior;
    alarmeAjuda = true;
  }
}

void contInclinado() {
  if(tempoAtual >= 1000) {
    tempoAnterior = millis();
    tempoAtual = 0;
    segundos++;
  }
}

void pedidoAjuda() {
  if(segundos > 30 && !smsEnviado) { // Se passar de 30s que caiu irá automaticamente enviar uma mensagem
      if(started){
      if(gsm.readSMS(smsbuffer, 160, n, 20)) {
        Serial.println(n);
        Serial.println(smsbuffer);
      }
      if(started){
        if (sms.SendSMS("18991609780", "Cai estou na localizacao X ... Y"))
        Serial.println("\nSMS sent OK");
      }
    }
    smsEnviado = true;   
  }
}

void cancelarPedidoAjuda() {
  if (digitalRead(botao) == LOW){ // Se o botão for apertado e solto, por aproximadamente 1 segundo
    segundos = -20;
    alarmeQueda = false;
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
}

void alarmeAjuda() {
  if(alarmeQueda) {
    delay(500);
    tone(buzzer,1200);  
    delay(200);
    noTone(buzzer); 
  }
}

void exibirDistancia() {
  Serial.print("\nDistancia em centimetros distancia_A: ");
  Serial.print(distancia_A);
  Serial.print("\nDistancia em centimetros distancia_B: ");
  Serial.print(distancia_B);
}

void exibirTempoSegundos() {
  Serial.print("\n\nTempo segundo: ");
  Serial.print(segundos);
  Serial.print("\n\n");
}

void loop(){
 
  leituraSensorUltrassonico(pinTrigger_A, pinEcho_A, tempoEcho_A, distancia_A); 
  leituraSensorUltrassonico(pinTrigger_B, pinEcho_B, tempoEcho_B, distancia_B);
  exibirDistancia();
  alertaProximidade();  
  infoSenInclinacao();
  contInclinado();
  pedidoAjuda();
  alarmeAjuda()
  cancelarPedidoAjuda();
  exibirTempoSegundos();
  
}
