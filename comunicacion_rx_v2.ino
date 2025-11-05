#include <SPI.h>
#include <RF24.h>

// ============================
// Pines NRF24L01
// ============================
#define CE_PIN 22
#define CSN_PIN 21
#define SCK_PIN 18
#define MISO_PIN 19
#define MOSI_PIN 23

// ============================
// Pines Motores L298N
// ============================
#define AIN1_PIN  4   // IN1 Motor A
#define AIN2_PIN  2   // IN2 Motor A
#define BIN1_PIN  5   // IN3 Motor B
#define BIN2_PIN  15  // IN4 Motor B (cambié de 18 a 15 para no conflictuar con SCK)

// ============================
// NRF24L01 Setup
// ============================
RF24 radio(CE_PIN, CSN_PIN);
const byte addressRX[6] = "00001"; // Para recibir comandos
const byte addressTX[6] = "00002"; // Para enviar datos

// ============================
// Estructura de datos
// ============================
struct ComandoMotor {
  int comando;      // 0=STOP, 1=ADELANTE, 2=ATRAS, 3=IZQUIERDA, 4=DERECHA
  int velocidad;    // 0-255
};

struct DatosSensor {
  float temperatura;
  float humedad;
  int bateria;      // Porcentaje de batería (0-100)
};

ComandoMotor cmdRecibido;
DatosSensor datosSensor;

// ============================
// Control de motores
// ============================
enum EstadoMov {
  QUIETO = 0,
  ADELANTE = 1,
  ATRAS = 2,
  IZQUIERDA = 3,
  DERECHA = 4
};

EstadoMov estado_actual = QUIETO;
int velocidad = 150;
const unsigned long PERIODO_PWM_MS = 20;
unsigned long t_start_period = 0;

// Timers
unsigned long lastSensorSend = 0;
const unsigned long SENSOR_INTERVAL = 2000; // Enviar datos cada 2 segundos

// ============================
// Funciones de Motor
// ============================
void motor_on_state() {
  switch (estado_actual) {
    case ADELANTE:
      digitalWrite(AIN1_PIN, HIGH); digitalWrite(AIN2_PIN, LOW);
      digitalWrite(BIN1_PIN, HIGH); digitalWrite(BIN2_PIN, LOW);
      break;

    case ATRAS:
      digitalWrite(AIN1_PIN, LOW);  digitalWrite(AIN2_PIN, HIGH);
      digitalWrite(BIN1_PIN, LOW);  digitalWrite(BIN2_PIN, HIGH);
      break;

    case DERECHA:
      digitalWrite(AIN1_PIN, HIGH); digitalWrite(AIN2_PIN, LOW);
      digitalWrite(BIN1_PIN, LOW);  digitalWrite(BIN2_PIN, HIGH);
      break;

    case IZQUIERDA:
      digitalWrite(AIN1_PIN, LOW);  digitalWrite(AIN2_PIN, HIGH);
      digitalWrite(BIN1_PIN, HIGH); digitalWrite(BIN2_PIN, LOW);
      break;

    case QUIETO:
    default:
      digitalWrite(AIN1_PIN, LOW); digitalWrite(AIN2_PIN, LOW);
      digitalWrite(BIN1_PIN, LOW); digitalWrite(BIN2_PIN, LOW);
      break;
  }
}

void motor_all_off() {
  digitalWrite(AIN1_PIN, LOW); digitalWrite(AIN2_PIN, LOW);
  digitalWrite(BIN1_PIN, LOW); digitalWrite(BIN2_PIN, LOW);
}

void aplicarComando(int cmd, int vel) {
  estado_actual = (EstadoMov)cmd;
  velocidad = constrain(vel, 0, 255);
  
  Serial.printf("📡 Comando: %d, Velocidad: %d\n", cmd, velocidad);
}

// ============================
// Leer sensores simulados
// ============================
void leerSensores() {
  // Simulación de sensores (reemplazar con sensores reales)
  datosSensor.temperatura = 25.0 + random(-10, 10) * 0.5;
  datosSensor.humedad = 50.0 + random(-10, 10) * 0.5;
  datosSensor.bateria = random(70, 100);
}

// ============================
// Setup
// ============================
void setup() {
  Serial.begin(115200);
  Serial.println("🔧 Inicializando Receptor con Control de Motores...");

  // Configurar pines de motores
  pinMode(AIN1_PIN, OUTPUT);
  pinMode(AIN2_PIN, OUTPUT);
  pinMode(BIN1_PIN, OUTPUT);
  pinMode(BIN2_PIN, OUTPUT);
  motor_all_off();

  // Inicializar SPI
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN);

  // Iniciar NRF24L01
  while (!radio.begin()) {
    Serial.println("❌ Fallo al iniciar NRF24L01. Revisa conexiones...");
    delay(1000);
  }

  // Configurar radio para bidireccional
  radio.openWritingPipe(addressTX);    // Para enviar datos
  radio.openReadingPipe(1, addressRX); // Para recibir comandos
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.startListening();

  Serial.println("✅ Receptor listo. Esperando comandos...");
  
  t_start_period = millis();
  lastSensorSend = millis();
}

// ============================
// Loop Principal
// ============================
void loop() {
  unsigned long now = millis();

  // 1) Recibir comandos del transmisor
  if (radio.available()) {
    radio.read(&cmdRecibido, sizeof(cmdRecibido));
    aplicarComando(cmdRecibido.comando, cmdRecibido.velocidad);
  }

  // 2) Enviar datos de sensores periódicamente
  if (now - lastSensorSend >= SENSOR_INTERVAL) {
    lastSensorSend = now;
    leerSensores();
    
    radio.stopListening();
    bool ok = radio.write(&datosSensor, sizeof(datosSensor));
    radio.startListening();
    
    if (ok) {
      Serial.printf("📤 Datos enviados -> Temp: %.1f°C, Hum: %.1f%%, Bat: %d%%\n", 
                    datosSensor.temperatura, datosSensor.humedad, datosSensor.bateria);
    } else {
      Serial.println("⚠️ Fallo al enviar datos");
    }
  }

  // 3) Aplicar PWM por ráfagas
  unsigned long dt = now - t_start_period;
  if (dt >= PERIODO_PWM_MS) {
    t_start_period = now;
    dt = 0;
  }

  float duty = (float)velocidad / 255.0f;
  float on_time_ms = duty * PERIODO_PWM_MS;

  if (estado_actual == QUIETO) {
    motor_all_off();
  } else {
    if ((float)dt < on_time_ms) {
      motor_on_state();
    } else {
      motor_all_off();
    }
  }

  delay(1);
}