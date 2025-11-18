# Como Executar o Código Python do Gráfico - Guia Completo

## 🚀 **Passo a Passo Simples**

### **Opção 1: Terminal/CMD (Recomendado)**

#### 1. **Abrir Terminal**
- **Windows**: Pressione `Win + R`, digite `cmd`, pressione Enter
- **Mac**: Pressione `Cmd + Espaço`, digite "Terminal", pressione Enter  
- **Linux**: Pressione `Ctrl + Alt + T`

#### 2. **Navegar até a pasta do projeto**
```bash
cd /Users/I574585/Desktop/tcc/web-tcc
```

#### 3. **Verificar se Python está instalado**
```bash
python --version
```
ou
```bash
python3 --version
```

Se não aparecer a versão, [instale Python aqui](https://python.org/downloads/)

#### 4. **Instalar bibliotecas necessárias**
```bash
pip install matplotlib numpy
```
ou se der erro:
```bash
pip3 install matplotlib numpy
```

#### 5. **Executar o código**
```bash
python grafico_irrigacoes.py
```
ou:
```bash
python3 grafico_irrigacoes.py
```

### **Opção 2: VS Code (Mais Fácil)**

#### 1. **Abrir VS Code**
- Já está aberto no seu projeto!

#### 2. **Abrir o arquivo Python**
- Clique em `grafico_irrigacoes.py` na barra lateral

#### 3. **Instalar extensão Python** (se não tiver)
- Vá em Extensions (Ctrl+Shift+X)
- Procure por "Python" 
- Instale a extensão da Microsoft

#### 4. **Executar o código**
- **Método 1**: Pressione `F5`
- **Método 2**: Clique no botão "Play" ▶️ no canto superior direito
- **Método 3**: Clique com botão direito → "Run Python File in Terminal"

### **Opção 3: Google Colab (Online - Sem Instalação)**

#### 1. **Acesse**: https://colab.research.google.com/

#### 2. **Criar novo notebook**
- Clique em "New notebook" ou "Novo notebook"

#### 3. **Copiar e colar o código**
- Copie todo o conteúdo do arquivo `grafico_irrigacoes.py`
- Cole na célula do Colab

#### 4. **Executar**
- Pressione `Shift + Enter` ou clique no botão Play ▶️

### **Opção 4: Jupyter Notebook (Local)**

#### 1. **Instalar Jupyter**
```bash
pip install jupyter matplotlib numpy
```

#### 2. **Iniciar Jupyter**
```bash
jupyter notebook
```

#### 3. **Criar novo notebook**
- Clique em "New" → "Python 3"

#### 4. **Copiar código e executar**
- Cole o código e pressione `Shift + Enter`

## 🔧 **Solução de Problemas Comuns**

### **Erro: "python not found"**
**Solução**: Instalar Python
- Baixe em: https://python.org/downloads/
- **Importante**: Marque "Add Python to PATH" durante instalação

### **Erro: "pip not found"**
**Solução**: 
```bash
python -m ensurepip --upgrade
```

### **Erro: "No module named matplotlib"**
**Solução**:
```bash
pip install matplotlib numpy
```

### **Erro de permissão no Mac/Linux**
**Solução**: Use `sudo`
```bash
sudo pip3 install matplotlib numpy
```

### **Bibliotecas não instalam**
**Solução**: Use conda
```bash
conda install matplotlib numpy
```

## 📱 **Método Ultra-Simples (Celular/Tablet)**

### **Pydroid 3 (Android)**
1. Baixe "Pydroid 3" na Play Store
2. Abra o app
3. Copie e cole o código
4. Toque em "Run"

### **Pythonista (iOS)**
1. Baixe "Pythonista 3" na App Store  
2. Crie novo script
3. Cole o código
4. Toque no botão play

## ✅ **O Que Deve Acontecer**

Quando executar corretamente, você verá:

```
Gerando Gráfico Temporal de Irrigações - TCC Sistema IoT
=======================================================
Médias calculadas:
Vaso Pequeno: 2.1 ciclos/dia
Vaso Médio: 1.4 ciclos/dia
Vaso Grande: 0.9 ciclos/dia

Gráficos salvos:
PNG: irrigacoes_temporal_20251117_194500.png
PDF: irrigacoes_temporal_20251117_194500.pdf

Estatísticas dos dados:
Vaso Pequeno - Desvio padrão: 0.74
Vaso Médio - Desvio padrão: 0.51
Vaso Grande - Desvio padrão: 0.52

Gráfico gerado com sucesso!
```

E uma janela com o gráfico aparecerá na tela!

## 📁 **Arquivos Gerados**

Após executar, você terá:
- `irrigacoes_temporal_YYYYMMDD_HHMMSS.png` - Para usar no LaTeX
- `irrigacoes_temporal_YYYYMMDD_HHMMSS.pdf` - Para impressão

## 🎯 **Recomendação**

**Para você**: Use o **VS Code** (Opção 2) - é o mais simples já que você já tem o VS Code aberto!

1. Clique no arquivo `grafico_irrigacoes.py`
2. Pressione `F5`
3. Pronto! 🎉

## 🆘 **Precisa de Ajuda?**

Se der algum erro, copie a mensagem de erro completa e me envie que eu ajudo a resolver!
