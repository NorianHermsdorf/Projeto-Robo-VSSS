#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>

const int IN1 = 12;
const int IN2 = 13;
const int IN3 = 14;
const int IN4 = 27;

const int canal1 = 0;
const int canal2 = 1;
const int canal3 = 2;
const int canal4 = 3;

const int pinoADC = 34;
const int pinoR = 25;
const int pinoG = 26;
const int pinoB = 33;

const int freq = 2000;
const int resolucao = 8;

unsigned long ultimaComunicacao = 0;
unsigned long anteriorBlink = 0;
bool estadoLed = false;

void setup() {
  Serial.begin(115200);
  
  Dabble.begin("ROBO_VSSS"); 

  ledcSetup(canal1, freq, resolucao);
  ledcSetup(canal2, freq, resolucao);
  ledcSetup(canal3, freq, resolucao);
  ledcSetup(canal4, freq, resolucao);

  ledcAttachPin(IN1, canal1);
  ledcAttachPin(IN2, canal2);
  ledcAttachPin(IN3, canal3);
  ledcAttachPin(IN4, canal4);

  pinMode(pinoR, OUTPUT);
  pinMode(pinoG, OUTPUT);
  pinMode(pinoB, OUTPUT);

  Serial.println("Iniciado");
}

void loop() {
  Dabble.processInput();

  int vLeft = 0;
  int vRight = 0;

  int rawX = GamePad.getXaxisData();
  int rawY = GamePad.getYaxisData();

  if (rawX != 0 || rawY != 0) {
    ultimaComunicacao = millis();
    int jX = map(rawX, -7, 7, -255, 255);
    int jY = map(rawY, -7, 7, -255, 255);
    jX = (jX * abs(jX)) / 255;
    jY = (jY * abs(jY)) / 255;
    vLeft  = constrain(jY + jX, -180, 180); 
    vRight = constrain(jY - jX, -180, 180);
    Serial.println("X: ");
    Serial.println(jX);
    Serial.println("Y: ");
    Serial.println(jY);
    
  }

  if (millis() - ultimaComunicacao > 300) {
    vLeft = 0; vRight = 0;
  }

  setMotor(vLeft, vRight);
  gerenciarFeedbackBateria();
}

void setMotor(int left, int right) {
  controlMotor(left, canal1, canal2);
  controlMotor(right, canal3, canal4);
}

void controlMotor(int speed, int chA, int chB) {
  if (speed > 20) { 
    ledcWrite(chA, speed);
    ledcWrite(chB, 0);
  } else if (speed < -20) {
    ledcWrite(chA, 0);
    ledcWrite(chB, -speed);
  } else {
    ledcWrite(chA, 0); 
    ledcWrite(chB, 0);
  }
}

// Mantenha setRGB e gerenciarFeedbackBateria igual antes...
void setRGB(int r, int g, int b) {
  analogWrite(pinoR, r);
  analogWrite(pinoG, g);
  analogWrite(pinoB, b);
}

void gerenciarFeedbackBateria() {
  int leitura = analogRead(pinoADC);
  float tensao = (leitura * 3.3 / 4095.0) * 3.12; 
  if (tensao > 8.5) {
    setRGB(0, 0, 255);
  } else {
    if (millis() - anteriorBlink >= 500) {
      anteriorBlink = millis();
      estadoLed = !estadoLed;
      if (estadoLed) {
        if (tensao > 7.6) setRGB(0, 255, 0);
        else if (tensao > 7.1) setRGB(255, 100, 0);
        else setRGB(255, 0, 0);
      } else {
        setRGB(0, 0, 0);
      }
    }
  }
}
