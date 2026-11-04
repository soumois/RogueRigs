---
id: task-426-02-radial-mask-capture
description: "Capture: check the screen circles darkening"
parent: task-426-screen-radial-darkener
kind: task
status: todo
---
# Darkener shader: circles around the screen centre, min at centre, max at corners: capture

- goal: One -game run on L_ArenaM1 at 1920x1080: a frame with Enable 1 and one with Enable 0 at the same spot, and one after the miner has moved, then check the list.
- agent: capture
- depends_on: task-426-01-radial-mask-material
- inputs: agent-capture (vault/reference/agents/agent-capture.md), vault/reference/tasks/task-426-screen-radial-darkener.md (acceptance)
- expected_output: Report per point seen / not seen / ambiguous with frame or crop paths in Saved/.
- done_when: C1 centre pixels equal between Enable 1 and Enable 0 (CenterDarkness 0). C2 corner pixels about 15 % of their Enable 0 value. C3 equal darkness along circles around the screen centre, linear fade. C4 the pattern stays fixed on screen when the miner moves. C5 void pixels at most halved. Each point reported; ambiguous points go to the owner.
- editor_state: Unreal editor closed (-game run).
- token_ceiling: 0.4 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-426-screen-radial-darkener]] — parent global task.
