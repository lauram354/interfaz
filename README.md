# 🤖 Interfaz Web de Control y Monitoreo — ESP32 + React

Este proyecto permite **controlar un robot** con una **ESP32** y **visualizar datos de sensores** (como temperatura y humedad) desde una **interfaz web desarrollada en React**.  
La comunicación entre el frontend y la ESP32 se realiza mediante **HTTP requests locales**, sin necesidad de conexión a internet externa.

---

## 🚀 Requisitos

### 🧩 Frontend
- [Node.js](https://nodejs.org/) (versión 16 o superior)
- [npm](https://www.npmjs.com/) (incluido con Node)
- Red WiFi local compartida entre la PC y la ESP32

### ⚙️ Backend (ESP32)
- [Arduino IDE](https://www.arduino.cc/en/software) o [PlatformIO](https://platformio.org/)
- Placa ESP32 (ej. ESP32 DevKit v1)
- Módulo RF24L01 (opcional para comunicación inalámbrica entre robots)
- Red WiFi doméstica (2.4 GHz)

---

## 📦 Instalación del Frontend

1. Clona o descarga este repositorio:
   ```bash
   git clone https://github.com/tu_usuario/esp32-react-control.git
   cd esp32-react-control
2. Instala las dependencias:

`npm install`
##⚙️ Configuración del Frontend

Abre src/App.jsx y cambia la constante con la IP local de tu ESP32:

const ESP_IP = "http://192.168.0.4"; // Cambia esta IP por la tuya

Para obtener la IP, conéctate al monitor serie del Arduino IDE y busca una línea similar a:

🌐 IP local: 192.168.0.4

▶️ Ejecución del Frontend

Ejecuta el entorno de desarrollo con:

npm run dev

## ⚙️ Configuración del Firmware (ESP32)

Esta sección describe cómo preparar y programar la **ESP32** para comunicarse con el frontend en React y, opcionalmente, con otros módulos mediante **RF24L01**.

---

### 🧰 Librerías necesarias

Antes de compilar el código, asegúrate de tener instaladas las siguientes librerías en el **Arduino IDE** (o en tu entorno PlatformIO):

| Librería | Autor / Fuente | Descripción |
|-----------|----------------|--------------|
| **WiFi** | Incluida en el core de ESP32 | Permite la conexión a la red WiFi |
| **SPI** | Incluida por defecto | Soporte de comunicación SPI para NRF24 |
| **RF24** | TMRh20 | Comunicación inalámbrica con módulos NRF24L01 |
| **ESPAsyncWebServer** | me-no-dev / esphome | Servidor web asíncrono con soporte para múltiples rutas |
| **AsyncTCP** | me-no-dev | Dependencia requerida por `ESPAsyncWebServer` |

> 🔧 **Instalación rápida desde el Arduino IDE:**
>
> 1. Abre **Herramientas → Administrar bibliotecas...**
> 2. Busca e instala:
>    - `RF24` by TMRh20  
>    - `ESP Async WebServer`  
>    - `AsyncTCP`  
>
> Si usas **PlatformIO**, añade esto a tu archivo `platformio.ini`:
> ```ini
> lib_deps = 
>   TMRh20/RF24
>   me-no-dev/ESP Async WebServer
>   me-no-dev/AsyncTCP
> ```

---

### 🔌 Conexiones de hardware (opcional para RF24)

Si vas a usar los módulos **NRF24L01** para comunicación inalámbrica entre robots o nodos:

| RF24L01 | ESP32 |
|----------|--------|
| VCC | 3.3V |
| GND | GND |
| CE | GPIO 4 |
| CSN | GPIO 5 |
| SCK | GPIO 18 |
| MOSI | GPIO 23 |
| MISO | GPIO 19 |

> ⚠️ **Nota:** No conectes el NRF24L01 a 5V — funciona únicamente a **3.3V**.

---

### 🖥️ Código base de la ESP32

El siguiente código implementa:
- Un servidor HTTP con dos rutas:
  - `/move?cmd=<comando>` → recibe instrucciones desde la interfaz web.
  - `/data` → devuelve valores simulados de sensores (temperatura y humedad).
- Soporte CORS para permitir la conexión desde el frontend React.
- Simulación de envío de datos por RF (comentado).

📄 **Código completo:**

```cpp
#include <WiFi.h>
#include <SPI.h>
#include <RF24.h>
#include <ESPAsyncWebServer.h>

// --- Configura tu red WiFi ---
const char* ssid = "red";
const char* password = "contrasenia";

// --- NRF24 ---
// RF24 radio(4, 5); // CE, CSN
// const byte address[6] = "00001";

// --- Servidor HTTP asíncrono ---
AsyncWebServer server(80);

// --- Variables simuladas ---
float temp = 25.0;
float humedad = 50.0;
unsigned long lastUpdate = 0;
const unsigned long updateInterval = 3000; // cada 3 segundos

void setup() {
  Serial.begin(115200);

  // --- Conexión WiFi ---
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("✅ Conectado a WiFi");
  Serial.print("🌐 IP local: ");
  Serial.println(WiFi.localIP());

  // --- NRF24 (descomenta cuando lo conectes) ---
  // radio.begin();
  // radio.openWritingPipe(address);
  // radio.setPALevel(RF24_PA_LOW);
  // radio.stopListening();

  // --- RUTA PRINCIPAL ---
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Servidor ESP32 con RF24 y simulación de sensores listo!");
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    response->addHeader("Access-Control-Allow-Headers", "Content-Type");
    request->send(response);
  });

  // --- RUTA PARA CONTROL DE MOVIMIENTO ---
  server.on("/move", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("cmd")) {
      String cmd = request->getParam("cmd")->value();
      Serial.println("Comando recibido: " + cmd);

      // Simula envío por RF
      // radio.write(&cmd, sizeof(cmd));

      // Simula acción local
      Serial.println("📡 Simulando envío RF del comando: " + cmd);
    }

    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", "{\"status\":\"ok\"}");
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    response->addHeader("Access-Control-Allow-Headers", "Content-Type");
    request->send(response);
  });

  // --- Manejo de preflight OPTIONS (CORS) ---
  server.on("/move", HTTP_OPTIONS, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(204);
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    response->addHeader("Access-Control-Allow-Headers", "Content-Type");
    request->send(response);
  });

  // --- NUEVA RUTA: datos simulados ---
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request) {
    char json[128];
    snprintf(json, sizeof(json),
             "{\"temperatura\": %.2f, \"humedad\": %.2f}",
             temp, humedad);

    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", json);
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    response->addHeader("Access-Control-Allow-Headers", "Content-Type");
    request->send(response);
  });

  // --- Iniciar servidor ---
  server.begin();
  Serial.println("🌍 Servidor iniciado correctamente");
}

void loop() {
  // --- Simulación de datos cada 3 segundos ---
  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdate > updateInterval) {
    lastUpdate = currentMillis;

    // Simula variaciones
    temp += random(-5, 5) * 0.1;
    humedad += random(-3, 3) * 0.2;

    // Asegura rangos válidos
    if (temp < 15) temp = 15;
    if (temp > 35) temp = 35;
    if (humedad < 30) humedad = 30;
    if (humedad > 90) humedad = 90;

    // Simula recepción RF
    Serial.print("📡 Datos simulados -> Temp: ");
    Serial.print(temp);
    Serial.print(" °C | Humedad: ");
    Serial.print(humedad);
    Serial.println(" %");
  }
}

