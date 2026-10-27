---
id: task-333-01-nebula-tune
description: "Retune the parameters of MI_Space_Nebula so the nebula is clearly visible at normal brightness, without changing the M_SpaceBackground graph."
parent: task-333-nebula-visible
kind: task
status: done
---
# Nebula: retune MI_Space_Nebula

- goal: MI_Space_Nebula gives a nebula peak in the N1 band of the global (default about 20 to 35 sRGB luma near an arena edge, below the floor strip beside it), with soft clouds per N3.
- agent: bp
- depends_on: none
- inputs: the global `request` and `gdd_check` with the owner answers N1 to N6 (vault/reference/tasks/task-333-nebula-visible.md); bp-space-background-material (vault/reference/tech/blueprints/world/bp-space-background-material.md, parameter table and the task-332-03 finding: Intensity 0.06 gives a peak of about 1.4 luma); mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md, the about 0.08 gain far from the miner); ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md, fixed exposure and bloom); color-code (vault/reference/gdd/art-audio/color-code.md, void background palette); bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md).
- expected_output: MI_Space_Nebula in /Game/RogueRigs/Environment/Space/ with new values: Intensity first set from the measured ratio (sRGB 1.4 to the band centre, converted to linear; state the arithmetic), plus Density, Contrast or Scale only if N3 needs it; ColorA/B/C unchanged; M_SpaceBackground, MI_Space_Stars and MI_Space_Dust unchanged. Change summary for the doc subtask: old and new values, the arithmetic, commit hash. No vault doc node.
- done_when: the instance compiles with no log error and is saved; its parameters read back; M_SpaceBackground, MI_Space_Stars and MI_Space_Dust show no change in `git status`; committed by explicit path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: sonnet
- tokens: input 32, cache write 29987, cache read 320472, output 6071
- usage: turns 16, context start 9k peak 30k, resumes 0, 1 min, units 0.10 M
- runs: 1
- milestone: M3
- started: 2026-10-27T21:59:11Z
- waiting_for: none
- result: Intensity 0.06 -> 1.6 (sRGB 1.4 -> 27.5 target, linear ratio 26.5), saved, commit 975425cf; render unverified until 02

## Relations
- [[task-333-nebula-visible]] — parent global task.
