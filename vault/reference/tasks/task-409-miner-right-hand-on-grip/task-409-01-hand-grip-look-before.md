---
id: task-409-01-hand-grip-look-before
description: "Capture: look before: current V2 with the rifle offset, two close views (Q3)"
parent: task-409-miner-right-hand-on-grip
kind: task
status: todo
---
# Miner right hand on the rifle pistol grip, rifle kept where the owner placed it: capture

- goal: Frames of the current look (A_Miner_UpperRifle_V2, RifleMesh offset (9.898091, -1.129037, -13.0)) from the two close views, before anything changes.
- agent: capture
- depends_on: none
- inputs: agent-capture (vault/reference/agents/agent-capture.md), tool-capture (vault/reference/tech/pipelines/tool-capture.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), task-408-04-upper-rifle-variants-capture (vault/reference/tasks/task-408-miner-right-arm-in-animation/task-408-04-upper-rifle-variants-capture.md), Commands: `rr.Stress.Peace 1`, Spawner off and KillRobots as in task-408-04, `rr.Debug.Camera <Pitch> <ArmLength> [Yaw]`, `rr.Debug.MinerAnimFreeze 1`, `rr.Debug.Shots`, `rr.Debug.Delayed`, Owner screenshot of the defect (Temp, may be gone): C:\Users\soumo\AppData\Local\Temp\claude\C--Users-soumo-Documents-Unreal-Projects-RogueRigs\86f0dfd7-050d-4e04-9a48-dec2c55949c6\images\1.webp
- expected_output: Frames `Saved/Captures/HandGrip409/before_side.png` and `before_front.png`, the exact camera commands used (pitch, arm length, yaw) written in the report so the after capture repeats them, report with paths.
- done_when: right side close view and three-quarter front close view (as task-408-04), miner standing, weapons stopped, animation frozen, at most 3 launches. Checklist: C1 the right hand and the rifle pistol grip both visible in each frame; C2 the gap between hand and grip visible as in the owner screenshot. Each point seen, not seen or ambiguous with a path; camera commands recorded in result.
- editor_state: closed
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
- [[task-409-miner-right-hand-on-grip]] — parent global task.
