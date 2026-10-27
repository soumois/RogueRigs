---
id: task-330-02-docs
description: "Update sys-aim-indicator after the sharp-edge fix: edge anti-aliasing, glow parameters kept or removed."
parent: task-330-aim-chevrons-sharp
kind: task
status: in-progress
---
# Aim chevrons sharp edges: docs

- goal: sys-aim-indicator describes the edges and glow of M_AimChevrons as they are after task-330-01.
- agent: doc
- depends_on: task-330-01-sharp-edges-material
- inputs: sys-aim-indicator (vault/reference/tech/systems/combat/sys-aim-indicator.md, to update), aim-indicator (vault/reference/gdd/art-audio/aim-indicator.md, decided values, read only), the global `request` and `gdd_check` of task-330-aim-chevrons-sharp, the task-330-01 change summary and commit.
- expected_output: in sys-aim-indicator: the material contract states the edge AA (one pixel, fwidth) and the cause fixed; the `GlowWidth` / `GlowOpacity` line matches the material (new default or removed). No other node.
- done_when: sys-aim-indicator updated, under ~400 words; lint clean; committed by explicit path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: 2026-10-27T16:39:41Z
- waiting_for: none
- result: open

## Relations
- [[task-330-aim-chevrons-sharp]] — parent global task.
