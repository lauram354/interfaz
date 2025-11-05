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
const byte addressTX[6] = "00001"; // Para enviar comandos
const byte addressRX[6] = "00002"; // Para recibir datos

// --- Servidor Web ---
AsyncWebServer server(80);

// --- Estructuras de datos ---
struct ComandoMotor {
  int comando;      // 0=STOP, 1=ADELANTE, 2=ATRAS, 3=IZQUIERDA, 4=DERECHA
  int velocidad;    // 0-255
};

struct DatosSensor {
  float temperatura;
  float humedad;
  int bateria;
};

ComandoMotor cmdEnviar;
DatosSensor datosRecibidos;

// --- Variables globales ---
String lastCmd = "";
int currentVelocidad = 150;
TaskHandle_t rfTask;

unsigned long lastDataCheck = 0;
const unsigned long DATA_CHECK_INTERVAL = 100; // Revisar datos cada 100ms

// --- Prototipos de funciones ---
void rf24Task(void *parameter);

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

  // Configurar radio para bidireccional
  radio.openWritingPipe(addressTX);    // Para enviar comandos
  radio.openReadingPipe(1, addressRX); // Para recibir datos
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.stopListening(); // Modo transmisión por defecto
  Serial.println("✅ Módulo NRF24L01 listo.");

  // --- Servidor Web ---
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Servidor Robot Bidireccional OK");
  });

  // Endpoint para enviar comandos de movimiento
  server.on("/move", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("cmd")) {
      lastCmd = request->getParam("cmd")->value();
      Serial.println("Comando recibido: " + lastCmd);
    }
    if (request->hasParam("vel")) {
      currentVelocidad = request->getParam("vel")->value().toInt();
      currentVelocidad = constrain(currentVelocidad, 0, 255);
    }
    request->send(200, "application/json", "{\"status\":\"ok\"}");
  });

  // Endpoint para obtener datos del robot
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request) { 
    char json[256]; 
    snprintf(json, sizeof(json), 
             "{\"temperatura\": %.2f, \"humedad\": %.2f, \"bateria\": %d}", 
             datosRecibidos.temperatura, datosRecibidos.humedad, datosRecibidos.bateria);
    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", json); 
    response->addHeader("Access-Control-Allow-Origin", "*"); 
    response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS"); 
    response->addHeader("Access-Control-Allow-Headers", "Content-Type"); 
    request->send(response); 
  });

  // Endpoint CORS para opciones
  server.on("/data", HTTP_OPTIONS, [](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse(200);
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    response->addHeader("Access-Control-Allow-Headers", "Content-Type");
    request->send(response);
  });

  server.begin();
  Serial.println("🌐 Servidor iniciado correctamente.");

  // --- Crear tarea RF24 ---
  xTaskCreatePinnedToCore(
    rf24Task, "RF24Task", 8192, NULL, 1, &rfTask, 0
  );
}

void loop() {
  // Loop vacío, todo en tareas
}

// --- Tarea RF24 Bidireccional ---
void rf24Task(void *parameter) {
  for (;;) {
    unsigned long now = millis();

    // 1) Enviar comando si hay uno pendiente
    if (lastCmd != "") {
      int comando = 0; // STOP por defecto
      
      if (lastCmd == "forward" || lastCmd == "1") comando = 1;
      else if (lastCmd == "backward" || lastCmd == "2") comando = 2;
      else if (lastCmd == "left" || lastCmd == "3") comando = 3;
      else if (lastCmd == "right" || lastCmd == "4") comando = 4;
      else if (lastCmd == "stop" || lastCmd == "0") comando = 0;

      cmdEnviar.comando = comando;
      cmdEnviar.velocidad = currentVelocidad;

      radio.stopListening();
      bool ok = radio.write(&cmdEnviar, sizeof(cmdEnviar));
      
      if (ok) {
        Serial.printf("✅ Comando enviado -> Cmd: %d, Vel: %d\n", 
                      cmdEnviar.comando, cmdEnviar.velocidad);
      } else {
        Serial.println("⚠️ Fallo al enviar comando RF");
      }

      lastCmd = "";
      vTaskDelay(50 / portTICK_PERIOD_MS);
    }

    // 2) Escuchar datos del receptor periódicamente
    if (now - lastDataCheck >= DATA_CHECK_INTERVAL) {
      lastDataCheck = now;
      
      radio.startListening();
      vTaskDelay(50 / portTICK_PERIOD_MS); // Dar tiempo para recibir

      if (radio.available()) {
        radio.read(&datosRecibidos, sizeof(datosRecibidos));
        Serial.printf("📡 Datos recibidos -> Temp: %.1f°C, Hum: %.1f%%, Bat: %d%%\n",
                      datosRecibidos.temperatura, datosRecibidos.humedad, datosRecibidos.bateria);
      }
      
      radio.stopListening();
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}