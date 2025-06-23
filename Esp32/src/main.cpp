#include <Arduino.h>
#include <ESP32Servo.h>
#include "motor_control.h"

bool adelante = true;
unsigned long tiempo = 0;

void setup() {
  iniciarDrivers();
  avanzar(512);
  tiempo = millis();
}

void loop() {
  
  if (millis() - tiempo >= 2000) {

    detener();            
    delay(1000);           

    if (adelante) {
      retroceder(512);  //Velocidad 512/50%  768/75%   1023/100%
    } else {
      avanzar(512);     
    }
    adelante = !adelante;
    tiempo = millis();  
  }
}
