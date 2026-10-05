// node vault/scripts/record-cost.js <task-NNN-SS> <runs>
// Fills model, tokens and runs in the subtask node from agent-usage.js and adds or updates its row in cost-log.
const fs = require('fs'), { execSync } = require('child_process');
const R = require('path').resolve(__dirname, '..', '..').split(require('path').sep).join('/') + '/';
process.chdir(R);
const [id, runs] = process.argv.slice(2);
const data = JSON.parse(execSync('node vault/scripts/agent-usage.js --json').toString());
const r = data.subtasks.find(x => x.id === id);
if (!r) throw new Error('no usage for ' + id);
const model = r.models.split('+').filter(m => m && m !== '<synthetic>').map(m => m.replace('claude-', '').replace(/-\d.*$/, '')).join('+');
const T = R + 'vault/reference/tasks/';
let file;
for (const d of fs.readdirSync(T, { withFileTypes: true })) if (d.isDirectory()) for (const f of fs.readdirSync(T + d.name)) if (f.startsWith(id + '-')) file = T + d.name + '/' + f;
let s = fs.readFileSync(file, 'utf8');
s = s.replace(/^- model: .*$/m, '- model: ' + model).replace(/^- tokens: .*$/m, `- tokens: input ${r.input}, cache write ${r.cacheWrite}, cache read ${r.cacheRead}, output ${r.output}`).replace(/^- runs: .*$/m, '- runs: ' + runs);
fs.writeFileSync(file, s);
const k = n => Math.round(n / 1000) + 'k';
const nodeId = file.split('/').pop().replace('.md', '');
const C = R + 'vault/reference/production/cost-log.md';
let c = fs.readFileSync(C, 'utf8');
const row = `| [[${nodeId}]] | ${model} | ${runs} | ${k(r.input)} | ${k(r.cacheWrite)} | ${k(r.cacheRead)} | ${k(r.output)} |`;
const rowRe = new RegExp('^\\| \\[\\[' + nodeId + '\\]\\].*$', 'm');
if (rowRe.test(c)) c = c.replace(rowRe, row);
else c = c.replace(/(\n## Per subtask\n[\s\S]*?)(\n\n## Totals)/, (m, a, b) => a + '\n' + row + b);
fs.writeFileSync(C, c);
console.log(id, model, k(r.cacheWrite), k(r.cacheRead), k(r.output));
