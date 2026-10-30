---
id: task-392-04-intermission-duskite-label
description: "Bp: WBP_Intermission: design-time "Total Ore:" text becomes "Total Duskite:""
parent: task-392-duskite-on-screen
kind: task
status: in-progress
---
# On-screen ore word: Duskite: bp

- goal: In WBP_Intermission, the design-time text "Total Ore:" of the Total Ore label -> "Total Duskite:" (the runtime text comes from screen_texts.json, task-392-01); variable and widget names unchanged; nothing else changes.
- agent: bp
- depends_on: task-392-01-screen-texts-duskite
- inputs: agent-bp (vault/reference/agents/agent-bp.md), wbp-intermission (vault/reference/tech/blueprints/ui/wbp-intermission.md), task-392-01-screen-texts-duskite (vault/reference/tasks/task-392-duskite-on-screen/task-392-01-screen-texts-duskite.md) result, global request and gdd_check: vault/reference/tasks/task-392-duskite-on-screen.md
- expected_output: WBP_Intermission saved, compiled with warnings as errors; commit `task-392-04-intermission-duskite-label: ...`; change summary.
- done_when: Label text read back "Total Duskite:", no other property changed; compiled with warnings as errors, no error; saved; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-30T22:01:14Z
- waiting_for: none
- result: open

## Relations
- [[task-392-duskite-on-screen]] — parent global task.
