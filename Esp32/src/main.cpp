#include <WiFi.h>
#include <WebServer.h>
#include <Arduino.h>
#include "motor_control.h"

// Credenciales WiFi
const char* ssid = "CELERITY_BUESTAN";
const char* password = "098773MA";

// Servidor web en el puerto 80
WebServer server(80);

// HTML simple con botones
const char html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head><title>Proyecto Delivery</title></head>
<body style="text-align:center;font-family:sans-serif;">
  <h2>Control de Carro</h2>
  <button onclick="fetch('/adelante')">Adelante</button>
  <button onclick="fetch('/retroceder')">Retroceder</button>
  <button onclick="fetch('/detener')">Detener</button><br><br>
  <button onclick="fetch('/izquierda')">Izquierda</button>
  <button onclick="fetch('/derecha')">Derecha</button>
  <button onclick="fetch('/centro')">Centro</button>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  iniciarDrivers();

  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", []() {
    server.send(200, "text/html", html);
  });

  server.on("/adelante", []() {
    avanzar(1023);
    Serial.println("[WEB] Adelante");
    server.send(200, "text/plain", "OK");
  });

  server.on("/retroceder", []() {
    retroceder(1023);
    Serial.println("[WEB] Retroceder");
    server.send(200, "text/plain", "OK");
  });

  server.on("/detener", []() {
    detener();
    Serial.println("[WEB] Detener");
    server.send(200, "text/plain", "OK");
  });

  server.on("/izquierda", []() {
    girarIzquierdaServos();
    Serial.println("[WEB] Izquierda");
    server.send(200, "text/plain", "OK");
  });

  server.on("/derecha", []() {
    girarDerechaServos();
    Serial.println("[WEB] Derecha");
    server.send(200, "text/plain", "OK");
  });

  server.on("/centro", []() {
    direccionCentro();
    Serial.println("[WEB] Centro");
    server.send(200, "text/plain", "OK");
  });

  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  server.handleClient();
}
