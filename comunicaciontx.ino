#include <SPI.h>
#include <RF24.h>

// Pines SPI del ESP32
#define SCK_PIN 18
#define MISO_PIN 19
#define MOSI_PIN 23

// Pines del NRF24L01
#define CE_PIN 22
#define CSN_PIN 21

// Pin analógico del ESP32 (usa un pin ADC válido)
#define ANALOG_PIN 34

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "00001";

void setup() {
  Serial.begin(115200);
  Serial.println("🔧 Inicializando Transmisor NRF24L01 en ESP32...");

  // Iniciar SPI con los pines del ESP32
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN);

  // Reintentar hasta que el NRF24L01 responda
  while (!radio.begin()) {
    Serial.println("❌ Fallo al iniciar NRF24L01. Reintentando...");
    delay(1000);
  }
  if (radio.begin()){
    Serial.println("Inicio transmision");
  }

  // Configuración del transmisor
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);      // más estable con ESP32
  radio.setDataRate(RF24_250KBPS);    // mejor alcance
  radio.stopListening();

  Serial.println("✅ Transmisor listo y enviando datos...");
}

void loop() {
  // Leer entrada analógica (0–4095 en ESP32)
  int lecturaADC = analogRead(ANALOG_PIN);

  // Mapear el valor a un rango de 0–180
  int angulo = 5;

  // Enviar el valor
  bool ok = radio.write(&angulo, sizeof(angulo));

  if (ok) {
    Serial.print("📡 Ángulo enviado: ");
    Serial.println(angulo);
  } else {
    Serial.println("⚠️ Error al enviar. Receptor no responde.");
  }

  delay(100);
}
