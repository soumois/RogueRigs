---
id: task-423-02-capture-before
description: "Capture: before frames of the current aim laser for the three owner cases"
parent: task-423-miner-aim-laser-range-fade
kind: task
status: done
---
# Miner aim laser: visible maximum range and fade-out at its end: capture

- goal: Take the before frames on the current build (laser unchanged), one per owner case, at 1920x1080, and record the exact setup (map, miner position, rr.Debug.Aim yaw, camera, spawn commands) so the after frames repeat it. Frames are temporary: Saved/Scratch/task-423/before/, never Art/Source, never committed.
- agent: capture
- depends_on: task-423-01-debug-aim-command
- inputs: task-423-miner-aim-laser-range-fade (vault/reference/tasks/task-423-miner-aim-laser-range-fade.md), aim-indicator (vault/reference/gdd/art-audio/aim-indicator.md), tool-capture (vault/reference/tech/pipelines/tool-capture.md), agent-capture (vault/reference/agents/agent-capture.md), `Art/Source/aim-indicator/laser-reference-2026-11-02/sheet.png` (reference look)
- expected_output: Three frames (plus crops if useful) in Saved/Scratch/task-423/before/, a setup note Saved/Scratch/task-423/setup.txt, and a report per point B1 to B3.
- done_when: B1 case 1: miner aimed at open floor with no block or robot within 1200 uu on the aim line; the frame shows the laser from the Muzzle; report where it visibly ends (uu estimate) and whether a fade is visible. B2 case 2: a rock block on the aim line about 350 uu from the miner, no nearer robot; report whether the laser ends at the block and whether a fade is visible there. B3 case 3: a living robot on the aim line about 350 uu away, no nearer block; report whether the laser ends at the robot and whether a fade is visible there. Each point reported seen, not seen or ambiguous with its frame path; setup.txt written.
- editor_state: Unreal editor closed (-game run)
- token_ceiling: 0.4 M units
- model: sonnet
- tokens: input 38, cache write 58579, cache read 747375, output 16631
- usage: turns 19, context start 13k peak 59k, resumes 0, 6 min, units 0.23 M
- runs: 1
- milestone: M3
- started: 2026-11-03T02:14:25Z
- waiting_for: none
- result: Before frames taken: B1 seen (fade ambiguous), B2 seen, B3 seen; setup in Saved/Scratch/task-423/setup.txt

## Relations
- [[task-423-miner-aim-laser-range-fade]] — parent global task.
