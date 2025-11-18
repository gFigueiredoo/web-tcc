# Circuit Diagram Web Editor - Conexões Detalhadas
**Sistema IoT de Irrigação Automatizada - TCC Gabriel Figueiredo**

## Acesso e Configuração Inicial

1. **Acesse**: https://www.circuit-diagram.org/editor
2. **Novo projeto**: Clique em "New Circuit"
3. **Grade**: Ative a grade (Grid) para alinhamento preciso
4. **Zoom**: Ajuste para 100% para trabalhar confortavelmente

## Componentes Necessários

### Buscar na biblioteca lateral:
- **Microcontroller** (para ESP32)
- **Resistor** (3 unidades: 1kΩ, 10kΩ, 10kΩ)
- **Transistor PNP** (TIP127)  
- **Relay** (módulo relé)
- **DC Motor** (bomba)
- **Sensor** (genérico, para DHT22 e sensor capacitivo)
- **Photoresistor** (LDR)
- **Power Supply** (5V e 3.3V)
- **Ground** (símbolo GND)

## Layout e Posicionamento

### Coordenadas sugeridas (grade):
```
┌─ ESP32 (centro): posição (10, 10)
├─ DHT22: posição (5, 5)
├─ Sensor Capacitivo: posição (15, 5)  
├─ LDR + R10k: posição (20, 5)
├─ TIP127: posição (10, 15)
├─ Relé: posição (15, 15)
├─ Bomba: posição (20, 15)
├─ Fonte 5V: posição (2, 20)
└─ GND: posição (25, 20)
```

## Conexões Passo-a-Passo

### 1. ESP32 (Microcontroller)
**Configurar pinos do ESP32:**
- Adicione labels nos pinos: GPIO4, GPIO15, GPIO32, GPIO34, VIN, 3V3, GND

### 2. Sensores → ESP32

#### DHT22:
```
DHT22 pino 1 (VCC) ──→ ESP32 pino 3V3
DHT22 pino 2 (DATA) ──→ ESP32 pino GPIO15  
DHT22 pino 4 (GND) ──→ ESP32 pino GND
```

#### Sensor Capacitivo:
```
Sensor pino VCC ──→ ESP32 pino 3V3
Sensor pino OUT ──→ ESP32 pino GPIO34
Sensor pino GND ──→ ESP32 pino GND
```

#### LDR (Divisor de Tensão):
```
ESP32 pino 3V3 ──→ LDR pino 1
LDR pino 2 ──→ ESP32 pino GPIO32
LDR pino 2 ──→ Resistor 10kΩ pino 1
Resistor 10kΩ pino 2 ──→ ESP32 pino GND
```

### 3. Sistema de Acionamento

#### Resistor de Base:
```
ESP32 pino GPIO4 ──→ Resistor 1kΩ pino 1
Resistor 1kΩ pino 2 ──→ TIP127 pino Base
```

#### Transistor TIP127 (PNP):
```
Fonte 5V positivo ──→ Resistor 10kΩ pino 1 (pull-up)
Resistor 10kΩ pino 2 ──→ TIP127 pino Collector
TIP127 pino Collector ──→ Relé pino IN
TIP127 pino Emitter ──→ Relé pino VCC
```

#### Módulo Relé:
```
Relé pino GND ──→ GND comum
Relé pino COM ──→ Fonte 5V positivo
Relé pino NO ──→ Bomba pino positivo
```

#### Bomba:
```
Bomba pino negativo ──→ GND comum
```

### 4. Alimentação

#### Fonte 5V:
```
Fonte 5V positivo ──→ ESP32 pino VIN
Fonte 5V negativo ──→ GND comum
```

#### GND Comum:
Conecte todos os GNDs em um ponto comum:
- ESP32 GND
- DHT22 GND  
- Sensor Capacitivo GND
- LDR Resistor
- Relé GND
- Bomba negativo
- Fonte 5V negativo

## Etiquetas e Anotações

### Adicione labels nos fios:
- **Vermelho**: 5V, VCC
- **Azul**: 3.3V  
- **Preto**: GND
- **Verde**: Sinais digitais (GPIO4, GPIO15)
- **Laranja**: Sinais analógicos (GPIO32, GPIO34)

### Valores dos componentes:
- R1 (base): 1kΩ
- R2 (pull-up): 10kΩ  
- R3 (LDR): 10kΩ
- Transistor: TIP127 (PNP)
- Relé: 5V/10A
- Bomba: 3-6V DC

## Texto e Título

### Adicione caixa de texto no topo:
```
SISTEMA IoT DE IRRIGAÇÃO AUTOMATIZADA
Diagrama Esquemático do Circuito Eletrônico
TCC - Gabriel Figueiredo Carolino - UNISINOS 2025
```

### Legenda lateral:
```
ESPECIFICAÇÕES TÉCNICAS:
• ESP32 DevKit v1: 240MHz, Wi-Fi, 30 GPIO
• Sensor Capacitivo: v1.2, ADC 12-bit
• DHT22: ±0.5°C, ±2% RH  
• LDR GL5528: 10-1000 lux
• TIP127: PNP, 5A, lógica invertida
• Relé: 5V/10A, isolamento galvânico
• Bomba: 3-6V, 300mA submersível

CARACTERÍSTICAS DO SISTEMA:
• Precisão: < 8,5% erro
• Responsividade: 1,8±0,4s
• Confiabilidade: 99,4% uptime
• Calibração por volume de vaso
• Comunicação Firebase tempo real
```

## Verificação das Conexões

### ✅ Checklist Final:
- [ ] ESP32 tem todos os pinos rotulados
- [ ] DHT22 conectado ao GPIO15
- [ ] Sensor capacitivo no GPIO34 (ADC)
- [ ] LDR com divisor de tensão no GPIO32
- [ ] GPIO4 controla transistor via resistor 1kΩ
- [ ] TIP127 tem pull-up de 10kΩ
- [ ] Relé recebe sinal do collector do TIP127
- [ ] Bomba conectada via relé (COM-NO)
- [ ] Todos os GNDs interconectados
- [ ] Alimentação 5V para ESP32 e relé
- [ ] Alimentação 3.3V para sensores

## Exportação

### Para TCC:
1. **File** → **Export**
2. Escolha **PNG** ou **SVG** 
3. **Qualidade**: 300 DPI
4. **Tamanho**: A4 landscape
5. **Download** do arquivo

### Dicas de Formatação:
- Use linhas retas e ângulos de 90°
- Mantenha espaçamento uniforme
- Agrupe componentes relacionados
- Use cores consistentes para tipos de sinal
- Adicione setas nas conexões quando necessário

---

**Este diagrama representa fielmente o circuito descrito no seu TCC, com conexões tecnicamente corretas conforme o código do ESP32.**
