/*
 * CALIBRAÇÃO DO SENSOR DE UMIDADE CAPACITIVO
 * TCC Gabriel - Sistema de Irrigação Automatizada
 * 
 * Este sketch serve apenas para calibrar o sensor.
 * Use o Serial Monitor para ver os valores em tempo real.
 */

#define PIN_SOIL_SENSOR  34  // GPIO34 (ADC1_CH6) → Sensor de Umidade

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("========================================");
  Serial.println("🌱 CALIBRAÇÃO DO SENSOR DE UMIDADE 🌱");
  Serial.println("========================================");
  Serial.println();
  Serial.println("Instruções:");
  Serial.println("1. Coloque o sensor em TERRA SECA");
  Serial.println("2. Anote o valor 'SECO'");
  Serial.println("3. Coloque o sensor em TERRA MOLHADA");  
  Serial.println("4. Anote o valor 'MOLHADO'");
  Serial.println("5. Use esses valores no código principal");
  Serial.println();
  Serial.println("Valores em tempo real:");
  Serial.println("====================");
}

void loop() {
  // Fazer múltiplas leituras para estabilizar
  int soma = 0;
  for(int i = 0; i < 10; i++) {
    soma += analogRead(PIN_SOIL_SENSOR);
    delay(50);
  }
  
  int valorMedio = soma / 10;
  
  // Mostrar valor no Serial Monitor
  Serial.printf("📊 Valor ADC: %4d", valorMedio);
  
  // Dar uma referência visual
  if(valorMedio > 3500) {
    Serial.println(" 🏜️  MUITO SECO (provavelmente no ar)");
  } else if(valorMedio > 2800) {
    Serial.println(" 🌵 SECO");
  } else if(valorMedio > 2000) {
    Serial.println(" 🌱 LEVEMENTE ÚMIDO");
  } else if(valorMedio > 1500) {
    Serial.println(" 💧 ÚMIDO");
  } else {
    Serial.println(" 🌊 MUITO ÚMIDO");
  }
  
  delay(1000); // Atualizar a cada segundo
}