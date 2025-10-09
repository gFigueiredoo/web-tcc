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
#define PIN_PUMP_LED     4      // GPIO 4 -> Resistor 220Ω -> LED -> GND
#define PIN_SOIL_SENSOR  34     // GPIO 34 (ADC1) para sensor capacitivo (opcional)

// Intervalos de tempo
const unsigned long HEARTBEAT_INTERVAL_MS = 10000;  // 10s
const unsigned long TELEMETRY_INTERVAL_MS = 10000;  // 10s
const unsigned long SENSOR_READ_INTERVAL_MS = 2000; // 2s

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
unsigned long lastHeartbeat = 0;
unsigned long lastTelemetry = 0;
unsigned long lastSensorRead = 0;
unsigned long lastIrrigationStart = 0;
unsigned long irrigationStartTime = 0;

bool pumpState = false;
float currentMoisture = 42.0;  // Valor inicial simulado

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
  int rawDry = 3100;
  int rawWet = 1400;
  String plantName = "";
  unsigned long updatedAt = 0;
} currentConfig;

// ========================================
// FUNÇÕES AUXILIARES
// ========================================

// Controle da bomba/LED
void setPump(bool on) {
  pumpState = on;
  digitalWrite(PIN_PUMP_LED, on ? HIGH : LOW);
  Serial.printf("[PUMP] %s\n", on ? "🟢 LIGADA" : "⚪ DESLIGADA");
}

// Leitura do sensor de umidade com calibração
float readSoilMoisture() {
  // Se você não tem sensor conectado ainda, retorna valor simulado
  #ifdef PIN_SOIL_SENSOR
    int rawValue = analogRead(PIN_SOIL_SENSOR);
    
    // Aplicar filtro de média móvel (opcional)
    static int readings[5] = {0};
    static int readIndex = 0;
    readings[readIndex] = rawValue;
    readIndex = (readIndex + 1) % 5;
    
    int sum = 0;
    for (int i = 0; i < 5; i++) {
      sum += readings[i];
    }
    rawValue = sum / 5;
    
    // Conversão para porcentagem com calibração
    float moisture = ((float)(currentConfig.rawWet - rawValue) / 
                     (float)(currentConfig.rawWet - currentConfig.rawDry)) * 100.0;
    
    // Clamp entre 0-100%
    if (moisture < 0) moisture = 0;
    if (moisture > 100) moisture = 100;
    
    return moisture;
  #else
    // Simulação: varia entre 38-46% para testar histerese
    static float simMoisture = 42.0;
    if (pumpState) {
      simMoisture += 0.5;  // Aumenta quando irriga
      if (simMoisture > 46) simMoisture = 46;
    } else {
      simMoisture -= 0.1;  // Diminui lentamente
      if (simMoisture < 38) simMoisture = 38;
    }
    return simMoisture;
  #endif
}

// Converte estado para string
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

// Stream: Comandos (irrigateNow)
void streamCallbackCmd(FirebaseStream data) {
  Serial.printf("[CMD] 📥 Recebido | path=%s | type=%s\n",
                data.dataPath().c_str(),
                data.dataType().c_str());

  if (data.dataType() == "boolean") {
    bool v = data.boolData();
    Serial.printf("[CMD] irrigateNow = %s\n", v ? "true" : "false");

    if (v) {
      Serial.println("[CMD] 💧 Executando irrigação manual...");
      
      // Irrigação manual forçada (ignora estado atual)
      setPump(true);
      delay(3000);  // 3 segundos fixo para comando manual
      setPump(false);

      // ACK do comando (zera para false)
      String cmdPath = "/devices/" + String(DEVICE_ID) + "/commands/irrigateNow";
      if (Firebase.RTDB.setBool(&fbdo, cmdPath.c_str(), false)) {
        Serial.println("[CMD] ✅ ACK enviado (irrigateNow=false)");
      } else {
        Serial.printf("[CMD] ❌ Falha ACK: %s\n", fbdo.errorReason().c_str());
      }

      // Registra evento
      String evtPath = "/devices/" + String(DEVICE_ID) + "/events";
      FirebaseJson evt;
      evt.set("tsMs", (int)millis());
      evt.set("type", "manual_irrigate");
      evt.set("duration", 3);
      if (Firebase.RTDB.pushJSON(&fbdo, evtPath.c_str(), &evt)) {
        Serial.println("[CMD] ✅ Evento registrado");
      }
      
      // Reseta estado para LOCKOUT após irrigação manual
      systemState = STATE_LOCKOUT;
      lastIrrigationStart = millis();
    }
  }
}

void streamTimeoutCmd(bool timeout) {
  if (timeout) {
    Serial.println("[CMD] ⚠️ Stream timeout, reconectando...");
  }
}

// Stream: Configuração
void streamCallbackConfig(FirebaseStream data) {
  Serial.printf("[CONFIG] 📥 Recebido | path=%s | type=%s\n",
                data.dataPath().c_str(),
                data.dataType().c_str());

  if (data.dataType() == "json") {
    FirebaseJson json = data.jsonObject();
    FirebaseJsonData result;

    // Atualiza configuração local
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
    Serial.println("[FB] ✅ Autenticação OK");
  } else {
    Serial.printf("[FB] ❌ Erro de autenticação: %s\n", 
                  config.signer.signupError.message.c_str());
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Publica timestamp de boot
  String bootPath = "/devices/" + String(DEVICE_ID) + "/status/bootTsMs";
  if (Firebase.RTDB.setInt(&fbdo, bootPath.c_str(), (int)millis())) {
    Serial.println("[FB] ✅ bootTsMs publicado");
  } else {
    Serial.printf("[FB] ⚠️ Falha bootTsMs: %s\n", fbdo.errorReason().c_str());
  }

  // Inicia stream de comandos
  String cmdPath = "/devices/" + String(DEVICE_ID) + "/commands/irrigateNow";
  if (Firebase.RTDB.beginStream(&streamCmd, cmdPath.c_str())) {
    Firebase.RTDB.setStreamCallback(&streamCmd, streamCallbackCmd, streamTimeoutCmd);
    Serial.printf("[FB] ✅ Stream commands ativo\n");
    Serial.printf("       Path: %s\n", cmdPath.c_str());
  } else {
    Serial.printf("[FB] ❌ Erro stream commands: %s\n", streamCmd.errorReason().c_str());
  }

  // Inicia stream de configuração
  String cfgPath = "/devices/" + String(DEVICE_ID) + "/config";
  if (Firebase.RTDB.beginStream(&streamCfg, cfgPath.c_str())) {
    Firebase.RTDB.setStreamCallback(&streamCfg, streamCallbackConfig, streamTimeoutConfig);
    Serial.printf("[FB] ✅ Stream config ativo\n");
    Serial.printf("       Path: %s\n", cfgPath.c_str());
  } else {
    Serial.printf("[FB] ❌ Erro stream config: %s\n", streamCfg.errorReason().c_str());
  }
}

// ========================================
// PUBLICAÇÃO DE DADOS
// ========================================

void publishHeartbeat() {
  String hbPath = "/devices/" + String(DEVICE_ID) + "/status/heartbeatTsMs";
  if (Firebase.RTDB.setInt(&fbdo, hbPath.c_str(), (int)millis())) {
    Serial.printf("[HB] ✅ Heartbeat enviado (%lu ms)\n", millis());
  } else {
    Serial.printf("[HB] ❌ Falha: %s\n", fbdo.errorReason().c_str());
  }
}

void publishSnapshot() {
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
  unsigned long now = millis();
  unsigned long timeSinceLastIrrigation = now - lastIrrigationStart;
  unsigned long irrigationDuration = now - irrigationStartTime;
  
  switch (systemState) {
    
    case STATE_IDLE:
      // Verifica se precisa irrigar
      if (currentMoisture < currentConfig.moistureLowPct) {
        Serial.printf("[CTRL] 💧 Umidade baixa (%.1f%% < %d%%), iniciando irrigação\n",
                      currentMoisture, currentConfig.moistureLowPct);
        systemState = STATE_IRRIGATING;
        irrigationStartTime = now;
        setPump(true);
      }
      break;
      
    case STATE_IRRIGATING:
      // Verifica condições de parada
      if (currentMoisture >= currentConfig.moistureHighPct) {
        Serial.printf("[CTRL] ✅ Umidade atingida (%.1f%% >= %d%%), parando irrigação\n",
                      currentMoisture, currentConfig.moistureHighPct);
        setPump(false);
        systemState = STATE_LOCKOUT;
        lastIrrigationStart = now;
      } 
      else if (irrigationDuration >= (currentConfig.tMaxIrrSec * 1000)) {
        Serial.printf("[CTRL] ⏱️ Tempo máximo atingido (%ds), parando irrigação\n",
                      currentConfig.tMaxIrrSec);
        setPump(false);
        systemState = STATE_LOCKOUT;
        lastIrrigationStart = now;
      }
      break;
      
    case STATE_LOCKOUT:
      // Aguarda intervalo mínimo antes de permitir nova irrigação
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

  // Configura pinos
  pinMode(PIN_PUMP_LED, OUTPUT);
  setPump(false);

  // Banner de inicialização
  Serial.println("\n╔════════════════════════════════════════════╗");
  Serial.println("║   ESP32 - Sistema de Irrigação IoT        ║");
  Serial.println("║   Device ID: esp32-vaso-01                 ║");
  Serial.println("║   Versão: 1.0 - TCC Gabriel                ║");
  Serial.println("╚════════════════════════════════════════════╝\n");

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

  // Leitura periódica do sensor
  if (now - lastSensorRead >= SENSOR_READ_INTERVAL_MS) {
    lastSensorRead = now;
    currentMoisture = readSoilMoisture();
  }

  // Controle automático de irrigação
  controlLoop();

  // Heartbeat periódico
  if (now - lastHeartbeat >= HEARTBEAT_INTERVAL_MS) {
    lastHeartbeat = now;
    publishHeartbeat();
    publishSnapshot();
  }

  // Telemetria periódica
  if (now - lastTelemetry >= TELEMETRY_INTERVAL_MS) {
    lastTelemetry = now;
    publishTelemetry();
  }

  delay(10);
}