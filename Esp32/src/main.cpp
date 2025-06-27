#include <WiFi.h>
#include <WebServer.h>
#include "motor_control.h"

// WiFi
const char* ssid = "XXXXXXXXXXXXXXXX";
const char* password = "XXXXXXXXXXXXXXXX";

WebServer server(80);

String estadoSensor = "";

// Página HTML con botones grandes y estado de obstáculo
const char html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ProyectoDelivery</title>
  <style>
    body { font-family: sans-serif; text-align: center; padding: 20px; }
    button {
      font-size: 24px; padding: 15px 30px; margin: 10px;
      background-color: #4CAF50; color: white; border: none; border-radius: 10px;
    }
    .alerta { color: red; font-weight: bold; font-size: 20px; }
  </style>
</head>
<body>
  <h2>Control de Carro</h2>
  <div>
    <button onclick="fetch('/adelante')">Adelante</button>
    <button onclick="fetch('/retroceder')">Retroceder</button>
    <button onclick="fetch('/detener')">Detener</button><br>
    <button onclick="fetch('/izquierda')">Izquierda</button>
    <button onclick="fetch('/derecha')">Derecha</button>
    <button onclick="fetch('/centro')">Centro</button>
  </div>
  <br>
  <div id="estado" class="alerta"></div>

  <script>
    setInterval(() => {
      fetch('/estado').then(r => r.text()).then(data => {
        document.getElementById('estado').innerHTML = data;
      });
    }, 1000);
  </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  iniciarDrivers();
  direccionCentro();

  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ WiFi conectado");
  Serial.print("📡 Dirección IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", []() {
    server.send(200, "text/html", html);
  });

  server.on("/adelante", []() {
    if (leerDistanciaCM() < 20) {
      detener();
      estadoSensor = "🚫 Obstáculo detectado";
    } else {
      avanzar(1023);
      estadoSensor = "";
    }
    Serial.println("[WEB] Adelante");
    server.send(200, "text/plain", "OK");
  });

  server.on("/retroceder", []() {
    retroceder(1023);
    estadoSensor = "";
    Serial.println("[WEB] Retroceder");
    server.send(200, "text/plain", "OK");
  });

  server.on("/detener", []() {
    detener();
    estadoSensor = "";
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

  server.on("/estado", []() {
    server.send(200, "text/plain", estadoSensor);
  });

  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  server.handleClient();

  server.handleClient();

  // Detección continua de obstáculo
  static bool yaDetenido = false;
  long distancia = leerDistanciaCM();

  if (distancia > 0 && distancia < 20 && !yaDetenido) {
    detener();
    estadoSensor = "🚫 Obstáculo detectado (Detenido)";
    Serial.println("[SENSOR] Obstáculo detectado, deteniendo vehículo.");
    yaDetenido = true;
  }

  // Si el obstáculo desaparece, permite avanzar de nuevo
  if (distancia >= 20 && yaDetenido) {
    estadoSensor = "";
    yaDetenido = false;
    Serial.println("[SENSOR] Obstrucción despejada.");
  }
}
