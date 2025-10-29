#include <SPI.h>
#include <RF24.h>

// Pines CE y CSN (según tu conexión)
#define CE_PIN 22
#define CSN_PIN 21

// Pines SPI del ESP32 (puedes mantener los estándar)
#define SCK_PIN 18
#define MISO_PIN 19
#define MOSI_PIN 23

// Crear el objeto radio con los pines correctos
RF24 radio(CE_PIN, CSN_PIN);

// Dirección del canal de comunicación (debe coincidir con el transmisor)
const byte address[6] = "00001";

int angulo = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("🔧 Inicializando Receptor NRF24L01 en ESP32...");

  // Inicializar SPI con los pines definidos
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN);

  // Iniciar el módulo de radio
  while (!radio.begin()) {
    Serial.println("❌ Fallo al iniciar el módulo NRF24L01. Revisa conexiones o alimentación (3.3V).");
    delay(1000);
  }

  // Configurar el canal y parámetros de comunicación
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);     // RF24_PA_LOW mejora estabilidad en ESP32
  radio.setDataRate(RF24_250KBPS);   // Mejor alcance y menos errores
  radio.startListening();

  Serial.println("✅ Receptor listo. Esperando datos...");
}

void loop() {
  if (radio.available()) {
    radio.read(&angulo, sizeof(angulo));
    Serial.print("📡 Ángulo recibido: ");
    Serial.println(angulo);
  }
}
