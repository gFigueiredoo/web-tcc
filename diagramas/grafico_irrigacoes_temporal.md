# Gráfico Temporal de Irrigações - 15 Dias de Teste

## Opções para Criar o Gráfico

### 1. **Excel/Google Sheets** (Mais Simples) ⭐⭐⭐⭐⭐
### 2. **Python + Matplotlib** (Mais Profissional) ⭐⭐⭐⭐
### 3. **Chart.js Online** (Web) ⭐⭐⭐
### 4. **Canva/Draw.io** (Visual) ⭐⭐⭐

## Dados Simulados Baseados no TCC

```csv
Dia,Vaso_Pequeno,Vaso_Medio,Vaso_Grande,Evento_Especial
1,2,1,1,
2,2,2,1,
3,3,1,1,Dia quente
4,2,1,0,
5,1,2,1,
6,2,1,1,
7,3,2,1,Mudança luminosidade
8,2,1,0,
9,2,1,1,
10,1,2,1,
11,3,1,1,Dia quente
12,2,2,0,
13,2,1,1,
14,1,1,1,
15,2,2,1,
```

## Código Python (Matplotlib) - Mais Profissional

```python
import matplotlib.pyplot as plt
import numpy as np

# Dados baseados no TCC
dias = list(range(1, 16))
vaso_pequeno = [2, 2, 3, 2, 1, 2, 3, 2, 2, 1, 3, 2, 2, 1, 2]
vaso_medio = [1, 2, 1, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 1, 2]
vaso_grande = [1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1]

# Eventos especiais
eventos = {3: 'Dia quente', 7: 'Mudança luminosidade', 11: 'Dia quente'}

# Configurar o gráfico
plt.figure(figsize=(12, 8))
plt.style.use('seaborn-v0_8')

# Plotar as linhas
plt.plot(dias, vaso_pequeno, 'o-', color='#1f77b4', linewidth=2.5, 
         markersize=6, label='Vaso Pequeno (1,2L) - Média: 2,1 ciclos/dia')
plt.plot(dias, vaso_medio, 's-', color='#2ca02c', linewidth=2.5, 
         markersize=6, label='Vaso Médio (3,5L) - Média: 1,4 ciclos/dia')
plt.plot(dias, vaso_grande, '^-', color='#d62728', linewidth=2.5, 
         markersize=6, label='Vaso Grande (7,8L) - Média: 0,9 ciclos/dia')

# Destacar eventos especiais
for dia, evento in eventos.items():
    plt.axvline(x=dia, color='orange', linestyle='--', alpha=0.7)
    plt.text(dia, max(max(vaso_pequeno), max(vaso_medio), max(vaso_grande)) + 0.2, 
             evento, rotation=90, ha='right', va='bottom', fontsize=9)

# Configurações do gráfico
plt.title('Padrões de Irrigação Automática durante 15 Dias de Teste\n' +
          'Validação Empírica dos Algoritmos Adaptativos por Volume de Vaso', 
          fontsize=14, fontweight='bold', pad=20)
plt.xlabel('Tempo (Dias)', fontsize=12)
plt.ylabel('Número de Irrigações por Dia', fontsize=12)
plt.grid(True, alpha=0.3)
plt.legend(loc='upper right', fontsize=10)

# Definir limites e ticks
plt.xlim(0.5, 15.5)
plt.ylim(-0.2, 3.5)
plt.xticks(range(1, 16))
plt.yticks(range(0, 4))

# Adicionar anotações explicativas
plt.text(12, 3.2, 'Maior variabilidade\n(vaso pequeno)', 
         ha='center', va='center', fontsize=9, 
         bbox=dict(boxstyle="round,pad=0.3", facecolor='lightblue', alpha=0.7))
plt.text(12, 0.3, 'Maior estabilidade\n(vaso grande)', 
         ha='center', va='center', fontsize=9,
         bbox=dict(boxstyle="round,pad=0.3", facecolor='lightcoral', alpha=0.7))

plt.tight_layout()
plt.savefig('irrigacoes_temporal.png', dpi=300, bbox_inches='tight')
plt.show()
```

## Código Excel/Google Sheets (Mais Simples)

### Passo 1: Criar planilha com dados
```
A1: Dia    B1: Pequeno    C1: Médio    D1: Grande
A2: 1      B2: 2          C2: 1        D2: 1
A3: 2      B3: 2          C3: 2        D3: 1
... (continuar com todos os dados)
```

### Passo 2: Criar gráfico
1. **Selecionar dados** (A1:D16)
2. **Inserir** → **Gráfico** → **Linha**
3. **Personalizar**:
   - Linha azul para Pequeno
   - Linha verde para Médio  
   - Linha vermelha para Grande
4. **Adicionar títulos** e legendas
5. **Marcar eventos especiais** com linhas verticais

## Código Chart.js (Online)

```javascript
const data = {
  labels: [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15],
  datasets: [{
    label: 'Vaso Pequeno (1,2L) - Média: 2,1 ciclos/dia',
    data: [2,2,3,2,1,2,3,2,2,1,3,2,2,1,2],
    borderColor: '#1f77b4',
    backgroundColor: 'rgba(31,119,180,0.1)',
    borderWidth: 3,
    pointRadius: 6
  }, {
    label: 'Vaso Médio (3,5L) - Média: 1,4 ciclos/dia',
    data: [1,2,1,1,2,1,2,1,1,2,1,2,1,1,2],
    borderColor: '#2ca02c',
    backgroundColor: 'rgba(44,160,44,0.1)',
    borderWidth: 3,
    pointRadius: 6
  }, {
    label: 'Vaso Grande (7,8L) - Média: 0,9 ciclos/dia',
    data: [1,1,1,0,1,1,1,0,1,1,1,0,1,1,1],
    borderColor: '#d62728',
    backgroundColor: 'rgba(214,39,40,0.1)',
    borderWidth: 3,
    pointRadius: 6
  }]
};

const config = {
  type: 'line',
  data: data,
  options: {
    responsive: true,
    plugins: {
      title: {
        display: true,
        text: 'Padrões de Irrigação Automática - 15 Dias de Teste'
      }
    },
    scales: {
      y: {
        beginAtZero: true,
        max: 4,
        title: {
          display: true,
          text: 'Número de Irrigações por Dia'
        }
      },
      x: {
        title: {
          display: true,
          text: 'Tempo (Dias)'
        }
      }
    }
  }
};
```

## Recomendação

**Para seu TCC, recomendo o Excel/Google Sheets** porque:
- ✅ Mais simples de usar
- ✅ Controle total sobre aparência
- ✅ Fácil de ajustar dados
- ✅ Exporta em alta qualidade
- ✅ Permite adicionar anotações facilmente

**Se quiser resultado mais profissional**, use Python com os dados que forneci.

## Características do Gráfico Final

- **3 linhas coloridas** (azul, verde, vermelho)
- **Eventos especiais** marcados com linhas verticais
- **Médias destacadas** nas legendas
- **Anotações** sobre variabilidade
- **Layout profissional** para TCC

Quer que eu ajude você a implementar em alguma dessas ferramentas específicas?
