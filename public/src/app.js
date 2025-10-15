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

  configRef = ref(db, `devices/${DEVICE_ID}/config`);

  // Event Listeners
  btnNovaPlanta.addEventListener('click', openNewPlantModal);
  closeModal.addEventListener('click', () => plantModal.classList.remove('active'));
  btnCancelar.addEventListener('click', () => plantModal.classList.remove('active'));
  btnSalvarPlanta.addEventListener('click', savePlant);
  plantSelect.addEventListener('change', onPlantSelectChange);
  btnSalvar.addEventListener('click', onSaveConfig);
  btnIrrigar.addEventListener('click', onIrrigate);
  historyIntervalSelect.addEventListener('change', monitorTelemetry);

  // Inicializar funcionalidades
  loadPlants();
  monitorSnapshot();
  monitorConfig();
  initChart();
  monitorTelemetry();
}

// ========== GERENCIAMENTO DE PLANTAS ==========

function loadPlants() {
  const plantsRef = ref(db, 'plantTypes');
  onValue(plantsRef, (snapshot) => {
    plantsData = snapshot.val() || {};
    renderPlantsList();
    updatePlantSelect();
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
  document.getElementById('sensorInterval').value = plant.sensorReadIntervalMs || 2000;
  
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
  document.getElementById('sensorInterval').value = 2000;
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
  const sensorIntervalMs = parseInt(document.getElementById('sensorInterval').value);
  
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
    if (!data) return;
    
    const moisture = data.soilMoisture || 0;
    const pump = data.pumpState || false;
    const state = data.state || 'IDLE';
    
    gaugeValue.textContent = `${moisture.toFixed(1)}%`;
    const percent = Math.min(100, Math.max(0, moisture));
    gauge.style.background = `conic-gradient(var(--accent) ${percent}%, #243244 ${percent}%)`;
    
    stateBadge.textContent = state;
    stateBadge.className = state === 'IDLE' ? 'badge ok' : 'badge warn';
    
    pumpBadge.textContent = pump ? 'LIGADA' : 'DESLIGADA';
    pumpBadge.className = pump ? 'badge ok' : 'badge';
    
    lastUpdate.textContent = `Última atualização: ${new Date().toLocaleTimeString()}`;
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
  const config = {
    moistureLowPct: parseInt(lowInput.value),
    moistureHighPct: parseInt(highInput.value),
    tMaxIrrSec: parseInt(tmaxInput.value),
    tMinGapMin: parseInt(tgapInput.value),
    rawDry: parseInt(rawDryInput.value),
    rawWet: parseInt(rawWetInput.value),
    sensorReadIntervalMs: parseInt(sensorIntervalInput.value),
    plantName: plantName || null,
    updatedAt: Date.now()
  };
  
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

// Inicializar quando o DOM carregar
if (document.readyState === 'loading') {
  document.addEventListener('DOMContentLoaded', init);
} else {
  init();
}