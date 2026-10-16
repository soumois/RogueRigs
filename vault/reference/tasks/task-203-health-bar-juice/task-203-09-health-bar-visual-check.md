---
id: task-203-09-health-bar-visual-check
description: "-game captures of the juicy health bar at full health and during a hit, compared with the 280 x 28 bar from before task-203."
parent: task-203-health-bar-juice
kind: task
status: done
---
# Health bar visual check in game

- goal: Captures with the HUD show the health bar at its old size and place (280 x 28 at 20,232 at 1920 x 1080, text "Current / Max" below it) and the juice effects during a hit.
- agent: cpp
- depends_on: task-203-08-health-bar-size
- inputs: wbp-health-status (vault/reference/tech/blueprints/ui/wbp-health-status.md), sys-hud-health-bar (vault/reference/tech/systems/ui/sys-hud-health-bar.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), Source/RogueRigs/Debug/RRVisualCheckCommands.cpp
- spec: One `-game -windowed` run (ResX 1920, ResY 1080) with `rr.Debug.Shots ... ShowUI 1`: one capture at full health, then `rr.Debug.DamageMiner` (a hit of about 30 percent, through `rr.Debug.Delayed`) with captures at about 0.02, 0.1, 0.3 and 0.8 s after it, so the flash, chunk, shake or punch and the final colour can be seen; `rr.Stress.Peace 1` so no enemy changes the health. Open each PNG and describe the bar: pixel size and position, fill colour, chunk, offset or height change. Compare with the owner's reference (before task-203: red bar 280 x 28 at 20,232). No code change unless a debug command is missing; then say so and stop.
- expected_output: PNGs in Saved/Screenshots/ (not committed), their paths and a description per capture; result written to wbp-health-status verified line; one commit of the doc.
- done_when: the full-health capture shows the bar at about 280 x 28 at 20,232 (at 1920 x 1080); the hit captures are described; paths reported for the Orchestrator to send to the owner; doc committed by path.
- editor_state: closed
- token_ceiling: open
- model: opus
- tokens: input 26, cache write 53386, cache read 394659, output 1759
- runs: 1
- milestone: M1
- result: -game 1920 x 1080, L_ArenaM1, hit of 54 (max health 100): full health bar 280 x 28 at 20,232, green #56B35E, text below; 2 frames after the hit: height 35 (punch), fill flashed near-white, off-white chunk; 0.34 s: height 28, yellow fill to 46 percent, chunk held; 0.8 s: chunk shrinking, empty part light grey #BABABA. 0.1 s capture lost (screenshot freeze). Not seen: horizontal shake, red below 20 percent. PNGs Saved/Screenshots/WindowsEditor/t203_09_*. Commit 34e89ebe.

## Relations
- [[task-203-health-bar-juice]] — parent global task.
