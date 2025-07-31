#include <WiFi.h> // C:\Users\wjortiz\.platformio\penv\Scripts\platformio.exe run --target uploadfs
#include <PubSubClient.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "motor_control.h"  // tus funciones: avanzar(), detener(), girar... etc.

// ================= CONFIGURACION WiFi =================
const char* ssid = "CELERITY_BUESTAN";
const char* password = "098773MA";

// ============== CONFIGURACION MQTT ===================
const char* mqtt_server = "192.168.1.52";
const char* mqtt_topic = "pedidos/esp32";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// ========== CONEXION WiFi ============
void conectarWiFi() {
  Serial.print("Conectando a WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Conectado a WiFi");
  Serial.print("IP: "); Serial.println(WiFi.localIP());
}

// ========== FUNCION PARA EJECUTAR RUTA ============

void ejecutarRutaDesdeJSON(const String& nombreRuta) {
  File file = SPIFFS.open("/rutas.json", "r");
  if (!file) {
    Serial.println("No se pudo abrir el archivo JSON");
    return;
  }

  StaticJsonDocument<4096> doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.print("Error leyendo JSON: ");
    Serial.println(error.c_str());
    return;
  }

  if (!doc.containsKey(nombreRuta)) {
    Serial.println("Ruta no encontrada: " + nombreRuta);
    return;
  }

  JsonArray pasos = doc[nombreRuta].as<JsonArray>();
  for (JsonObject paso : pasos) {
    String accion = paso["accion"].as<String>();
    int duracion = paso["duracion"];

    Serial.println("Ejecutando: " + accion + " por " + String(duracion) + " ms");

    if (accion == "avanzar") avanzar(1023);
    else if (accion == "girarDerechaServos") girarDerechaServos();
    else if (accion == "girarIzquierdaServos") girarIzquierdaServos();
    else if (accion == "centro") direccionCentro();

    delay(duracion);
    detener();
    delay(500);
  }

  Serial.println("✅ Ruta ejecutada correctamente");
}


// ========== CALLBACK MQTT ============
void mqttCallback(char* topic, byte* payload, unsigned int length) {

    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.println("║     📦 NUEVO PEDIDO RECIBIDO 📦       ║");
    Serial.println("╚════════════════════════════════════════╝");

  String mensaje;
  for (unsigned int i = 0; i < length; i++) {
    mensaje += (char)payload[i];
  }

  Serial.println("📦 Ruta recibida: " + mensaje);
  ejecutarRutaDesdeJSON(mensaje);
}

// ========== CONEXION MQTT ============
void conectarMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Conectando a MQTT...");
    if (mqttClient.connect("ESP32RutaCliente")) {
      Serial.println("✅ Conectado al broker MQTT");
      mqttClient.subscribe(mqtt_topic);
      Serial.println("Suscrito al tópico MQTT: " + String(mqtt_topic));

    } else {
      Serial.print("❌ Error: ");
      Serial.println(mqttClient.state());
      delay(2000);
    }
  }
}

// ========== SETUP ============
void setup() {
  Serial.begin(115200);
  conectarWiFi();
  iniciarDrivers();
  
  if (!SPIFFS.begin(true)) {
    Serial.println("Fallo al montar SPIFFS");
    while (true) delay(1000);
  }

  // Mostrar contenido para debug
  File file = SPIFFS.open("/rutas.json", "r");
  if (!file) {
    Serial.println("No se pudo abrir rutas.json");
  } else {
    Serial.println("Contenido de rutas.json:");
    while (file.available()) {
      Serial.write(file.read());
    }
    file.close();
  }
  Serial.print("Tamaño del archivo rutas.json: ");
  Serial.println(file.size());  

  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.setCallback(mqttCallback);

  Serial.println("\n Sistema listo!");
}

// ========== LOOP PRINCIPAL ============
void loop() {
  if (!mqttClient.connected()) {
    conectarMQTT();
  }
  mqttClient.loop();

  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 10000) {
    mqttClient.subscribe(mqtt_topic);  
    lastCheck = millis();
  }
} 
