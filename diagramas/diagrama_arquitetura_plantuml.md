# Diagrama da Arquitetura do Sistema em 3 Camadas - PlantUML

## Código PlantUML

```plantuml
@startuml Arquitetura_Sistema_IoT

!theme plain
skinparam backgroundColor white
skinparam shadowing false
skinparam defaultFontSize 11
skinparam defaultFontName Arial

' Definindo cores para cada camada
skinparam package {
    BackgroundColor<<interface>> #e8f5e8
    BorderColor<<interface>> #388e3c
    BackgroundColor<<comunicacao>> #e3f2fd
    BorderColor<<comunicacao>> #1976d2
    BackgroundColor<<fisica>> #ffebee
    BorderColor<<fisica>> #d32f2f
}

skinparam rectangle {
    BackgroundColor<<web>> #f1f8e9
    BorderColor<<web>> #689f38
    BackgroundColor<<wifi>> #e8eaf6
    BorderColor<<wifi>> #3f51b5
    BackgroundColor<<firebase>> #e8eaf6
    BorderColor<<firebase>> #3f51b5
    BackgroundColor<<buffer>> #fce4ec
    BorderColor<<buffer>> #e91e63
    BackgroundColor<<esp32>> #ffebee
    BorderColor<<esp32>> #f44336
    BackgroundColor<<sensores>> #ffebee
    BorderColor<<sensores>> #f44336
    BackgroundColor<<controle>> #ffebee
    BorderColor<<controle>> #f44336
}

' CAMADA INTERFACE (Primeiro - Topo)
package "🖥️ CAMADA INTERFACE" <<interface>> {
    rectangle "Interface Web Responsiva\n• HTML5 + CSS3 + JS ES6+\n• Modo Simples + Avançado\n• Chart.js v4.4.4" as WEB <<web>>
}

' CAMADA COMUNICAÇÃO (Meio)
package "📡 CAMADA COMUNICAÇÃO" <<comunicacao>> {
    rectangle "Wi-Fi 2.4GHz\n• Reconexão automática\n• Credenciais salvas\n• Timeout 30s" as WIFI <<wifi>>
    rectangle "Firebase Realtime DB\n• Sincronização tempo real\n• Push notifications\n• Latência <120ms" as FIREBASE <<firebase>>
    rectangle "Buffer Local\n• Últimas 100 medições\n• Operação offline\n• Sync automática" as BUFFER <<buffer>>
}

' CAMADA FÍSICA (Base)
package "⚙️ CAMADA FÍSICA" <<fisica>> {
    rectangle "ESP32 DevKit v1\n• Dual-core 240MHz\n• 520KB SRAM\n• 30 GPIO pins" as ESP32 <<esp32>>
    rectangle "SENSORES\n• Capacitivo (GPIO34)\n• DHT22 (Temp/Umidade)\n• LDR (Luminosidade)" as SENSORES <<sensores>>
    rectangle "SISTEMA CONTROLE\n• Transistor TIP127 (PNP)\n• Relé 5V/10A\n• Bomba 3-6V/300mA" as CONTROLE <<controle>>
}

' Fluxo descendente: Interface → Comunicação → Física
WEB --> FIREBASE : "Comandos remotos\nHTTP requests"
FIREBASE --> ESP32 : "Commands queue\nJSON commands"
ESP32 --> CONTROLE : "Acionamento\nGPIO control"

' Fluxo ascendente: Física → Comunicação → Interface
SENSORES --> ESP32 : "Leituras ADC\n12-bit resolution"
CONTROLE --> ESP32 : "Status bomba\nOn/Off events"
ESP32 --> FIREBASE : "Dados sensores\n30s intervals"
FIREBASE --> WEB : "Real-time sync\nWebSocket"

' Backup e redundância (setas tracejadas)
ESP32 ..> BUFFER : "Backup local\nSe Wi-Fi offline"
BUFFER ..> FIREBASE : "Sync quando\nconexão restaurada"

' Especificações técnicas posicionadas adequadamente

note right of ESP32
📋 **ESPECIFICAÇÕES TÉCNICAS**

• Sensor capacitivo: 12-bit ADC
• DHT22: ±0.5°C, ±2%RH  
• LDR: 10-1000 lux range
• Bomba: 300mA, 3-6V
• Relé: 5V coil, 10A contact
end note

@enduml
```

## Como Usar Este Diagrama PlantUML

### 1. Renderização Online
- **Acesse**: https://www.plantuml.com/plantuml/
- **Cole o código** acima na caixa de texto
- **Clique** em "Submit" para gerar
- **Baixe** como PNG, SVG ou PDF

### 2. Alternativa: PlantUML Editor Online
- **Acesse**: https://plantuml-editor.kkeisuke.com/
- **Interface mais amigável** com preview ao vivo
- **Cole o código** e veja o resultado instantaneamente

### 3. Para seu TCC LaTeX
```latex
\begin{figure}[htbp]
\centering
\includegraphics[width=0.9\textwidth]{figuras/arquitetura_sistema_plantuml.png}
\caption{Arquitetura do sistema em três camadas: física, comunicação e interface}
\label{fig:arquitetura_sistema}
\end{figure}
```

### 4. Usando VS Code (Opcional)
Se quiser editar localmente:
1. Instalar extensão "PlantUML"
2. Salvar arquivo como `.puml`
3. Ctrl+Shift+P → "PlantUML: Preview Current Diagram"

## Vantagens do PlantUML vs Mermaid

### ✅ **Mais Simples:**
- Sintaxe mais clara e legível
- Menos configuração necessária
- Melhor controle de layout

### ✅ **Melhor para Arquitetura:**
- Packages (camadas) bem definidos
- Setas personalizáveis (sólidas/tracejadas)
- Notes para especificações técnicas

### ✅ **Saída de Qualidade:**
- PNG/SVG em alta resolução
- Fontes e cores consistentes
- Layout automático otimizado

## Características do Diagrama PlantUML

- **3 camadas** claramente separadas em packages coloridos
- **Fluxo de dados** com setas direcionais e legendas
- **Redundâncias** destacadas com setas tracejadas
- **Especificações técnicas** em notes laterais
- **Cores diferenciadas** para cada tipo de componente
- **Layout profissional** adequado para TCC acadêmico

## Customizações Possíveis

Se quiser ajustar:
- **Cores**: Modificar valores nos `skinparam`
- **Layout**: Ajustar posicionamento dos elements
- **Texto**: Editar conteúdo dos retângulos
- **Setas**: Alterar estilos e direções

Este diagrama PlantUML oferece muito mais controle e clareza visual que o Mermaid equivalente!
