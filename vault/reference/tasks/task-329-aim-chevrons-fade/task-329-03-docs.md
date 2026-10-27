---
id: task-329-03-docs
description: "Update sys-aim-indicator after the fade fix: cause, material change, and the fixed length 416 uu now decided by the owner."
parent: task-329-aim-chevrons-fade
kind: task
status: done
---
# Aim chevrons fade: docs

- goal: sys-aim-indicator describes the material as it is after task-329-02 and records the fixed length as an owner value.
- agent: doc
- depends_on: task-329-02-fade-material
- inputs: sys-aim-indicator (vault/reference/tech/systems/combat/sys-aim-indicator.md, to update), aim-indicator (vault/reference/gdd/art-audio/aim-indicator.md, decided values, read only), the global `request` and `gdd_check` of task-329-aim-chevrons-fade, the task-329-02 change summary and commit.
- expected_output: in sys-aim-indicator: the material contract line states how the per-chevron opacity Decay^i is applied (which pin) and the cause fixed by task-329-02; the "fixed length" line says 416 uu is the owner's value (owner 2026-10-27, "La longueur est bonne"), no longer only a derivation; the "not verified on screen" gotcha updated to what the owner saw. No other node.
- done_when: sys-aim-indicator updated, under ~400 words; lint clean; committed by explicit path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 12, cache write 20446, cache read 77896, output 3703
- usage: turns 6, context start 10k peak 20k, resumes 0, 1 min, units 0.05 M
- runs: 1
- milestone: M3
- started: 2026-10-27T16:37:06Z
- waiting_for: none
- result: sys-aim-indicator updated (commit bba32b9c): Decay^i on MP_Opacity, EmissiveGain 0.5 provisional, probable cause, 416 uu owner value. Node 655 words, over the ~400 target (was ~620 before, trimmed from 730); left to owner decision.

## Relations
- [[task-329-aim-chevrons-fade]] — parent global task.
