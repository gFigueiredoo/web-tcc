# Fluxograma do Protocolo de Calibração Experimental - Sistema IoT

## Código Mermaid

```mermaid
flowchart TD
    %% Estilos simplificados
    classDef etapa fill:#e8f5e8,stroke:#388e3c,stroke-width:2px
    classDef decisao fill:#fff3e0,stroke:#f57c00,stroke-width:2px
    
    %% As 4 etapas principais
    E1["🌱 ETAPA 1<br/>PREPARAÇÃO SUBSTRATO<br/><br/>Solo universal + homogeneização"]:::etapa
    
    E2["🔥 ETAPA 2<br/>PONTO SECO<br/><br/>Secagem 48h → rawDry"]:::etapa
    
    ESTAB{"Estabilização<br/>< 0,1%?"}:::decisao
    
    E3["💧 ETAPA 3<br/>PONTO SATURADO<br/><br/>Irrigação gradual → rawWet"]:::etapa
    
    E4["📈 ETAPA 4<br/>VALIDAÇÃO<br/><br/>Pontos 25%, 50%, 75%"]:::etapa
    
    VALID{"R² ≥ 0,85?"}:::decisao
    
    FINAL["✅ CALIBRAÇÃO<br/>CONCLUÍDA"]:::etapa
    
    %% Fluxo simplificado
    E1 --> E2
    E2 --> ESTAB
    ESTAB -->|NÃO| E2
    ESTAB -->|SIM| E3
    E3 --> E4
    E4 --> VALID
    VALID -->|SIM| FINAL
    VALID -->|NÃO| E2
```

## Tempos Específicos do Protocolo

| **Etapa** | **Duração** | **Frequência** | **Critério** |
|-----------|-------------|----------------|--------------|
| Preparação | 30 min | Uma vez | Visual (homogeneidade) |
| Ponto Seco | 48h | Medições cada 4h | Variação < 0,1% |
| Ponto Saturado | 2-4h | Irrigação 50ml/5min | Sem drenagem 30min |
| Equilíbrio | 2h | Uma vez | Estabilização leituras |
| Validação | 4-6h | Pontos 25%, 50%, 75% | R² ≥ 0,85 |

## Como Usar Este Diagrama

### 1. Renderização Online
- Acesse: https://mermaid.live/
- Cole o código acima
- Baixe como PNG, SVG ou PDF

### 2. Para seu TCC LaTeX
```latex
\begin{figure}[htbp]
\centering
\includegraphics[width=0.85\textwidth]{figuras/protocolo_calibracao.png}
\caption{Fluxograma do protocolo de calibração experimental para diferentes volumes de vaso}
\label{fig:protocolo_calibracao}
\end{figure}
```

## Características do Fluxograma

- **4 etapas principais** do protocolo de calibração
- **Tempos específicos** para cada fase
- **Critérios de qualidade** claramente definidos
- **Pontos críticos** destacados com avisos
- **Fluxo de decisão** com loops de repetição
- **Validação estatística** com critério R² ≥ 0,85
- **Layout profissional** adequado para TCC acadêmico

Este diagrama pode ser usado diretamente na Seção 3 (Materiais e Métodos) do seu TCC como a "IMAGEM 3" do protocolo de calibração experimental.
