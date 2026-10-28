---
id: task-350-04-turret-capture
description: "-game capture of the new turret placed and firing, compared with the before frames, plus the turret tests on the final Blueprint; editor left open."
parent: task-350-turret-in-game
kind: task
status: in-progress
---
# Turret in game: after capture and tests

- goal: Show the owner the new turret in game and confirm the turret rules still hold with the final BP_Turret: -game frames of a placed turret whose head turns toward robots and fires from the canon, the same framing as the before frames of subtask 01; the turret tests rerun on the final Blueprints.
- agent: cpp
- depends_on: task-350-03-bp-turret-meshes
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-turret (vault/reference/tech/systems/combat/sys-turret.md), sys-tests-weapon-types (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-weapon-types.md); decisions: global `request` and `gdd_check` (vault/reference/tasks/task-350-turret-in-game.md); before frames `Saved/TurretInGame/before/` (subtask 01).
- method: the editor stays OPEN (owner instruction): run the -game capture and the automation tests as separate processes without closing it; if a run cannot start while the editor is open, stop and report (do not close it). At least two frames with the head pointing at two different robots; one frame with the flash. Contact sheet before / after.
- expected_output: after frames and a before/after contact sheet in `Saved/TurretInGame/after/` (not committed); test results; short report (what is seen, what is not verified).
- done_when: frames exist and their paths are reported so the Executor sends them to the owner; the frames show the base fixed and the head turned toward the target in two directions; `RogueRigs.Turret.*` and `Integration.PlaceTurretInputKillsRobot` pass on the final Blueprints; no repo change, or committed by path as `task-350-04-turret-capture: <summary>` if a capture helper changed; editor still open.
- editor_state: open
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-28T16:24:55Z
- waiting_for: none
- result: open

## Relations
- [[task-350-turret-in-game]] — parent global task.
