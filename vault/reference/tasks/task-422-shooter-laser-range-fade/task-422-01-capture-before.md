---
id: task-422-01-capture-before
description: "Capture: before frames of the three cases on the current build"
parent: task-422-shooter-laser-range-fade
kind: task
status: todo
---
# Shooter targeting laser: maximum range and fade-out: capture

- goal: Before any code change, capture the current Shooter laser in the three staged cases and report what each frame shows.
- agent: capture
- depends_on: none
- inputs: task-422-shooter-laser-range-fade (vault/reference/tasks/task-422-shooter-laser-range-fade.md), enemy-shooter (vault/reference/gdd/enemies/enemy-shooter.md), sys-enemies-shooter (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-shooter.md), tool-capture (vault/reference/tech/pipelines/tool-capture.md), Staging, same for before and after (map L_ArenaM1, 1920x1080, rr.Stress.Peace 1 or rr.Spawner.Enabled 0 so only the staged Shooter fires; BP_Shooter through rr.Debug.SpawnNear; BugItGo then Walk to place the miner; rr.Debug.Delayed + rr.Debug.Shots during an Aim, i.e. while the red laser shows, 0.6 s WindUp): S1 open terrain, no block on the line: Shooter spawned about 1300 uu from the miner, it walks in and aims at about 956 uu (range edge); S2 a rock block on the aim line beyond the miner, within 960 uu of the Shooter (miner between the Shooter and the block, line of sight to the miner clear, the Shooter only aims with line of sight, enemy-shooter); S3 Shooter spawned about 400 uu from the miner in open terrain.
- expected_output: Frames and a contact sheet copied to Saved/Scratch/task-422-shooter-laser/before/ (temporary, git-ignored, never committed), report per point.
- done_when: B1 frame of S1 with the laser visible during an Aim: where it ends (at the miner or elsewhere) and whether it fades. B2 frame of S2: where the laser ends relative to the miner and the rock block. B3 frame of S3: the laser from the cannon tip to the miner. Each point seen, not seen or ambiguous with its frame path under Saved/Scratch/task-422-shooter-laser/before/; staging (commands, distances, positions) written in the report so the after run repeats it.
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
