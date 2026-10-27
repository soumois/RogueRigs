---
id: task-326-01-units-ceiling-script
description: "record-cost.js alerts when a subtask's weighted units exceed a token_ceiling written in M units; legacy integer ceilings give no alert."
parent: task-326-units-ceiling
kind: task
status: done
---
# Ceiling alert on units

- goal: apply Q5, Q7 and Q8 of [[task-326-units-ceiling]] `gdd_check` as answered by the owner (defaults if "ok"). `record-cost.js` reads `- token_ceiling: <x> M units` (decimal allowed), compares it with the subtask's `units` (same figure as the `usage` line) times `alerts.ceilingRatio`, and prints `ALERT <id>: units <u> M is over token_ceiling <x> M (<ratio>x): ...`; a ceiling without `M units` (legacy integer or `k`) or `open` gives no ceiling alert. Update the header comments of `record-cost.js` and `task-create.js` (ceiling format) and the `note` of `cost-rates.json`. No other behaviour change; edit no vault node: return a change summary (files, commit hash, new ALERT text, format accepted) for the doc subtask.
- agent: cpp
- depends_on: task-325-01-doc-agent
- inputs: record-cost.js (vault/scripts/record-cost.js), task-create.js (vault/scripts/task-create.js), cost-rates.json (vault/scripts/cost-rates.json), cost-log (vault/reference/production/cost-log.md)
- expected_output: the three edited script files; a change summary.
- done_when: on a throwaway copy of a done subtask node and of cost-log (never a live node, reverted after; or a dry-run path if the script offers one): `1.2 M units` with units 0.69 gives no ceiling alert, `0.5 M units` gives one with the ratio; `80000` and `open` give none; the other alerts are unchanged; lint clean; committed by explicit path.
- editor_state: any
- token_ceiling: 0.5 M units
- model: opus
- tokens: input 18, cache write 25658, cache read 177394, output 7410
- usage: turns 9, context start 10k peak 30k, resumes 0, 1 min, units 0.09 M
- runs: 1
- milestone: M2
- started: 2026-10-27T14:20:40Z
- waiting_for: none
- result: record-cost.js compares units to an '<x> M units' ceiling, legacy/open give no alert; tests on throwaway copies pass; commit 93c7dc5d

## Relations
- [[task-326-units-ceiling]] — parent global task.
