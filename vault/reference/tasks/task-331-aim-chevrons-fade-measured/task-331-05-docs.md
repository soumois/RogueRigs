---
id: task-331-05-docs
description: "Update sys-aim-indicator with the measured cause of the weak chevron fade, the material fix and the EmissiveGain decision."
parent: task-331-aim-chevrons-fade-measured
kind: task
status: in-progress
---
# Aim chevrons measured fade: docs

- goal: sys-aim-indicator describes M_AimChevrons as it is after task-331-03, with the measured cause replacing the unmeasured task-329 one.
- agent: doc
- depends_on: task-331-03-fade-material, task-331-04-measure-after
- inputs: sys-aim-indicator (vault/reference/tech/systems/combat/sys-aim-indicator.md, to update); aim-indicator (vault/reference/gdd/art-audio/aim-indicator.md, decided values, read only); the global `request` and `gdd_check` of task-331-aim-chevrons-fade-measured (vault/reference/tasks/task-331-aim-chevrons-fade-measured.md); the task-331-03 change summary and commit; the task-331-04 after table.
- expected_output: in sys-aim-indicator: the material contract line states the measured cause and the fix (which pin, how the per-chevron weight is applied), the engine fact that decal emissive is drawn `SrcAlpha, One` (scaled by opacity, floor not darkened); the "probable, not measured" sentence of task-329 replaced; `EmissiveGain` default and status per the Q2 answer; the gotcha updated with the measured after ratios. Keep the node near ~400 words (it is ~655): trim history sentences rather than add. No other node.
- done_when: sys-aim-indicator updated and not longer than before; lint clean; committed by explicit path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: 2026-10-27T20:42:51Z
- waiting_for: none
- result: open

## Relations
- [[task-331-aim-chevrons-fade-measured]] — parent global task.
