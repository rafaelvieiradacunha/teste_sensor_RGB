#include <ESP32Servo.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"

//definição dos pinos e parametros do motor DC
//const int pinoENA = 25;
const int pinoIN1 = 26;
const int pinoIN2 = 27;

const int freqPWM = 5000;
const int resolucaoPWM = 8;

int velocidadeMotor = 150;
int presenca = 100;//definição do limiar de presença para a parada do motor

//definição do pino do servo moto
const int pinoServo = 13;

//definição do servo motor com o nome de servo1
Servo servo1;

//definição do sensor de cor(tcs) com o tempo de resposta a 50ms com um "ganho de cor" definido a 4x(para um abmiente com luminosidade neutra)
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

void setup() {
  Serial.begin(115200);

  //inicialização dos pinos do  motor
  pinMode(pinoIN1, OUTPUT);
  ledcAttach(pinoIN2, freqPWM, resolucaoPWM);

  //pinMode(pinoIN2, OUTPUT);
  //ledcAttach(pinoENA, freqPWM, resolucaoPWM);

  //inicialização da velocidade do motor
  digitalWrite(pinoIN1, LOW);
  ledcWrite(pinoIN2, velocidadeMotor);

  //digitalWrite(pinoIN2, LOW);
  //ledcWrite(pinoENA, velocidadeMotor);
  

  //inicialização do servo motor
  ESP32PWM::allocateTimer(1);//aloca um temporizador de hardware dedicado para gerar o sinal
  servo1.setPeriodHertz(50);//define a frequencia especifica do pulso enviado para o servo
  servo1.attach(pinoServo, 500, 2400);//"fala" para o esp qual o pino de comunicação do servo e define a rotação minima e maxima do servo

  //inicialização do sensor do sensor de cor
  if (tcs.begin()){
    Serial.println("Sensor reconhecido com sucesso!!");
  } else {
    Serial.println("Sensor não foi reconhecido, reveja as conexões");
    while(1);
  }

}

void loop() {
  //leitura dos dados brutos do sensor
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);


  Serial.printf("Luminosidade (C): "); Serial.print(c);
  Serial.print(" | Vermelho: "); Serial.print(r);
  Serial.print(" | Verde: "); Serial.print(g);
  Serial.print(" | Azul: "); Serial.print(b);

  //sistema de controle do servo motor
  if(c > presenca){
    //ledcWrite(pinoENA, 0);

    ledcWrite(pinoIN2, 0);//motor para ao detectar uma peça
    delay(100);//delay para que o motor pare totalmente

    if (r > g && r > b){
      Serial.println("PEÇA VERMELHA DETECTADA");
      servo1.write(22);
    }else if (g > b && g > r){
        Serial.println("PEÇA VERDE DETECTADA");
        servo1.write(67);
    }else if(b > g && b > r){
        Serial.println("PEÇA AZUL DETECTADA");
        servo1.write(0);
    }
    delay(1500);//delay para que o servo consiga trocar a posição totalmente

    //ledcWrite(pinoENA, velocidadeMotor);

    ledcWrite(pinoIN2, velocidadeMotor);
    delay(2000);//tempo para descartar uma peça

  }else{
    //ledcWrite(pinoENA, velocidadeMotor);
    
    ledcWrite(pinoIN2, velocidadeMotor);//motor segue normalmente
  }

  delay(100);//delay para não sobrecarregar o sistema
}