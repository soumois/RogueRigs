---
id: task-422-03-capture-after
description: "Capture: after frames of the three cases, checklist against the before frames"
parent: task-422-shooter-laser-range-fade
kind: task
status: todo
---
# Shooter targeting laser: maximum range and fade-out: capture

- goal: Repeat the before staging on the new build and check the laser against the checklist, next to the before frames.
- agent: capture
- depends_on: task-422-02-laser-range-fade
- inputs: task-422-shooter-laser-range-fade (vault/reference/tasks/task-422-shooter-laser-range-fade.md), enemy-shooter (vault/reference/gdd/enemies/enemy-shooter.md), tool-capture (vault/reference/tech/pipelines/tool-capture.md), Saved/Scratch/task-422-shooter-laser/before/ (look-before frames and the staging written in the task-422-01 report), Staging, same for before and after (map L_ArenaM1, 1920x1080, rr.Stress.Peace 1 or rr.Spawner.Enabled 0 so only the staged Shooter fires; BP_Shooter through rr.Debug.SpawnNear; BugItGo then Walk to place the miner; rr.Debug.Delayed + rr.Debug.Shots during an Aim, i.e. while the red laser shows, 0.6 s WindUp): S1 open terrain, no block on the line: Shooter spawned about 1300 uu from the miner, it walks in and aims at about 956 uu (range edge); S2 a rock block on the aim line beyond the miner, within 960 uu of the Shooter (miner between the Shooter and the block, line of sight to the miner clear, the Shooter only aims with line of sight, enemy-shooter); S3 Shooter spawned about 400 uu from the miner in open terrain.
- expected_output: Frames, crops and a before/after sheet in Saved/Scratch/task-422-shooter-laser/after/ (temporary, never committed), report per point C1 to C5.
- done_when: C1 S1: the red laser runs past the miner to about 960 uu from the muzzle and its end fades to invisible, no hard cut. C2 S2: the laser ends at the face of the rock block and fades to zero there, nothing beyond it or on the block top. C3 S3: the laser is fully opaque red from the cannon tip to the miner, the fade only far past him. C4 the laser is red and about as thin as in the before frames. C5 before/after sheet of S1, S2, S3 side by side under Saved/Scratch/task-422-shooter-laser/after/. Each point seen, not seen or ambiguous with its frame path.
- editor_state: Unreal editor closed (-game run)
- token_ceiling: 0.4 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-422-shooter-laser-range-fade]] — parent global task.
