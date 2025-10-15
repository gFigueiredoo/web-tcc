// ========== FUNÇÕES GLOBAIS PARA INTERFACE ==========
// Este arquivo não é um módulo ES6, então as funções ficam globais

// Função global para o slider
function updateSliderDisplay(value) {
  const moistureDisplay = document.getElementById('moistureDisplay');
  const lowInput = document.getElementById('low');
  const highInput = document.getElementById('high');
  
  if (moistureDisplay) {
    moistureDisplay.textContent = `${value}%`;
  }
  if (lowInput) lowInput.value = value;
  if (highInput) highInput.value = parseInt(value) + 10;
  
  console.log('Slider atualizado via função global:', value + '%');
}

// Função para botões de tempo
function selectTime(element, seconds) {
  // Remover active de todos
  document.querySelectorAll('.time-btn').forEach(btn => btn.classList.remove('active'));
  // Adicionar active no clicado
  element.classList.add('active');
  
  const tmaxInput = document.getElementById('tmax');
  if (tmaxInput) tmaxInput.value = seconds;
  
  console.log('Tempo selecionado via função global:', seconds + 's');
}

// Função para botões de frequência
function selectFreq(element, ms) {
  // Remover active de todos
  document.querySelectorAll('.freq-btn').forEach(btn => btn.classList.remove('active'));
  // Adicionar active no clicado
  element.classList.add('active');
  
  const sensorIntervalInput = document.getElementById('sensorInterval');
  if (sensorIntervalInput) sensorIntervalInput.value = ms;
  
  console.log('Frequência selecionada via função global:', ms + 'ms');
}

// Função para alternar modos
function switchConfigMode(element, mode) {
  // Atualizar botões
  document.querySelectorAll('.mode-btn').forEach(btn => btn.classList.remove('active'));
  element.classList.add('active');
  
  const simpleMode = document.getElementById('simpleMode');
  const advancedMode = document.getElementById('advancedMode');
  
  // Mostrar/esconder seções
  if (mode === 'simple') {
    if (simpleMode) simpleMode.style.display = 'block';
    if (advancedMode) advancedMode.style.display = 'none';
    console.log('Modo simples ativado via função global');
  } else {
    if (simpleMode) simpleMode.style.display = 'none';
    if (advancedMode) advancedMode.style.display = 'block';
    console.log('Modo avançado ativado via função global');
  }
}

console.log('Funções globais carregadas!');