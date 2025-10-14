#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// ========================================
// CONFIGURAÇÕES DO USUÁRIO
// ========================================
#define WIFI_SSID        "GABRIEL OI"
#define WIFI_PASSWORD    "85139584Gg#"
#define API_KEY          "AIzaSyCkVxnJ7TA3MgXOsDvDpxj8HGg-SY7NZzA"
#define DATABASE_URL     "https://tcc-gabriel-9a0f9-default-rtdb.firebaseio.com/"
#define DEVICE_ID        "esp32-vaso-01"

// ========================================
// PINOS E CONSTANTES
// ========================================
#define PIN_PUMP_RELAY   4      // ✅ GPIO 4 -> TIP127 -> Relé
#define PIN_SOIL_SENSOR  34     // ✅ GPIO 34 (ADC1_CH6) -> Sensor de Umidade

// ========================================
// CALIBRAÇÃO DO SENSOR
// ========================================
#define SENSOR_DRY   3100  // Valor ADC quando solo está SECO (ar)
#define SENSOR_WET   1400  // Valor ADC quando solo está MOLHADO (água)

// Intervalos de tempo
const unsigned long HEARTBEAT_INTERVAL_MS = 60000;  // 60s
const unsigned long TELEMETRY_INTERVAL_MS = 10000;  // 10s
const unsigned long SENSOR_READ_INTERVAL_MS = 2000; // 2s
const unsigned long BOOT_STABILIZATION_MS = 10000;  // 10s de estabilização

// ========================================
// OBJETOS GLOBAIS
// ========================================
FirebaseData fbdo;
FirebaseData streamCmd;
FirebaseData streamCfg;
FirebaseAuth auth;
FirebaseConfig config;

// ========================================
// VARIÁVEIS DE ESTADO
// ========================================
unsigned long bootAt = 0;
unsigned long lastHeartbeat = 0;
unsigned long lastTelemetry = 0;
unsigned long lastSensorRead = 0;
unsigned long lastIrrigationStart = 0;
unsigned long irrigationStartTime = 0;

bool pumpState = false;
float currentMoisture = 40.0;
bool firebaseInitialized = false;

// Estados da máquina de controle
enum SystemState {
  STATE_IDLE,
  STATE_IRRIGATING,
  STATE_LOCKOUT
};
SystemState systemState = STATE_IDLE;

// ========================================
// CONFIGURAÇÃO ATUAL (sincronizada com Firebase)
// ========================================
struct Config {
  int moistureLowPct = 35;
  int moistureHighPct = 45;
  int tMaxIrrSec = 30;
  int tMinGapMin = 15;
  int rawDry = SENSOR_DRY;
  int rawWet = SENSOR_WET;
  String plantName = "";
  unsigned long updatedAt = 0;
} currentConfig;

// ========================================
// DECLARAÇÕES DE FUNÇÕES
// ========================================
void publishSnapshot();
void publishTelemetry();
void publishHeartbeat();

// ========================================
// FUNÇÕES AUXILIARES
// ========================================

bool isFirebaseReady() {
  return Firebase.ready();
}

// ✅ Controle do relé com TIP127 (PNP - lógica invertida)
void setPump(bool on) {
  pumpState = on;
  digitalWrite(PIN_PUMP_RELAY, on ? LOW : HIGH);  // ✅ Invertido para PNP!
  Serial.printf("[PUMP] %s\n", on ? "🟢 LIGADA" : "⚪ DESLIGADA");
}

// Leitura do sensor de umidade REAL
float readSoilMoisture() {
  static int readings[10] = {0};
  static int readIndex = 0;
  static bool initialized = false;

  int raw = analogRead(PIN_SOIL_SENSOR);

  if (!initialized) {
    for (int i = 0; i < 10; i++) readings[i] = raw;
    initialized = true;
  } else {
    readings[readIndex] = raw;
    readIndex = (readIndex + 1) % 10;
  }

  int sum = 0;
  for (int i = 0; i < 10; i++) sum += readings[i];
  int rawValue = sum / 10;

  float moisture = map(rawValue, currentConfig.rawDry, currentConfig.rawWet, 0, 100);

  if (moisture < 0) moisture = 0;
  if (moisture > 100) moisture = 100;

  static unsigned long lastDebug = 0;
  if (millis() - lastDebug > 5000) {
    Serial.printf("[SENSOR] 📊 Raw: %d | Umidade: %.1f%% | Calibração: [%d seco ↔ %d molhado]\n", 
                  rawValue, moisture, currentConfig.rawDry, currentConfig.rawWet);
    lastDebug = millis();
  }

  return moisture;
}

const char* stateToString(SystemState state) {
  switch (state) {
    case STATE_IDLE: return "IDLE";
    case STATE_IRRIGATING: return "IRRIGATING";
    case STATE_LOCKOUT: return "LOCKOUT";
    default: return "UNKNOWN";
  }
}

// ========================================
// SETUP WIFI
// ========================================
void setupWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.printf("\n[WiFi] Conectando a \"%s\"", WIFI_SSID);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 60) {
    delay(500);
    Serial.print(".");
    retries++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("[WiFi] ✅ Conectado\n");
    Serial.printf("       IP: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("       RSSI: %d dBm\n", WiFi.RSSI());
  } else {
    Serial.println("[WiFi] ⚠️ Não conectou (continuará tentando em background)");
  }
}

// ========================================
// CALLBACKS DE STREAM
// ========================================

void streamCallbackCmd(FirebaseStream data) {
  Serial.printf("[CMD] 📥 Recebido | path=%s | type=%s\n",
                data.dataPath().c_str(),
                data.dataType().c_str());

  if (data.dataType() == "boolean") {
    bool v = data.boolData();
    Serial.printf("[CMD] irrigateNow = %s\n", v ? "true" : "false");

    if (v) {
      Serial.println("[CMD] 💧 Executando irrigação manual...");
      
      setPump(true);
      delay(3000);
      setPump(false);

      String cmdPath = "/devices/" + String(DEVICE_ID) + "/commands/irrigateNow";
      if (Firebase.RTDB.setBool(&fbdo, cmdPath.c_str(), false)) {
        Serial.println("[CMD] ✅ ACK enviado (irrigateNow=false)");
      } else {
        Serial.printf("[CMD] ❌ Falha ACK: %s\n", fbdo.errorReason().c_str());
      }

      String evtPath = "/devices/" + String(DEVICE_ID) + "/events";
      FirebaseJson evt;
      evt.set("tsMs", (int)millis());
      evt.set("type", "manual_irrigate");
      evt.set("duration", 3);
      Firebase.RTDB.pushJSON(&fbdo, evtPath.c_str(), &evt);
      
      systemState = STATE_LOCKOUT;
      lastIrrigationStart = millis();
      
      publishSnapshot();
    }
  }
}

void streamTimeoutCmd(bool timeout) {
  if (timeout) {
    Serial.println("[CMD] ⚠️ Stream timeout, reconectando...");
  }
}

void streamCallbackConfig(FirebaseStream data) {
  Serial.printf("[CONFIG] 📥 Recebido | path=%s | type=%s\n",
                data.dataPath().c_str(),
                data.dataType().c_str());

  if (data.dataType() == "json") {
    FirebaseJson json = data.jsonObject();
    FirebaseJsonData result;

    if (json.get(result, "moistureLowPct")) currentConfig.moistureLowPct = result.intValue;
    if (json.get(result, "moistureHighPct")) currentConfig.moistureHighPct = result.intValue;
    if (json.get(result, "tMaxIrrSec")) currentConfig.tMaxIrrSec = result.intValue;
    if (json.get(result, "tMinGapMin")) currentConfig.tMinGapMin = result.intValue;
    if (json.get(result, "rawDry")) currentConfig.rawDry = result.intValue;
    if (json.get(result, "rawWet")) currentConfig.rawWet = result.intValue;
    if (json.get(result, "plantName")) currentConfig.plantName = result.stringValue;
    if (json.get(result, "updatedAt")) currentConfig.updatedAt = result.intValue;

    Serial.println("[CONFIG] ✅ Configuração atualizada:");
    Serial.printf("         🌱 Planta: %s\n", 
                  currentConfig.plantName.length() > 0 ? currentConfig.plantName.c_str() : "Manual");
    Serial.printf("         💧 Faixa umidade: %d%% - %d%%\n", 
                  currentConfig.moistureLowPct, currentConfig.moistureHighPct);
    Serial.printf("         ⏱️  Tempo máx irrigação: %ds\n", currentConfig.tMaxIrrSec);
    Serial.printf("         ⏳ Intervalo mínimo: %dmin\n", currentConfig.tMinGapMin);
    Serial.printf("         📊 Calibração: dry=%d | wet=%d\n", 
                  currentConfig.rawDry, currentConfig.rawWet);
  }
}

void streamTimeoutConfig(bool timeout) {
  if (timeout) {
    Serial.println("[CONFIG] ⚠️ Stream timeout, reconectando...");
  }
}

// ========================================
// SETUP FIREBASE
// ========================================
void setupFirebase() {
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;

  Serial.println("[FB] 🔐 Autenticando anonimamente...");
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("[FB] ✅ Autenticação iniciada");
  } else {
    Serial.printf("[FB] ❌ Erro de autenticação: %s\n", 
                  config.signer.signupError.message.c_str());
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  Serial.println("[FB] ⏳ Firebase inicializado (token será obtido em background)");
  firebaseInitialized = true;
}

void initializeStreams() {
  static bool streamsInitialized = false;
  
  if (streamsInitialized || !isFirebaseReady()) {
    return;
  }
  
  Serial.println("[FB] ✅ Token pronto! Inicializando streams...");
  
  String bootPath = "/devices/" + String(DEVICE_ID) + "/status/bootTsMs";
  if (Firebase.RTDB.setInt(&fbdo, bootPath.c_str(), (int)millis())) {
    Serial.println("[FB] ✅ bootTsMs publicado");
  }

  String cmdPath = "/devices/" + String(DEVICE_ID) + "/commands/irrigateNow";
  if (Firebase.RTDB.beginStream(&streamCmd, cmdPath.c_str())) {
    Firebase.RTDB.setStreamCallback(&streamCmd, streamCallbackCmd, streamTimeoutCmd);
    Serial.printf("[FB] ✅ Stream commands ativo\n");
  } else {
    Serial.printf("[FB] ❌ Erro stream commands: %s\n", streamCmd.errorReason().c_str());
  }

  String cfgPath = "/devices/" + String(DEVICE_ID) + "/config";
  if (Firebase.RTDB.beginStream(&streamCfg, cfgPath.c_str())) {
    Firebase.RTDB.setStreamCallback(&streamCfg, streamCallbackConfig, streamTimeoutConfig);
    Serial.printf("[FB] ✅ Stream config ativo\n");
  } else {
    Serial.printf("[FB] ❌ Erro stream config: %s\n", streamCfg.errorReason().c_str());
  }
  
  publishSnapshot();
  
  streamsInitialized = true;
  Serial.println("[FB] ✅ Streams inicializados com sucesso!\n");
}

// ========================================
// PUBLICAÇÃO DE DADOS
// ========================================

void publishHeartbeat() {
  if (!isFirebaseReady()) {
    return;
  }
  
  String hbPath = "/devices/" + String(DEVICE_ID) + "/status/heartbeatTsMs";
  if (Firebase.RTDB.setInt(&fbdo, hbPath.c_str(), (int)millis())) {
    Serial.printf("[HB] ✅ Heartbeat enviado (%lu ms)\n", millis());
  } else {
    Serial.printf("[HB] ❌ Falha: %s\n", fbdo.errorReason().c_str());
  }
}

void publishSnapshot() {
  if (!isFirebaseReady()) {
    return;
  }
  
  String snapPath = "/devices/" + String(DEVICE_ID) + "/snapshot";
  FirebaseJson snap;
  snap.set("soilMoisture", currentMoisture);
  snap.set("pumpState", pumpState);
  snap.set("state", stateToString(systemState));
  snap.set("tsMs", (int)millis());
  
  if (Firebase.RTDB.setJSON(&fbdo, snapPath.c_str(), &snap)) {
    Serial.printf("[SNAP] ✅ Atualizado | Umidade: %.1f%% | Estado: %s | Bomba: %s\n",
                  currentMoisture, stateToString(systemState), pumpState ? "ON" : "OFF");
  } else {
    Serial.printf("[SNAP] ❌ Falha: %s\n", fbdo.errorReason().c_str());
  }
}

void publishTelemetry() {
  if (!isFirebaseReady()) {
    return;
  }
  
  String telePath = "/devices/" + String(DEVICE_ID) + "/telemetry";
  FirebaseJson tele;
  tele.set("tsMs", (int)millis());
  tele.set("soilMoisture", currentMoisture);
  tele.set("pumpState", pumpState);
  tele.set("state", stateToString(systemState));
  
  if (Firebase.RTDB.pushJSON(&fbdo, telePath.c_str(), &tele)) {
    Serial.printf("[TEL] ✅ Publicada | Umidade: %.1f%%\n", currentMoisture);
  } else {
    Serial.printf("[TEL] ❌ Falha: %s\n", fbdo.errorReason().c_str());
  }
}

// ========================================
// LÓGICA DE CONTROLE AUTOMÁTICO
// ========================================
void controlLoop() {
  if (millis() - bootAt < BOOT_STABILIZATION_MS) {
    return;
  }

  unsigned long now = millis();
  unsigned long timeSinceLastIrrigation = now - lastIrrigationStart;
  unsigned long irrigationDuration = now - irrigationStartTime;
  
  switch (systemState) {
    
    case STATE_IDLE:
      if (currentMoisture < currentConfig.moistureLowPct) {
        Serial.printf("[CTRL] 💧 Umidade baixa (%.1f%% < %d%%), iniciando irrigação\n",
                      currentMoisture, currentConfig.moistureLowPct);
        systemState = STATE_IRRIGATING;
        irrigationStartTime = now;
        setPump(true);
        
        if (isFirebaseReady()) {
          String evtPath = "/devices/" + String(DEVICE_ID) + "/events";
          FirebaseJson evt;
          evt.set("tsMs", (int)millis());
          evt.set("type", "auto_irrigate_start");
          evt.set("moisture", currentMoisture);
          Firebase.RTDB.pushJSON(&fbdo, evtPath.c_str(), &evt);
        }
      }
      break;
      
    case STATE_IRRIGATING:
      if (currentMoisture >= currentConfig.moistureHighPct) {
        Serial.printf("[CTRL] ✅ Umidade atingida (%.1f%% >= %d%%), parando irrigação\n",
                      currentMoisture, currentConfig.moistureHighPct);
        setPump(false);
        systemState = STATE_LOCKOUT;
        lastIrrigationStart = now;
        
        if (isFirebaseReady()) {
          String evtPath = "/devices/" + String(DEVICE_ID) + "/events";
          FirebaseJson evt;
          evt.set("tsMs", (int)millis());
          evt.set("type", "auto_irrigate_end");
          evt.set("reason", "target_reached");
          evt.set("duration", (int)(irrigationDuration / 1000));
          Firebase.RTDB.pushJSON(&fbdo, evtPath.c_str(), &evt);
        }
      } 
      else if (irrigationDuration >= (currentConfig.tMaxIrrSec * 1000)) {
        Serial.printf("[CTRL] ⏱️ Tempo máximo atingido (%ds), parando irrigação\n",
                      currentConfig.tMaxIrrSec);
        setPump(false);
        systemState = STATE_LOCKOUT;
        lastIrrigationStart = now;
        
        if (isFirebaseReady()) {
          String evtPath = "/devices/" + String(DEVICE_ID) + "/events";
          FirebaseJson evt;
          evt.set("tsMs", (int)millis());
          evt.set("type", "auto_irrigate_end");
          evt.set("reason", "timeout");
          evt.set("duration", currentConfig.tMaxIrrSec);
          Firebase.RTDB.pushJSON(&fbdo, evtPath.c_str(), &evt);
        }
      }
      break;
      
    case STATE_LOCKOUT:
      if (timeSinceLastIrrigation >= (currentConfig.tMinGapMin * 60 * 1000)) {
        Serial.printf("[CTRL] ⏳ Intervalo mínimo cumprido (%dmin), voltando para IDLE\n",
                      currentConfig.tMinGapMin);
        systemState = STATE_IDLE;
      }
      break;
  }
}

// ========================================
// SETUP
// ========================================
void setup() {
  Serial.begin(115200);
  delay(300);

  bootAt = millis();

  pinMode(PIN_PUMP_RELAY, OUTPUT);
  pinMode(PIN_SOIL_SENSOR, INPUT);
  
  // ✅ Garante bomba desligada (HIGH com TIP127 PNP)
  digitalWrite(PIN_PUMP_RELAY, HIGH);
  pumpState = false;

  Serial.println("\n╔════════════════════════════════════════════╗");
  Serial.println("║   ESP32 - Sistema de Irrigação IoT        ║");
  Serial.println("║   Device ID: esp32-vaso-01                 ║");
  Serial.println("║   Versão: 2.0 - TIP127 (PNP)               ║");
  Serial.println("║   🌱 Sensor: GPIO34 (ADC1_CH6)             ║");
  Serial.println("║   💧 Relé: GPIO4 + TIP127                  ║");
  Serial.println("╚════════════════════════════════════════════╝\n");
  
  // ✅ TESTE INICIAL DO RELÉ
  Serial.println("[TEST] 🔧 Testando relé com TIP127...");
  digitalWrite(PIN_PUMP_RELAY, LOW);  // Liga
  delay(1000);
  digitalWrite(PIN_PUMP_RELAY, HIGH); // Desliga
  Serial.println("[TEST] ✅ Relé OK!\n");
  
  Serial.printf("[BOOT] ⏳ Período de estabilização: %d segundos\n", BOOT_STABILIZATION_MS/1000);
  Serial.println("[BOOT] 🚫 Controle automático bloqueado durante boot\n");

  setupWifi();
  setupFirebase();

  Serial.println("\n✅ Setup concluído! Sistema operacional.\n");
  Serial.println("═══════════════════════════════════════════════\n");
}

// ========================================
// LOOP PRINCIPAL
// ========================================
void loop() {
  unsigned long now = millis();
  static SystemState lastPublishedState = STATE_IDLE;

  if (firebaseInitialized && isFirebaseReady()) {
    initializeStreams();
  }

  if (now - lastSensorRead >= SENSOR_READ_INTERVAL_MS) {
    lastSensorRead = now;
    currentMoisture = readSoilMoisture();
  }

  controlLoop();

  if (systemState != lastPublishedState || pumpState) {
    publishSnapshot();
    lastPublishedState = systemState;
  }

  if (now - lastTelemetry >= TELEMETRY_INTERVAL_MS) {
    lastTelemetry = now;
    publishTelemetry();
  }

  if (now - lastHeartbeat >= HEARTBEAT_INTERVAL_MS) {
    lastHeartbeat = now;
    publishHeartbeat();
  }

  delay(10);
}