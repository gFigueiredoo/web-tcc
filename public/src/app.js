import { db, ref, onValue, set, push, remove } from './firebase.js';

const DEVICE_ID = 'esp32-vaso-01';



let currentEditPlantId = null;
let plantsData = {};
let configRef;
let chart;

// Referências DOM
let gaugeValue, gauge, rangeLabel, stateBadge, pumpBadge, lastUpdate, currentPlantLabel;
let lowInput, highInput, tmaxInput, tgapInput, rawDryInput, rawWetInput, sensorIntervalInput, cfgMsg;
let btnSalvar, btnIrrigar, plantSelect, plantsList, btnNovaPlanta;
let plantModal, closeModal, btnCancelar, btnSalvarPlanta, modalTitle, plantMsg;
let historyIntervalSelect;

// Novos elementos para interface amigável
let moistureRange, moistureDisplay, modeBtns, simpleMode, advancedMode;
let timeBtns, freqBtns;

// Inicializar quando o DOM estiver pronto
function init() {
  // Referências DOM
  gaugeValue = document.getElementById('gaugeValue');
  gauge = document.getElementById('gauge');
  rangeLabel = document.getElementById('rangeLabel');
  stateBadge = document.getElementById('stateBadge');
  pumpBadge = document.getElementById('pumpBadge');
  lastUpdate = document.getElementById('lastUpdate');
  currentPlantLabel = document.getElementById('currentPlantLabel');

  lowInput = document.getElementById('low');
  highInput = document.getElementById('high');
  tmaxInput = document.getElementById('tmax');
  tgapInput = document.getElementById('tgap');
  rawDryInput = document.getElementById('rawDry');
  rawWetInput = document.getElementById('rawWet');
  sensorIntervalInput = document.getElementById('sensorInterval');
  cfgMsg = document.getElementById('cfgMsg');

  btnSalvar = document.getElementById('btnSalvar');
  btnIrrigar = document.getElementById('btnIrrigar');
  plantSelect = document.getElementById('plantSelect');
  plantsList = document.getElementById('plantsList');
  btnNovaPlanta = document.getElementById('btnNovaPlanta');

  plantModal = document.getElementById('plantModal');
  closeModal = document.getElementById('closeModal');
  btnCancelar = document.getElementById('btnCancelar');
  btnSalvarPlanta = document.getElementById('btnSalvarPlanta');
  modalTitle = document.getElementById('modalTitle');
  plantMsg = document.getElementById('plantMsg');

  historyIntervalSelect = document.getElementById('historyInterval');

  // Novos elementos da interface amigável
  moistureRange = document.getElementById('moistureRange');
  moistureDisplay = document.getElementById('moistureDisplay');
  modeBtns = document.querySelectorAll('.mode-btn');
  simpleMode = document.getElementById('simpleMode');
  advancedMode = document.getElementById('advancedMode');
  timeBtns = document.querySelectorAll('.time-btn');
  freqBtns = document.querySelectorAll('.freq-btn');

  // Log dos elementos para debug
  console.log('Elementos encontrados:', {
    moistureRange: !!moistureRange,
    moistureDisplay: !!moistureDisplay,
    modeBtns: modeBtns.length,
    timeBtns: timeBtns.length,
    freqBtns: freqBtns.length
  });

  configRef = ref(db, `devices/${DEVICE_ID}/config`);

  // Garantir valor padrão para o campo de intervalo do sensor
  if (!sensorIntervalInput.value) {
    sensorIntervalInput.value = 2000;
  }

  // Event Listeners
  btnNovaPlanta.addEventListener('click', openNewPlantModal);
  closeModal.addEventListener('click', () => plantModal.classList.remove('active'));
  btnCancelar.addEventListener('click', () => plantModal.classList.remove('active'));
  btnSalvarPlanta.addEventListener('click', savePlant);
  plantSelect.addEventListener('change', onPlantSelectChange);
  btnSalvar.addEventListener('click', onSaveConfig);
  btnIrrigar.addEventListener('click', onIrrigate);
  historyIntervalSelect.addEventListener('change', monitorTelemetry);

  // Event Listeners para interface amigável usando delegação
  document.addEventListener('click', function(e) {
    if (e.target.classList.contains('mode-btn')) {
      switchMode(e);
    }
    if (e.target.classList.contains('time-btn')) {
      selectTimeOption(e);
    }
    if (e.target.classList.contains('freq-btn')) {
      selectFrequencyOption(e);
    }
  });
  
  // Event listener direto para o slider
  document.addEventListener('input', function(e) {
    if (e.target.id === 'moistureRange') {
      updateMoistureDisplay();
    }
  });

  // Inicializar funcionalidades
  loadPlants();
  monitorSnapshot();
  monitorConfig();
  initChart();
  monitorTelemetry();
  
  // Inicializar gauge com valor de demonstração
  if (gaugeValue && gauge) {
    gaugeValue.textContent = '42.5%';
    gauge.style.background = 'conic-gradient(var(--accent) 42.5%, #243244 42.5%)';
    console.log('🎯 Gauge inicializado com valor de demonstração');
  }
  
  // Inicializar interface amigável
  setTimeout(() => {
    // Inicializar slider
    updateMoistureDisplay();
    
    // Certificar que o modo simples está ativo por padrão
    const simpleMode = document.getElementById('simpleMode');
    const advancedMode = document.getElementById('advancedMode');
    
    if (simpleMode && advancedMode) {
      simpleMode.style.display = 'block';
      advancedMode.style.display = 'none';
    }
    
    // Garantir que os botões corretos estejam ativos
    const defaultTimeBtn = document.querySelector('.time-btn[data-seconds="30"]');
    const defaultFreqBtn = document.querySelector('.freq-btn[data-ms="2000"]');
    
    if (defaultTimeBtn) defaultTimeBtn.classList.add('active');
    if (defaultFreqBtn) defaultFreqBtn.classList.add('active');
    
    console.log('Interface amigável inicializada');
  }, 200);
}

// ========== GERENCIAMENTO DE PLANTAS ==========

function loadPlants() {
  const plantsRef = ref(db, 'plantTypes');
  onValue(plantsRef, (snapshot) => {
    plantsData = snapshot.val() || {};
    
    // Adicionar plantas populares se não existirem
    if (Object.keys(plantsData).length === 0) {
      createDefaultPlants();
    }
    
    renderPlantsList();
    updatePlantSelect();
  });
}

function createDefaultPlants() {
  const defaultPlants = [
    {
      name: "🍅 Tomate",
      low: 30,
      high: 40,
      tMaxIrrSec: 45,
      tMinGapMin: 20,
      rawDry: 3100,
      rawWet: 1400,
      sensorReadIntervalMs: 3000
    },
    {
      name: "🥬 Alface",
      low: 40,
      high: 50,
      tMaxIrrSec: 25,
      tMinGapMin: 15,
      rawDry: 3100,
      rawWet: 1400,
      sensorReadIntervalMs: 2000
    },
    {
      name: "🌿 Manjericão",
      low: 35,
      high: 45,
      tMaxIrrSec: 30,
      tMinGapMin: 18,
      rawDry: 3100,
      rawWet: 1400,
      sensorReadIntervalMs: 2000
    },
    {
      name: "🌹 Rosa",
      low: 25,
      high: 35,
      tMaxIrrSec: 40,
      tMinGapMin: 30,
      rawDry: 3100,
      rawWet: 1400,
      sensorReadIntervalMs: 4000
    },
    {
      name: "🌵 Suculenta",
      low: 15,
      high: 25,
      tMaxIrrSec: 15,
      tMinGapMin: 60,
      rawDry: 3100,
      rawWet: 1400,
      sensorReadIntervalMs: 10000
    }
  ];

  const plantsRef = ref(db, 'plantTypes');
  defaultPlants.forEach(plant => {
    const plantRef = push(plantsRef);
    set(plantRef, {
      ...plant,
      updatedAt: Date.now()
    });
  });
}

function renderPlantsList() {
  const keys = Object.keys(plantsData);
  if (keys.length === 0) {
    plantsList.innerHTML = '<div class="muted">Nenhuma planta cadastrada ainda.</div>';
    return;
  }
  
  let html = '';
  keys.forEach(key => {
    const plant = plantsData[key];
    html += `
      <div class="plant-item">
        <div class="plant-info">
          <div class="plant-name">${plant.name}</div>
          <div class="plant-details">
            Umidade: ${plant.low}%-${plant.high}% | 
            Irrigação: ${plant.tMaxIrrSec}s | 
            Intervalo: ${plant.tMinGapMin}min
          </div>
        </div>
        <div class="plant-actions">
          <button class="btn small primary" data-action="apply" data-id="${key}">Aplicar</button>
          <button class="btn small" data-action="edit" data-id="${key}">Editar</button>
          <button class="btn small danger" data-action="delete" data-id="${key}">Excluir</button>
        </div>
      </div>
    `;
  });
  plantsList.innerHTML = html;

  // Adicionar event listeners
  document.querySelectorAll('[data-action="apply"]').forEach(btn => {
    btn.addEventListener('click', (e) => applyPlantConfig(e.target.dataset.id));
  });
  document.querySelectorAll('[data-action="edit"]').forEach(btn => {
    btn.addEventListener('click', (e) => editPlant(e.target.dataset.id));
  });
  document.querySelectorAll('[data-action="delete"]').forEach(btn => {
    btn.addEventListener('click', (e) => deletePlant(e.target.dataset.id));
  });
}

function updatePlantSelect() {
  const keys = Object.keys(plantsData);
  let options = '<option value="">Configuração manual</option>';
  keys.forEach(key => {
    options += `<option value="${key}">${plantsData[key].name}</option>`;
  });
  plantSelect.innerHTML = options;
}

function applyPlantConfig(plantId) {
  const plant = plantsData[plantId];
  if (!plant) return;
  
  lowInput.value = plant.low;
  highInput.value = plant.high;
  tmaxInput.value = plant.tMaxIrrSec;
  tgapInput.value = plant.tMinGapMin;
  rawDryInput.value = plant.rawDry;
  rawWetInput.value = plant.rawWet;
  sensorIntervalInput.value = plant.sensorReadIntervalMs || 2000;
  plantSelect.value = plantId;
  
  saveConfig(plant.name);
}

function editPlant(plantId) {
  const plant = plantsData[plantId];
  if (!plant) return;
  
  currentEditPlantId = plantId;
  modalTitle.textContent = 'Editar Planta';
  
  document.getElementById('plantName').value = plant.name;
  document.getElementById('plantLow').value = plant.low;
  document.getElementById('plantHigh').value = plant.high;
  document.getElementById('plantTmax').value = plant.tMaxIrrSec;
  document.getElementById('plantTgap').value = plant.tMinGapMin;
  document.getElementById('plantRawDry').value = plant.rawDry;
  document.getElementById('plantRawWet').value = plant.rawWet;
  document.getElementById('plantSensorInterval').value = plant.sensorReadIntervalMs || 2000;
  
  plantModal.classList.add('active');
}

function deletePlant(plantId) {
  if (!confirm('Tem certeza que deseja excluir esta planta?')) return;
  
  const plantRef = ref(db, `plantTypes/${plantId}`);
  remove(plantRef)
    .then(() => console.log('Planta excluída com sucesso'))
    .catch(err => {
      console.error('Erro ao excluir planta:', err);
      alert('Erro ao excluir planta');
    });
}

function openNewPlantModal() {
  currentEditPlantId = null;
  modalTitle.textContent = 'Nova Planta';
  
  document.getElementById('plantName').value = '';
  document.getElementById('plantLow').value = 35;
  document.getElementById('plantHigh').value = 45;
  document.getElementById('plantTmax').value = 30;
  document.getElementById('plantTgap').value = 15;
  document.getElementById('plantRawDry').value = 3100;
  document.getElementById('plantRawWet').value = 1400;
  document.getElementById('plantSensorInterval').value = 2000;
  plantMsg.textContent = '';
  
  plantModal.classList.add('active');
}

function savePlant() {
  const name = document.getElementById('plantName').value.trim();
  const low = parseInt(document.getElementById('plantLow').value);
  const high = parseInt(document.getElementById('plantHigh').value);
  const tmax = parseInt(document.getElementById('plantTmax').value);
  const tgap = parseInt(document.getElementById('plantTgap').value);
  const rawDry = parseInt(document.getElementById('plantRawDry').value);
  const rawWet = parseInt(document.getElementById('plantRawWet').value);
  const sensorIntervalMs = parseInt(document.getElementById('plantSensorInterval').value) || 2000;
  
  if (!name) {
    plantMsg.textContent = '❌ Nome da planta é obrigatório';
    plantMsg.className = 'helper error';
    return;
  }
  
  const plantData = {
    name,
    low,
    high,
    tMaxIrrSec: tmax,
    tMinGapMin: tgap,
    rawDry,
    rawWet,
    sensorReadIntervalMs: sensorIntervalMs,
    updatedAt: Date.now()
  };
  
  console.log('Salvando planta:', plantData); // Debug para verificar os dados da planta
  
  let plantRef;
  if (currentEditPlantId) {
    plantRef = ref(db, `plantTypes/${currentEditPlantId}`);
  } else {
    plantRef = push(ref(db, 'plantTypes'));
  }
  
  set(plantRef, plantData)
    .then(() => {
      plantMsg.textContent = '✅ Planta salva com sucesso!';
      plantMsg.className = 'helper success';
      setTimeout(() => {
        plantModal.classList.remove('active');
      }, 1000);
    })
    .catch(err => {
      console.error('Erro ao salvar planta:', err);
      plantMsg.textContent = '❌ Erro ao salvar planta';
      plantMsg.className = 'helper error';
    });
}

function onPlantSelectChange(e) {
  const plantId = e.target.value;
  if (plantId) {
    applyPlantConfig(plantId);
  }
}

// ========== MONITORAMENTO E CONTROLE ==========

function monitorSnapshot() {
  const snapshotRef = ref(db, `devices/${DEVICE_ID}/snapshot`);
  onValue(snapshotRef, (snapshot) => {
    const data = snapshot.val();
    
    // Se não há dados, criar dados de demonstração
    let moisture, pump, state;
    if (!data) {
      moisture = 42.5; // Valor de demonstração
      pump = false;
      state = 'IDLE';
      console.log('📊 Usando dados de demonstração para o gauge');
    } else {
      moisture = data.soilMoisture || 0;
      pump = data.pumpState || false;
      state = data.state || 'IDLE';
    }
    
    // Atualizar gauge com porcentagem visível
    if (gaugeValue) {
      gaugeValue.textContent = `${moisture.toFixed(1)}%`;
    }
    
    if (gauge) {
      const percent = Math.min(100, Math.max(0, moisture));
      gauge.style.background = `conic-gradient(var(--accent) ${percent}%, #243244 ${percent}%)`;
    }
    
    if (stateBadge) {
      stateBadge.textContent = state;
      stateBadge.className = state === 'IDLE' ? 'badge ok' : 'badge warn';
    }
    
    if (pumpBadge) {
      pumpBadge.textContent = pump ? 'LIGADA' : 'DESLIGADA';
      pumpBadge.className = pump ? 'badge ok' : 'badge';
    }
    
    if (lastUpdate) {
      lastUpdate.textContent = `Última atualização: ${new Date().toLocaleTimeString()}`;
    }
  });
}

function monitorConfig() {
  onValue(configRef, (snapshot) => {
    const cfg = snapshot.val();
    if (!cfg) return;
    
    lowInput.value = cfg.moistureLowPct || 35;
    highInput.value = cfg.moistureHighPct || 45;
    tmaxInput.value = cfg.tMaxIrrSec || 30;
    tgapInput.value = cfg.tMinGapMin || 15;
    rawDryInput.value = cfg.rawDry || 3100;
    rawWetInput.value = cfg.rawWet || 1400;
    sensorIntervalInput.value = cfg.sensorReadIntervalMs || 2000;
    
    // Sincronizar com interface amigável
    if (moistureRange) {
      moistureRange.value = cfg.moistureLowPct || 35;
      updateMoistureDisplay();
    }
    syncAdvancedToSimple();
    
    rangeLabel.textContent = `${cfg.moistureLowPct}% - ${cfg.moistureHighPct}%`;
    
    if (cfg.plantName) {
      currentPlantLabel.textContent = cfg.plantName;
      currentPlantLabel.className = 'badge ok';
    } else {
      currentPlantLabel.textContent = 'Manual';
      currentPlantLabel.className = 'badge';
    }
  });
}

function saveConfig(plantName = null) {
  // Garantir que sensorReadIntervalMs sempre tenha um valor válido
  const sensorIntervalValue = parseInt(sensorIntervalInput.value) || 2000;
  
  const config = {
    moistureLowPct: parseInt(lowInput.value),
    moistureHighPct: parseInt(highInput.value),
    tMaxIrrSec: parseInt(tmaxInput.value),
    tMinGapMin: parseInt(tgapInput.value),
    rawDry: parseInt(rawDryInput.value),
    rawWet: parseInt(rawWetInput.value),
    sensorReadIntervalMs: sensorIntervalValue,
    plantName: plantName || null,
    updatedAt: Date.now()
  };
  
  console.log('Salvando configuração:', config); // Debug para verificar se o campo está sendo enviado
  
  set(configRef, config)
    .then(() => {
      cfgMsg.textContent = '✅ Configuração salva!';
      cfgMsg.className = 'helper success';
      setTimeout(() => { cfgMsg.textContent = ''; }, 3000);
    })
    .catch(err => {
      console.error('Erro ao salvar config:', err);
      cfgMsg.textContent = '❌ Erro ao salvar';
      cfgMsg.className = 'helper error';
    });
}

function onSaveConfig() {
  const selectedPlant = plantSelect.value;
  const plantName = selectedPlant ? plantsData[selectedPlant]?.name : null;
  saveConfig(plantName);
}

function onIrrigate() {
  const cmdRef = ref(db, `devices/${DEVICE_ID}/commands/irrigateNow`);
  set(cmdRef, true)
    .then(() => console.log('Comando de irrigação enviado'))
    .catch(err => console.error('Erro ao enviar comando:', err));
}

// ========== GRÁFICO E TELEMETRIA ==========

function formatDateTime(ts) {
  const d = new Date(ts);
  const pad = n => n.toString().padStart(2, '0');
  return `${pad(d.getDate())}/${pad(d.getMonth()+1)}/${d.getFullYear()} ${pad(d.getHours())}:${pad(d.getMinutes())}:${pad(d.getSeconds())}`;
}

function initChart() {
  const ctx = document.getElementById('telemetryChart').getContext('2d');
  chart = new Chart(ctx, {
    type: 'line',
    data: {
      labels: [],
      datasets: [{
        label: 'Umidade (%)',
        data: [],
        borderColor: '#60a5fa',
        tension: 0.4
      }]
    },
    options: {
      responsive: true,
      maintainAspectRatio: false,
      scales: {
        y: { beginAtZero: true, max: 100 }
      }
    }
  });
}

function monitorTelemetry() {
  const telemetryRef = ref(db, `devices/${DEVICE_ID}/telemetry`);
  onValue(telemetryRef, (snapshot) => {
    const data = snapshot.val();
    if (!data) return;

    const now = Date.now();
    let cutoff = now - 3600000; // padrão 1h

    switch(historyIntervalSelect.value) {
      case '1h': cutoff = now - 3600000; break;
      case '3d': cutoff = now - 3*24*3600000; break;
      case '1w': cutoff = now - 7*24*3600000; break;
    }

    const filteredEntries = Object.values(data).filter(e => e.tsMs >= cutoff);

    // Ordenar por timestamp crescente para gráfico
    filteredEntries.sort((a,b) => a.tsMs - b.tsMs);

    chart.data.labels = filteredEntries.map(e => new Date(e.tsMs).toLocaleTimeString());
    chart.data.datasets[0].data = filteredEntries.map(e => e.soilMoisture);
    chart.update();

    // Mostrar os 10 eventos mais recentes dentro do filtro
    const tbody = document.getElementById('telemetryBody');
    const recentEntries = filteredEntries.slice(-10).reverse();
    tbody.innerHTML = recentEntries.map(e => `
      <tr>
        <td>${formatDateTime(e.tsMs)}</td>
        <td>${e.soilMoisture.toFixed(1)}%</td>
        <td>${e.pumpState ? '🟢 LIGADA' : '⚪ DESLIGADA'}</td>
      </tr>
    `).join('');
  });
}

// ========== INTERFACE AMIGÁVEL ==========

function switchMode(e) {
  const mode = e.target.dataset.mode;
  console.log('Mudando para modo:', mode);
  
  // Atualizar botões
  modeBtns.forEach(btn => btn.classList.remove('active'));
  e.target.classList.add('active');
  
  // Mostrar/esconder seções
  if (mode === 'simple') {
    if (simpleMode) simpleMode.style.display = 'block';
    if (advancedMode) advancedMode.style.display = 'none';
    syncSimpleToAdvanced();
    console.log('Modo simples ativado');
  } else {
    if (simpleMode) simpleMode.style.display = 'none';
    if (advancedMode) advancedMode.style.display = 'block';
    syncAdvancedToSimple();
    console.log('Modo avançado ativado');
  }
}

function updateMoistureDisplay() {
  const moistureRange = document.getElementById('moistureRange');
  const moistureDisplay = document.getElementById('moistureDisplay');
  
  if (!moistureRange || !moistureDisplay) {
    console.log('updateMoistureDisplay: elementos não encontrados', {
      moistureRange: !!moistureRange,
      moistureDisplay: !!moistureDisplay
    });
    return;
  }
  
  const value = moistureRange.value;
  moistureDisplay.textContent = `${value}%`;
  
  // Sincronizar com campo avançado
  const lowInput = document.getElementById('low');
  const highInput = document.getElementById('high');
  
  if (lowInput) lowInput.value = value;
  if (highInput) highInput.value = parseInt(value) + 10; // 10% acima para parar
  
  console.log('Umidade atualizada para:', value + '%');
}

function selectTimeOption(e) {
  const seconds = parseInt(e.target.dataset.seconds);
  console.log('Tempo de irrigação selecionado:', seconds + 's');
  
  // Atualizar botões - buscar novamente para garantir
  const allTimeBtns = document.querySelectorAll('.time-btn');
  allTimeBtns.forEach(btn => btn.classList.remove('active'));
  e.target.classList.add('active');
  
  // Sincronizar com campo avançado
  const tmaxInput = document.getElementById('tmax');
  if (tmaxInput) {
    tmaxInput.value = seconds;
    console.log('Campo avançado atualizado:', tmaxInput.value);
  }
}

function selectFrequencyOption(e) {
  const ms = parseInt(e.target.dataset.ms);
  console.log('Frequência selecionada:', ms + 'ms');
  
  // Atualizar botões - buscar novamente para garantir
  const allFreqBtns = document.querySelectorAll('.freq-btn');
  allFreqBtns.forEach(btn => btn.classList.remove('active'));
  e.target.classList.add('active');
  
  // Sincronizar com campo avançado
  const sensorIntervalInput = document.getElementById('sensorInterval');
  if (sensorIntervalInput) {
    sensorIntervalInput.value = ms;
    console.log('Campo sensor interval atualizado:', sensorIntervalInput.value);
  }
}

function syncSimpleToAdvanced() {
  // Sincronizar valores do modo simples para avançado
  const moistureValue = parseInt(moistureRange.value);
  lowInput.value = moistureValue;
  highInput.value = moistureValue + 10;
  
  const activeTimeBtn = document.querySelector('.time-btn.active');
  if (activeTimeBtn) {
    tmaxInput.value = activeTimeBtn.dataset.seconds;
  }
  
  const activeFreqBtn = document.querySelector('.freq-btn.active');
  if (activeFreqBtn) {
    sensorIntervalInput.value = activeFreqBtn.dataset.ms;
  }
}

function syncAdvancedToSimple() {
  // Sincronizar valores do modo avançado para simples
  if (moistureRange && lowInput) {
    moistureRange.value = lowInput.value;
    updateMoistureDisplay();
  }
  
  // Atualizar botão de tempo ativo
  if (timeBtns.length > 0 && tmaxInput) {
    timeBtns.forEach(btn => {
      btn.classList.remove('active');
      if (btn.dataset.seconds === tmaxInput.value) {
        btn.classList.add('active');
      }
    });
  }
  
  // Atualizar botão de frequência ativo
  if (freqBtns.length > 0 && sensorIntervalInput) {
    freqBtns.forEach(btn => {
      btn.classList.remove('active');
      if (btn.dataset.ms === sensorIntervalInput.value) {
        btn.classList.add('active');
      }
    });
  }
}

// Inicializar quando o DOM carregar
if (document.readyState === 'loading') {
  document.addEventListener('DOMContentLoaded', init);
} else {
  init();
}