---
id: task-334-05-capture
description: "One -game run near an arena edge, a shot at two miner positions, showing the asteroids and their parallax shift, sheet for the owner."
parent: task-334-near-asteroids
kind: task
status: in-progress
---
# Asteroids: capture for the owner

- goal: the owner sees the asteroids in game and their shift against the space layers. Runs only if the owner kept A6; otherwise the Executor cancels this subtask.
- agent: cpp
- depends_on: task-334-04-asteroid-field
- inputs: the global `request`, `acceptance` and `gdd_check` (vault/reference/tasks/task-334-near-asteroids.md); the task-334-04 change summary (positions); the task-332-03 capture node for the method (vault/reference/tasks/task-332-space-background/task-332-03-capture.md); agent-cpp (vault/reference/agents/agent-cpp.md, capture_script and visual_check); bp-arena-grid (vault/reference/tech/blueprints/world/bp-arena-grid.md, map and spawn).
- expected_output: one `-game` run through `Tools/capture/capture.js` in L_ArenaM1, `rr.Stress.Peace 1`, miner near the arena edge with the most asteroids in view, one shot there and one after moving the miner about 600 uu along the edge; `Saved/Captures/<Name>/` with the PNGs and contact_sheet.png; a short description (asteroids seen, shift against the stars between shots, any asteroid hiding floor or units, any reading as walkable); numbers: mean sRGB luma of the asteroids, of the floor strip beside the edge and of the arena rock tops. No code or asset change; no other run.
- done_when: the PNGs exist and are described; at least one asteroid visible in each shot; no asteroid covers a floor cell or a unit (else reported as a defect, not fixed here); asteroid mean luma below the arena rock tops (else reported); the contact sheet path is given for the owner.
- editor_state: any
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: 2026-10-27T23:47:57Z
- waiting_for: none
- result: open

## Relations
- [[task-334-near-asteroids]] — parent global task.
