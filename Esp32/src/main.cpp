#include <Arduino.h>
#include <ESP32Servo.h>
#include "motor_control.h"

bool adelante = true;
unsigned long tiempo = 0;

void setup() {
  iniciarDrivers();
  avanzar(1023);
  
  
  tiempo = millis();
}

void loop() {
  direccionCentro(); // posición inicial
  //girarIzquierdaServos();
  //girarDerechaServos();
  if (millis() - tiempo >= 2000) {

    detener();            
    delay(1000);           

    if (adelante) {
       
      retroceder(1023);  //Velocidad 512/50%  768/75%   1023/100%
    } else {
      
      avanzar(1023);     
    }
    adelante = !adelante;
    tiempo = millis();  
  }
}
