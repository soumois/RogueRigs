---
id: task-197-07-barrel-fx-visual-check
description: "Visual check in a -game run: screenshots of one barrel fuse and explosion showing the shake, blink, burst and shockwave ring."
parent: task-197-barrel-explosion-feedback
kind: task
status: done
---
# Barrel FX visual check

- goal: owner request (2026-10-16): check visually that the barrel effects start in game. One `-game` run of the packaged-free editor build with `rr.Debug.Shots` (and `rr.Debug.Delayed` to chain steps): light one barrel fuse in view of the camera with existing routes only (for example `rr.Debug.SpawnNear` of a kamikaze next to a barrel, or a drone dig, or `ke` on the grid), and capture frames during the fuse (shake, blink, growing danger circle) and during the explosion (burst, shockwave ring, sphere). No new cheat or debug command committed (owner 2026-10-16); if no existing route can light a barrel in view, stop and report the options instead.
- agent: cpp
- depends_on: task-197-06-barrel-explosion-fx-blueprint
- inputs: barrel (vault/reference/gdd/terrain/barrel.md), sys-grid-barrels (vault/reference/tech/systems/world/sys-grid/sys-grid-barrels.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md); the doc nodes written by subtasks 3 to 6.
- expected_output: PNG frames in `Saved/Screenshots/` (not committed), their paths, the command line used, the log lines of the fuse and the explosion.
- done_when: frames exist covering the fuse and the explosion; the Orchestrator has looked at them and written what is seen and what is not in the result; nothing committed except the task node by the Orchestrator.
- editor_state: closed
- token_ceiling: open
- model: opus
- tokens: input 106, cache write 119171, cache read 3575478, output 28667
- runs: 1
- milestone: M1
- result: Barrel chain test expects BP_ExplosionFX_Barrel (0.6 s visual, 1.5 s actor), 15/15 tests; -game frames: blink and danger circle during the fuse, sphere, burst and faint ring at the explosion; green decal square on first decal frame (probable shader compile), ring hidden by the sphere, burst small; commits 62c44756, 14e493f3

## Relations
- [[task-197-barrel-explosion-feedback]] — parent global task.
