#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>

// Pinos Motores
const int IN1 = 4;
const int IN2 = 5;
const int IN3 = 6;
const int IN4 = 7;

// Pinos LED RGB e ADC (Bateria)
const int pinoADC = 1;
const int pinoR = 10;
const int pinoG = 20;
const int pinoB = 21;

// Configuração PWM
const int freq = 2000;
const int resolucao = 8;

unsigned long ultimaComunicacao = 0;
unsigned long anteriorBlink = 0;
bool estadoLed = false;
bool carregando = false; 

void setup() {
  Serial.begin(115200);
  Dabble.begin("ROBO_VSSS");

  ledcAttach(IN1, freq, resolucao);
  ledcAttach(IN2, freq, resolucao);
  ledcAttach(IN3, freq, resolucao);
  ledcAttach(IN4, freq, resolucao);

  // Configurar pinos do LED
  pinMode(pinoR, OUTPUT);
  pinMode(pinoG, OUTPUT);
  pinMode(pinoB, OUTPUT);

  Serial.println("Sistema Iniciado...");
}

void loop() {
  Dabble.processInput();

  int vLeft = 0;
  int vRight = 0;

  if (GamePad.isConnected()) {
    int rawX = GamePad.getXaxisData();
    int rawY = GamePad.getYaxisData();

    if (rawX != 0 || rawY != 0) {
      ultimaComunicacao = millis(); // Só reseta se houver movimento

      int jX = map(rawX, -7, 7, -255, 255);
      int jY = map(rawY, -7, 7, -255, 255);

      jX = (jX * abs(jX)) / 255;
      jY = (jY * abs(jY)) / 255;

      vLeft  = constrain(jY + jX, -180, 180); // Limite de 6V (aprox 70% de 8.4V)
      vRight = constrain(jY - jX, -180, 180);
    }
  }

  // Fail-safe: Se ficar 300ms sem comando, zera os motores
  if (millis() - ultimaComunicacao > 300) {
    vLeft = 0;
    vRight = 0;
  }

  setMotor(vLeft, vRight);
  gerenciarFeedbackBateria();
}

void setMotor(int left, int right) {
  controlMotor(IN1, IN2, left);
  controlMotor(IN3, IN4, right);
}

void controlMotor(int pinA, int pinB, int speed) {
  if (speed > 20) { 
    ledcWrite(pinA, speed);
    ledcWrite(pinB, 0);
  } else if (speed < -20) {
    ledcWrite(pinA, 0);
    ledcWrite(pinB, -speed);
  } else {
    // Freio ativo para precisão no VSSS
    ledcWrite(pinA, 0); 
    ledcWrite(pinB, 0);
  }
}

void setRGB(int r, int g, int b) {
  analogWrite(pinoR, r);
  analogWrite(pinoG, g);
  analogWrite(pinoB, b);
}

void gerenciarFeedbackBateria() {
  int leitura = analogRead(pinoADC);

  float tensao = (leitura * 3.3 / 4095.0) * 3.12; 
  
  carregando = (tensao > 8.5); 

  if (carregando) {
    setRGB(0, 0, 255); // Azul fixo carregando
  } else {
    if (millis() - anteriorBlink >= 500) {
      anteriorBlink = millis();
      estadoLed = !estadoLed;
      
      if (estadoLed) {
        if (tensao > 7.6) setRGB(0, 255, 0);      // Verde
        else if (tensao > 7.1) setRGB(255, 100, 0); // Laranja
        else setRGB(255, 0, 0);                  // Vermelho
      } else {
        setRGB(0, 0, 0);
      }
    }
  }
}
