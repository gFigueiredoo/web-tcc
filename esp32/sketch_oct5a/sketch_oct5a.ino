#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// ========= CONFIG DO USUÁRIO =========
#define WIFI_SSID      "GABRIEL OI"
#define WIFI_PASSWORD  "85139584Gg#"

#define API_KEY        "AIzaSyCkVxnJ7TA3MgXOsDvDpxj8HGg-SY7NZzA"
#define DATABASE_URL   "https://tcc-gabriel-9a0f9-default-rtdb.firebaseio.com/"  // termine com '/'

#define DEVICE_ID      "esp32-vaso-01"   // use o mesmo que está no seu RTDB
// ========= FIM CONFIG =========

// Objetos Firebase
FirebaseData fbdo;
FirebaseData streamCmd;
FirebaseAuth auth;
FirebaseConfig config;

// Controle de publicação
unsigned long lastHeartbeat = 0;
const unsigned long HEARTBEAT_MS = 10000; // 10s

void setupWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.printf("Conectando ao WiFi: %s\n", WIFI_SSID);
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 60) {
    delay(500);
    Serial.print(".");
    retries++;
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("WiFi OK. IP: %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("WiFi ainda não conectou; seguirá tentando em background.");
  }
}

void streamCallbackCmd(FirebaseStream data) {
  // Escuta /devices/{id}/commands/irrigateNow
  Serial.printf("[Stream] path: %s, type: %s, event: %s\n",
                data.dataPath().c_str(),
                data.dataType().c_str(),
                data.eventType().c_str());

  if (data.dataType() == "boolean") {
    bool v = data.boolData();
    Serial.printf("Comando irrigateNow = %s\n", v ? "true" : "false");
    if (v) {
      // Aqui apenas "ack" do comando e imprime no Serial
      String cmdPath = "/devices/" + String(DEVICE_ID) + "/commands/irrigateNow";
      Firebase.RTDB.setBool(&fbdo, cmdPath.c_str(), false);
      // Também registra um evento simples
      String evtPath = "/devices/" + String(DEVICE_ID) + "/events";
      FirebaseJson evt;
      evt.set("tsMs", (int)millis());
      evt.set("type", "manual_irrigate_command_received");
      Firebase.RTDB.pushJSON(&fbdo, evtPath.c_str(), &evt);
    }
  }
}

void streamTimeoutCmd(bool timeout) {
  if (timeout) Serial.println("[Stream] commands timeout; reconectando...");
}

void setupFirebase() {
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Auth anônima
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Auth anon OK");
  } else {
    Serial.printf("Auth anon ERRO: %s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Publica um "boot"
  String bootPath = "/devices/" + String(DEVICE_ID) + "/status/bootTsMs";
  Firebase.RTDB.setInt(&fbdo, bootPath.c_str(), (int)millis());

  // Inicia stream do caminho de comandos
  String cmdPath = "/devices/" + String(DEVICE_ID) + "/commands/irrigateNow";
  if (!Firebase.RTDB.beginStream(&streamCmd, cmdPath.c_str())) {
    Serial.printf("beginStream commands erro: %s\n", streamCmd.errorReason().c_str());
  } else {
    Firebase.RTDB.setStreamCallback(&streamCmd, streamCallbackCmd, streamTimeoutCmd);
  }
}

void setup() {
  Serial.begin(115200);
  delay(400);
  setupWifi();
  setupFirebase();
}

void loop() {
  unsigned long now = millis();

  // A cada 10s publica um "heartbeat" e um snapshot simples
  if (now - lastHeartbeat >= HEARTBEAT_MS) {
    lastHeartbeat = now;

    // Heartbeat
    String hbPath = "/devices/" + String(DEVICE_ID) + "/status/heartbeatTsMs";
    if (!Firebase.RTDB.setInt(&fbdo, hbPath.c_str(), (int)now)) {
      Serial.printf("Falha heartbeat: %s\n", fbdo.errorReason().c_str());
    } else {
      Serial.println("Heartbeat enviado.");
    }

    // Snapshot (simulado)
    // Obs.: sem sensor ligado; só envia valores fixos para você ver no Console.
    String snapPath = "/devices/" + String(DEVICE_ID) + "/snapshot";
    FirebaseJson snap;
    snap.set("soilMoisture", 42.0);     // valor fixo só para teste
    snap.set("pumpState", false);
    snap.set("state", "IDLE");
    if (!Firebase.RTDB.setJSON(&fbdo, snapPath.c_str(), &snap)) {
      Serial.printf("Falha snapshot: %s\n", fbdo.errorReason().c_str());
    } else {
      Serial.println("Snapshot atualizado.");
    }

    // Telemetria (push) — simulado
    String telePath = "/devices/" + String(DEVICE_ID) + "/telemetry";
    FirebaseJson tele;
    tele.set("tsMs", (int)now);
    tele.set("soilMoisture", 42.0);
    tele.set("pumpState", false);
    if (!Firebase.RTDB.pushJSON(&fbdo, telePath.c_str(), &tele)) {
      Serial.printf("Falha telemetria: %s\n", fbdo.errorReason().c_str());
    } else {
      Serial.println("Telemetria publicada.");
    }
  }

  delay(10);
}