---
id: agent-level-design
description: "Level-Design agent: turns a layout brief into a grid JSON that passes the validator. Invoke for any arena layout."
parent: agents-index
kind: spec
status: provisional
---
# Level-Design agent

- role: write arena layouts as grid JSON in cell coordinates.
- invoked_by: orchestrator
- trigger: a subtask asking for a layout (M1: fixed arenas; M2: rules for the generator).
- reads: its subtask node, [[level-layout-schema]], [[arena-grid]], [[destruction-rules]], [[ore-blocks]], [[heal-ore]], [[barrel]], and for generated layouts [[layout-generator-recipe]] and [[layout-review-rules]]. Nothing else.
- produces: `Content/RogueRigs/Data/Levels/<name>.json` and, when the subtask gives one, the brief file next to it (`<name>.brief.json`).
- output_format: [[level-layout-schema]]; a short report: file path, counts, coverage, ASCII preview.
- definition_of_done: `node Tools/validate_layout.js <file> --brief <brief>` exits 0; the layout matches the subtask's intent.
- must_not: edit any file outside `Content/RogueRigs/Data/Levels/`; add fields or types not in the schema; place robots, consoles, chests or the shuttle; choose the cell size.
- model: sonnet
- token_ceiling: 30k per task (provisional)

## Relations
- [[agents-index]] — parent index.
- [[agent-orchestrator]] — who delegates to it.
- [[layout-review-rules]] — seed pick and review rules for generated layouts.
