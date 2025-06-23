#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>
#include <ESP32Servo.h>

// Pines para cada driver (LPWM y RPWM)
#define DRIVER1_LPWM 22
#define DRIVER1_RPWM 23

#define DRIVER2_LPWM 21
#define DRIVER2_RPWM 19

#define DRIVER3_LPWM 18
#define DRIVER3_RPWM 5

// Pines para servomotores de dirección
#define SERVO1_PIN 17
#define SERVO2_PIN 16

// Canales PWM para ESP32 (uno por señal PWM)
#define CH_D1_LPWM 0
#define CH_D1_RPWM 1
#define CH_D2_LPWM 2
#define CH_D2_RPWM 3
#define CH_D3_LPWM 4
#define CH_D3_RPWM 5

#define PWM_FREQ 1000
#define PWM_RES 10

// Instancias para los dos servos
Servo servo1;
Servo servo2;

void iniciarDrivers() {
  // Driver 1
  ledcSetup(CH_D1_LPWM, PWM_FREQ, PWM_RES);
  ledcAttachPin(DRIVER1_LPWM, CH_D1_LPWM);
  ledcSetup(CH_D1_RPWM, PWM_FREQ, PWM_RES);
  ledcAttachPin(DRIVER1_RPWM, CH_D1_RPWM);

  // Driver 2
  ledcSetup(CH_D2_LPWM, PWM_FREQ, PWM_RES);
  ledcAttachPin(DRIVER2_LPWM, CH_D2_LPWM);
  ledcSetup(CH_D2_RPWM, PWM_FREQ, PWM_RES);
  ledcAttachPin(DRIVER2_RPWM, CH_D2_RPWM);

  // Driver 3
  ledcSetup(CH_D3_LPWM, PWM_FREQ, PWM_RES);
  ledcAttachPin(DRIVER3_LPWM, CH_D3_LPWM);
  ledcSetup(CH_D3_RPWM, PWM_FREQ, PWM_RES);
  ledcAttachPin(DRIVER3_RPWM, CH_D3_RPWM);

  // Servos
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);

  // Inicializa dirección centrada
  servo1.write(90);
  servo2.write(90);

}

void avanzar(int duty) {
  ledcWrite(CH_D1_LPWM, duty); ledcWrite(CH_D1_RPWM, 0);
  ledcWrite(CH_D2_LPWM, duty); ledcWrite(CH_D2_RPWM, 0);
  ledcWrite(CH_D3_LPWM, duty); ledcWrite(CH_D3_RPWM, 0);
}

void retroceder(int duty) {
  ledcWrite(CH_D1_LPWM, 0); ledcWrite(CH_D1_RPWM, duty);
  ledcWrite(CH_D2_LPWM, 0); ledcWrite(CH_D2_RPWM, duty);
  ledcWrite(CH_D3_LPWM, 0); ledcWrite(CH_D3_RPWM, duty);
}

void detener() {
  ledcWrite(CH_D1_LPWM, 0);
  ledcWrite(CH_D1_RPWM, 0);
  ledcWrite(CH_D2_LPWM, 0);
  ledcWrite(CH_D2_RPWM, 0);
  ledcWrite(CH_D3_LPWM, 0);
  ledcWrite(CH_D3_RPWM, 0);
}

// --------- FUNCIONES PARA SERVOS DE DIRECCIÓN ---------

void direccionCentro() {
  servo1.write(90);
  servo2.write(90);
}

void girarIzquierdaServos() {
  servo1.write(60);  // Ajusta según el montaje
  servo2.write(60);
}

void girarDerechaServos() {
  servo1.write(120); // Ajusta según el montaje
  servo2.write(120);
}


#endif
