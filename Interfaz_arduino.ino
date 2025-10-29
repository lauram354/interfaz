#include <WiFi.h>
#include <SPI.h>
#include <RF24.h>
#include <ESPAsyncWebServer.h>

// --- Pines NRF24 ---
#define CE_PIN 22
#define CSN_PIN 21
#define SCK_PIN 18
#define MISO_PIN 19
#define MOSI_PIN 23

// --- Credenciales WiFi ---
const char* ssid = "gandalf";
const char* password = "gandalf2025";

// --- NRF24 ---
RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "00001";

// --- Servidor Web ---
AsyncWebServer server(80);

// --- Variables globales ---
String lastCmd = "";
TaskHandle_t rfTask;
TaskHandle_t sensorTaskHandle;

float temp = 25.0;
float humedad = 50.0;
unsigned long lastUpdate = 0;
const unsigned long updateInterval = 3000;

// --- Prototipos de funciones ---
void rf24Task(void *parameter);
void sensorTask(void *parameter);

void setup() {
  Serial.begin(115200);

  // --- WiFi ---
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi conectado");
  Serial.println(WiFi.localIP());

  // --- SPI + NRF ---
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN);
  while (!radio.begin()) {
    Serial.println("❌ Fallo al iniciar NRF24L01. Reintentando...");
    delay(1000);
  }

  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.stopListening();
  Serial.println("✅ Módulo NRF24L01 listo.");

  // --- Servidor Web ---
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Servidor funcionando con NRF24!");
  });

  server.on("/move", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("cmd")) {
      lastCmd = request->getParam("cmd")->value();
      Serial.println("Comando recibido: " + lastCmd);
    }
    request->send(200, "application/json", "{\"status\":\"ok\"}");
  });

  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request) { 
    char json[128]; 
    snprintf(json, sizeof(json), "{\"temperatura\": %.2f, \"humedad\": %.2f}", temp, humedad);
    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", json); 
    response->addHeader("Access-Control-Allow-Origin", "*"); 
    response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS"); 
    response->addHeader("Access-Control-Allow-Headers", "Content-Type"); 
    request->send(response); 
  });

  server.begin();
  Serial.println("🌐 Servidor iniciado correctamente.");

  // --- Crear tareas ---
  xTaskCreatePinnedToCore(
    rf24Task, "RF24Task", 4096, NULL, 1, &rfTask, 0
  );

  xTaskCreatePinnedToCore(
    sensorTask, "SensorTask", 4096, NULL, 1, &sensorTaskHandle, 1
  );
}

void loop() {
  // No se usa loop, todo corre en tareas
}

// --- Tarea RF24 ---
void rf24Task(void *parameter) {
  for (;;) {
    if (lastCmd != "") {
      int valor = lastCmd.toInt();
      bool ok = radio.write(&valor, sizeof(valor));

      if (ok) {
        Serial.printf("✅ Enviado correctamente -> %d\n", valor);
      } else {
        Serial.println("⚠️ Fallo al enviar RF");
      }

      lastCmd = "";
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// --- Tarea Sensor ---
void sensorTask(void *parameter) {
  for (;;) {
    unsigned long currentMillis = millis();
    if (currentMillis - lastUpdate > updateInterval) {
      lastUpdate = currentMillis;

      temp += random(-5, 5) * 0.1;
      humedad += random(-3, 3) * 0.2;

      temp = constrain(temp, 15, 35);
      humedad = constrain(humedad, 30, 90);

      Serial.printf("📡 Datos simulados -> Temp: %.2f °C | Humedad: %.2f %%\n", temp, humedad);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
