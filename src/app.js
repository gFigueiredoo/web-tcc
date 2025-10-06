// src/app.js
import { auth, db } from "./firebase.js";
import { signInAnonymously } from "firebase/auth";
import {
  ref, child, get, onValue, update, set,
  query, limitToLast, onChildAdded
} from "firebase/database";

// ====== CONFIG ======
const deviceId = "esp32-vaso-01"; // mantenha igual ao do ESP32
// ====================

// DOM
const deviceIdBadge = document.getElementById('deviceIdBadge');
const gaugeEl   = document.getElementById('gauge');
const gaugeVal  = document.getElementById('gaugeValue');
const rangeLabel= document.getElementById('rangeLabel');
const pumpBadge = document.getElementById('pumpBadge');
const stateBadge= document.getElementById('stateBadge');
const lastUpdate= document.getElementById('lastUpdate');
const tbody     = document.getElementById('telemetryBody');

const lowEl   = document.getElementById('low');
const highEl  = document.getElementById('high');
const tmaxEl  = document.getElementById('tmax');
const tgapEl  = document.getElementById('tgap');
const rawDryEl= document.getElementById('rawDry');
const rawWetEl= document.getElementById('rawWet');
const cfgMsg  = document.getElementById('cfgMsg');

document.getElementById('btnIrrigar').addEventListener('click', irrigarAgora);
document.getElementById('btnSalvar').addEventListener('click', salvarConfig);

// Firebase
signInAnonymously(auth).catch(e => console.error('Auth anônima falhou:', e));
const baseRef = ref(db, `devices/${deviceId}`);
deviceIdBadge.textContent = `device: ${deviceId}`;

// Gauge
function setGauge(pct){
  const p = isFinite(pct) ? Math.max(0, Math.min(100, pct)) : 0;
  gaugeEl.style.background = `conic-gradient(var(--accent) ${p}%, #243244 ${p}%)`;
  gaugeVal.textContent = `${p.toFixed(0)}%`;
}

// Badges
function setBadge(el, text, type){
  el.textContent = text;
  el.className = `badge ${type || ''}`;
}

// Chart
const ctx = document.getElementById('telemetryChart');
const labels = [];
const moistData = [];
const MAX_POINTS = 50;

const chart = new Chart(ctx, {
  type: 'line',
  data: {
    labels,
    datasets: [{
      label: 'Umidade (%)',
      data: moistData,
      borderColor: '#60a5fa',
      backgroundColor: 'rgba(96,165,250,.15)',
      borderWidth: 2,
      tension: 0.25,
      pointRadius: 0,
      fill: true
    }]
  },
  options: {
    responsive: true,
    maintainAspectRatio: false,
    scales: {
      y: { beginAtZero: true, max: 100, grid: { color: 'rgba(148,163,184,.15)'} },
      x: { ticks: { maxTicksLimit: 6 }, grid: { display:false } }
    },
    plugins: {
      legend: { labels: { color: '#cbd5e1' } },
      tooltip: { mode: 'index', intersect: false }
    }
  }
});

function pushPoint(value){
  const label = new Date().toLocaleTimeString();
  labels.push(label);
  moistData.push(value);

  if (labels.length > MAX_POINTS) { labels.shift(); moistData.shift(); }
  chart.update();
}

// Snapshot em tempo real
onValue(child(baseRef, 'snapshot'), (snap) => {
  const v = snap.val() || {};
  if (typeof v.soilMoisture === 'number') setGauge(v.soilMoisture);
  setBadge(pumpBadge, v.pumpState ? 'LIGADA' : 'DESLIGADA', v.pumpState ? 'ok' : '');
  setBadge(stateBadge, v.state || '—', v.state === 'IRRIGATING' ? 'ok' :
                                   v.state === 'LOCKOUT' ? 'warn' : '');
  lastUpdate.textContent = `Última atualização: ${new Date().toLocaleTimeString()}`;
});

// Carrega config inicial
get(child(baseRef, 'config')).then((snap) => {
  const c = snap.val() || {};
  if (c.moisture) { lowEl.value  = c.moisture.low ?? 35; highEl.value = c.moisture.high ?? 45; }
  if (c.timers)   { tmaxEl.value = c.timers.tMaxIrrSec ?? 30; tgapEl.value = c.timers.tMinGapMin ?? 15; }
  if (c.calibration) { rawDryEl.value = c.calibration.rawDry ?? 3100; rawWetEl.value = c.calibration.rawWet ?? 1400; }
  rangeLabel.textContent = `${lowEl.value}% – ${highEl.value}%`;
});

// Atualiza label da faixa ao editar
[lowEl, highEl].forEach(inp => inp.addEventListener('input', () => {
  rangeLabel.textContent = `${lowEl.value}% – ${highEl.value}%`;
}));

// Últimas leituras (stream)
const teleQuery = query(child(baseRef, 'telemetry'), limitToLast(50));
onChildAdded(teleQuery, (snap) => {
  const v = snap.val() || {};
  if (typeof v.soilMoisture === 'number') pushPoint(v.soilMoisture);

  const tr = document.createElement('tr');
  tr.innerHTML = `<td>${new Date().toLocaleTimeString()}</td>
                  <td>${v.soilMoisture != null ? v.soilMoisture.toFixed(1) : ''}</td>
                  <td>${v.pumpState ? '1' : '0'}</td>`;
  tbody.appendChild(tr);
  if (tbody.children.length > 100) tbody.removeChild(tbody.firstChild);
});

// Ações
async function irrigarAgora(){
  try{
    await set(child(baseRef, 'commands/irrigateNow'), true);
    feedback('Comando enviado.', true);
  }catch(e){
    feedback('Falha ao enviar comando: ' + e.message, false);
  }
}

async function salvarConfig(){
  const cfg = {
    moisture: { low: Number(lowEl.value), high: Number(highEl.value) },
    timers: { tMaxIrrSec: Number(tmaxEl.value), tMinGapMin: Number(tgapEl.value) },
    calibration: { rawDry: Number(rawDryEl.value), rawWet: Number(rawWetEl.value) }
  };
  try{
    await update(child(baseRef, 'config'), cfg);
    feedback('Config salva.', true);
    rangeLabel.textContent = `${cfg.moisture.low}% – ${cfg.moisture.high}%`;
  }catch(e){
    feedback('Erro ao salvar: ' + e.message, false);
  }
}

function feedback(msg, ok){
  cfgMsg.textContent = msg;
  cfgMsg.className = ok ? 'success' : 'error';
  setTimeout(() => { cfgMsg.textContent = ''; cfgMsg.className=''; }, 2500);
}