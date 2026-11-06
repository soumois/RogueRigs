---
id: task-478-01-wall-match-loop
description: "Bp: Loop: remove the dark lines and visible cuts, place the emissive on the top edge, capture, compare with image 3, rework; at most 4 rounds."
parent: task-478-wall-lines-emissive-edge
kind: task
status: in-progress
---
# Walls without black lines, emissive on the top edge: bp

- goal: Loop: remove the dark lines and visible cuts, place the emissive on the top edge, capture, compare with image 3, rework; at most 4 rounds.
- agent: bp
- depends_on: none
- inputs: task-471-wall-look-reference (vault/reference/tasks/task-471-wall-look-reference.md), visual-style (vault/reference/gdd/art-audio/visual-style.md), rock-look-reference (vault/reference/gdd/art-audio/rock-look-reference.md), Art/Source/cartoon-look/reference/owner-2026-11-06-wall-black-lines.png, Art/Source/cartoon-look/reference/owner-2026-11-06-wall-section-target.png
- expected_output: see done_when
- done_when: A2 to A7 and A9 pass on the loop sheets, or 4 rounds are done, or the owner said "je valide"; each round written in the result; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-06T20:00:00Z
- waiting_for: none
- result: open

## Relations
- [[task-478-wall-lines-emissive-edge]] — parent global task.
