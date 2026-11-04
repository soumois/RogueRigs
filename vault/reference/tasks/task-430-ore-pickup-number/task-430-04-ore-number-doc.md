---
id: task-430-04-ore-number-doc
description: "Doc: document the ore number in the damage number, Niagara contract, atlas and pickup docs"
parent: task-430-ore-pickup-number
kind: task
status: done
---
# Ore pickup number on the miner: doc

- goal: update the tech docs with the ore number path, glyph 11, the atlas cell and the pickup call site.
- agent: doc
- depends_on: task-430-01-ore-icon-glyph, task-430-02-ore-number-cpp, task-430-03-ore-number-capture
- inputs: sys-damage-numbers (vault/reference/tech/systems/combat/sys-damage-numbers.md), sys-damage-numbers-niagara (vault/reference/tech/systems/combat/sys-damage-numbers-niagara.md), bp-damage-numbers (vault/reference/tech/blueprints/fx/bp-damage-numbers.md), sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), agent-doc (vault/reference/agents/agent-doc.md)
- expected_output: updated `sys-damage-numbers`, `sys-damage-numbers-niagara`, `bp-damage-numbers` and `sys-pickups` nodes; lint clean; commit `task-430-04-ore-number-doc: ...`.
- done_when: the four nodes describe AddOre (or its equivalent), `OreGlyph` 11, `OreColor`, the icon layout and spacing, cell 11 of T_DamageDigitsOutline and its source files, the Ore pickup call site, and the capture result; `node vault/scripts/lint.js --write` clean; committed by path.
- editor_state: closed
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 18, cache write 49403, cache read 316044, output 6611
- usage: turns 9, context start 11k peak 54k, resumes 0, 1 min, units 0.13 M
- runs: 1
- milestone: M2
- started: 2026-11-04T21:22:38Z
- waiting_for: none
- result: Four nodes updated (sys-damage-numbers, -niagara, bp-damage-numbers, sys-pickups); gdd and lifecycle/tests nodes untouched; commit 637656d1

## Relations
- [[task-430-ore-pickup-number]] — parent global task.
