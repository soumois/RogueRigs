// node vault/scripts/crew-board.js "<orchestrator note>" -> Saved/CrewBoard/crew-board.html (crew dashboard page; publish it as an artifact).
const fs = require('fs'), path = require('path');
const ROOT = path.resolve(__dirname, '..', 'reference', 'tasks');
const OUT = path.resolve(__dirname, '..', '..', 'Saved', 'CrewBoard', 'crew-board.html');
fs.mkdirSync(path.dirname(OUT), { recursive: true });
const note = process.argv[2] || '';

function parse(file) {
  const s = fs.readFileSync(file, 'utf8').replace(/\r\n/g, '\n'); // some nodes are CRLF
  const fm = {}; const m = s.match(/^---\n([\s\S]*?)\n---/);
  if (m) m[1].split('\n').forEach(l => { const i = l.indexOf(':'); if (i > 0) fm[l.slice(0, i).trim()] = l.slice(i + 1).trim().replace(/^"|"$/g, ''); });
  const field = k => { const r = s.match(new RegExp('^- ' + k + ': (.*)$', 'm')); return r ? r[1] : ''; };
  const title = (s.match(/^# (.*)$/m) || [])[1] || fm.id;
  return { ...fm, title, agent: field('agent'), result: field('result'), waiting: field('waiting_for'), next: field('next_step'), goal: field('goal') };
}
const globals = fs.readdirSync(ROOT).filter(f => /^task-\d+.*\.md$/.test(f)).sort().map(f => {
  const g = parse(path.join(ROOT, f)); const dir = path.join(ROOT, f.replace(/\.md$/, ''));
  g.subs = fs.existsSync(dir) ? fs.readdirSync(dir).filter(x => x.endsWith('.md')).sort().map(x => parse(path.join(dir, x))) : [];
  return g;
});
const active = globals.filter(g => +g.id.split('-')[1] >= 13);
const history = globals.filter(g => +g.id.split('-')[1] < 13);
const order = ['task-013-01', 'task-014', 'task-015', 'task-016', 'task-013-02', 'task-017', 'task-018', 'task-019', 'task-020', 'task-021', 'task-022', 'task-023', 'task-024', 'task-025', 'task-029', 'task-026', 'task-027', 'task-028'];
const queue = [];
order.forEach(p => active.forEach(g => g.subs.forEach(s => { if (s.id.startsWith(p) && !queue.includes(s)) queue.push(s); })));
const esc = t => String(t || '').replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/\[\[([^\]]+)\]\]/g, '$1').replace(/`/g, '');
const STATUS = { 'todo': 'À venir', 'in-progress': 'En cours', 'review': 'En review', 'done': 'Fait', 'blocked': 'Bloqué', 'cancelled': 'Annulé', 'wait-answers': 'En attente de réponse' };
const AG = {
  orchestrator: ['Orchestrator', 'Planifie, délègue, vérifie, commite'],
  cpp: ['C++', 'Code gameplay, build, tests headless'],
  bp: ['Blueprint', 'Éditeur via MCP : Blueprints, UI, PIE'],
  'level-design': ['Level Design', 'Layouts d’arène en JSON'],
};
const cur = queue.find(s => s.status === 'in-progress') || null;
const done = queue.filter(s => s.status === 'done').length;
function agentCard(key) {
  const mine = queue.filter(s => s.agent === key);
  const now = mine.find(s => s.status === 'in-progress');
  const nextT = mine.find(s => s.status === 'todo');
  const fin = mine.filter(s => s.status === 'done').length;
  let state = now ? 'busy' : (key === 'orchestrator' ? 'busy' : 'idle');
  let body = now ? `<p class="now-id">${now.id}</p><p class="now-t">${esc(now.description)}</p>`
    : key === 'orchestrator' ? `<p class="now-id">${cur ? 'suit ' + cur.id : 'en attente'}</p><p class="now-t">${esc(note) || 'Coordonne la file et vérifie chaque livraison.'}</p>`
    : `<p class="now-t muted">En attente${nextT ? ' — prochaine : <b>' + nextT.id + '</b>' : ''}</p>`;
  const m = key === 'orchestrator' ? `${done}/${queue.length} sous-tâches validées` : `${fin}/${mine.length} faites`;
  return `<article class="agent ${state}"><header><span class="dot" aria-hidden="true"></span><h3>${AG[key][0]}</h3><span class="meta">${m}</span></header><p class="role">${AG[key][1]}</p>${body}</article>`;
}
function row(s, i) {
  return `<li class="step s-${s.status}"><span class="idx">${String(i + 1).padStart(2, '0')}</span><div class="sbody"><div class="sline"><code>${s.id}</code><span class="ag">${AG[s.agent] ? AG[s.agent][0] : esc(s.agent)}</span><span class="pill p-${s.status}">${STATUS[s.status] || s.status}</span></div><p class="sdesc">${esc(s.description)}</p>${s.status === 'wait-answers' ? `<p class="swait">Attend : ${esc(s.waiting)}</p>` : ''}${s.status === 'done' || s.status === 'blocked' || s.status === 'cancelled' ? `<details><summary>Résultat</summary><p>${esc(s.result)}</p></details>` : ''}</div></li>`;
}
const stamp = new Date().toLocaleString('fr-FR', { timeZone: 'Europe/Paris', dateStyle: 'medium', timeStyle: 'short' });
const html = `<title>Rogue Rigs Crew Board</title>
<link rel="preconnect" href="https://fonts.googleapis.com"><link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
<link rel="stylesheet" href="https://fonts.googleapis.com/css2?family=Saira+Condensed:wght@500;700&family=IBM+Plex+Sans:wght@400;500;600&family=IBM+Plex+Mono:wght@400;500&display=swap">
<style>
/* Layout: control-room board — agent bays on top, ordered work queue below, history folded at the end */
:root{
 --bg:#eef1f0;--panel:#ffffff;--ink:#16201f;--mute:#5d6b69;--line:#d3dbd9;
 --teal:#0f7f78;--rust:#b2552c;--ok:#2f7d3d;--warn:#a86b00;--bad:#b3261e;--wash:#e3ecea;
 --display:"Saira Condensed","Arial Narrow",sans-serif;--body:"IBM Plex Sans",system-ui,sans-serif;--mono:"IBM Plex Mono",ui-monospace,monospace;
}
@media (prefers-color-scheme:dark){:root:not([data-theme="light"]){--bg:#0f1514;--panel:#172120;--ink:#e3ecea;--mute:#91a19e;--line:#2a3735;--teal:#3cc3b8;--rust:#e08257;--ok:#6cc47a;--warn:#e0a83a;--bad:#ef7b72;--wash:#1d2a28;color-scheme:dark}}
:root[data-theme="dark"]{--bg:#0f1514;--panel:#172120;--ink:#e3ecea;--mute:#91a19e;--line:#2a3735;--teal:#3cc3b8;--rust:#e08257;--ok:#6cc47a;--warn:#e0a83a;--bad:#ef7b72;--wash:#1d2a28;color-scheme:dark}
body{background:var(--bg);color:var(--ink);font:15px/1.5 var(--body)}
.wrap{max-width:1080px;margin:0 auto;padding-inline:16px;padding-block:28px 48px;display:grid;gap:32px}
h1,h2,h3{font-family:var(--display);letter-spacing:.02em;margin:0;text-wrap:balance}
h1{font-size:2.3rem;line-height:1}
h2{font-size:1.35rem;text-transform:uppercase;letter-spacing:.08em;color:var(--mute)}
.top{display:flex;flex-wrap:wrap;gap:12px 24px;align-items:end;justify-content:space-between}
.sub{margin:6px 0 0;color:var(--mute)}
.gauge{min-width:0;flex:1 1 260px;max-width:420px}
.gauge .lbl{display:flex;justify-content:space-between;font:500 .8rem var(--mono);color:var(--mute);font-variant-numeric:tabular-nums}
.bar{height:10px;background:var(--wash);border:1px solid var(--line);border-radius:2px;overflow:hidden;margin-top:6px}
.bar i{display:block;height:100%;background:repeating-linear-gradient(135deg,var(--teal) 0 8px,color-mix(in srgb,var(--teal) 80%,var(--bg)) 8px 16px)}
.bays{display:grid;grid-template-columns:repeat(auto-fit,minmax(230px,1fr));gap:12px}
.agent{background:var(--panel);border:1px solid var(--line);border-radius:4px;padding:14px 16px;display:grid;gap:6px;align-content:start;min-width:0}
.agent header{display:flex;align-items:center;gap:8px}
.agent h3{font-size:1.25rem}
.agent .meta{margin-left:auto;font:.75rem var(--mono);color:var(--mute);font-variant-numeric:tabular-nums}
.dot{width:9px;height:9px;border-radius:50%;background:var(--line)}
.busy .dot{background:var(--teal);box-shadow:0 0 0 0 var(--teal);animation:ping 1.8s infinite}
.busy{border-top:3px solid var(--teal)}
.role{margin:0;color:var(--mute);font-size:.85rem}
.now-id{margin:4px 0 0;font:500 .85rem var(--mono);color:var(--teal)}
.now-t{margin:0;font-size:.9rem}.muted{color:var(--mute)}
@keyframes ping{0%{box-shadow:0 0 0 0 color-mix(in srgb,var(--teal) 60%,transparent)}70%{box-shadow:0 0 0 7px transparent}100%{box-shadow:0 0 0 0 transparent}}
@media (prefers-reduced-motion:reduce){.busy .dot{animation:none}}
.queue{list-style:none;margin:0;padding:0;border-top:1px solid var(--line)}
.step{display:grid;grid-template-columns:2.4rem 1fr;gap:10px;padding:12px 4px;border-bottom:1px solid var(--line)}
.idx{font:500 .8rem var(--mono);color:var(--mute);padding-top:3px;font-variant-numeric:tabular-nums}
.sbody{min-width:0}
.sline{display:flex;flex-wrap:wrap;gap:6px 10px;align-items:center}
.sline code{font:500 .85rem var(--mono);overflow-wrap:anywhere}
.ag{font:.72rem var(--mono);text-transform:uppercase;letter-spacing:.08em;color:var(--mute)}
.pill{margin-left:auto;font:600 .72rem var(--body);text-transform:uppercase;letter-spacing:.06em;padding:2px 8px;border-radius:2px;border:1px solid currentColor}
.p-done{color:var(--ok)}.p-in-progress{color:var(--teal);background:var(--wash)}.p-todo{color:var(--mute)}.p-blocked{color:var(--bad)}.p-review{color:var(--warn)}.p-cancelled{color:var(--mute);text-decoration:line-through}.p-wait-answers{color:var(--rust);background:var(--wash)}
.swait{margin:4px 0 0;font-size:.85rem;color:var(--rust);overflow-wrap:anywhere}
.sdesc{margin:4px 0 0;font-size:.9rem;color:var(--mute)}
.s-in-progress{background:var(--wash)}.s-in-progress .sdesc{color:var(--ink)}
.s-done .sline code{color:var(--mute)}
details{margin-top:6px;font-size:.85rem}summary{cursor:pointer;color:var(--rust);font-weight:500}
summary:focus-visible{outline:2px solid var(--teal);outline-offset:2px}
details p{margin:6px 0 0;overflow-wrap:anywhere}
.hist{display:grid;grid-template-columns:repeat(auto-fit,minmax(260px,1fr));gap:4px 20px;margin:0;padding:0;list-style:none;font-size:.85rem}
.hist li{display:flex;gap:8px;min-width:0}.hist code{font:.78rem var(--mono);color:var(--ok);flex:none}
.hist span{color:var(--mute);overflow:hidden;text-overflow:ellipsis;white-space:nowrap}
footer{font:.75rem var(--mono);color:var(--mute)}
</style>
<div class="wrap">
 <section class="top">
  <div><h1>Rogue Rigs · Crew Board</h1><p class="sub">Milestone M1 · tâches 013 à 028 · mis à jour le ${stamp}</p></div>
  <div class="gauge" role="img" aria-label="${done} sous-tâches sur ${queue.length} validées"><div class="lbl"><span>PROGRESSION DE LA FILE</span><span>${done} / ${queue.length}</span></div><div class="bar"><i style="width:${queue.length ? Math.round(100 * done / queue.length) : 0}%"></i></div></div>
 </section>
 <section><h2>Agents</h2><div class="bays" style="margin-top:12px">${['orchestrator', 'cpp', 'bp', 'level-design'].map(agentCard).join('')}</div></section>
 <section><h2>File d’exécution</h2><ol class="queue" style="margin-top:12px">${queue.map(row).join('')}</ol></section>
 <section><h2>Déjà livré (000 à 012)</h2><ul class="hist" style="margin-top:12px">${history.map(g => `<li><code>${g.id.split('-').slice(0, 2).join('-')}</code><span title="${esc(g.description)}">${esc(g.title)}</span></li>`).join('')}</ul></section>
 <footer>Source : vault/reference/tasks (statuts des nœuds). Page régénérée par l’Orchestrator à chaque changement de statut.</footer>
</div>`;
fs.writeFileSync(OUT, html);
console.log(OUT, done + '/' + queue.length, cur ? cur.id : '-');
