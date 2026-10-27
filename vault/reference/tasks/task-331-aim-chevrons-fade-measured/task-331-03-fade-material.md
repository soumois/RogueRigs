---
id: task-331-03-fade-material
description: "Name the measured cause of the weak chevron fade and fix it in M_AimChevrons, setting EmissiveGain as the owner decided."
parent: task-331-aim-chevrons-fade-measured
kind: task
status: done
---
# Aim chevrons: fix the fade from measurements

- goal: M_AimChevrons draws a fade whose rendered contrast follows about 0.78^i per chevron, with a cause named from measured values, not from reading the HLSL alone.
- agent: bp
- depends_on: task-331-02-measure-before
- inputs: the global `request` and `gdd_check` of task-331-aim-chevrons-fade-measured (vault/reference/tasks/task-331-aim-chevrons-fade-measured.md: engine decal blend fact, owner answers Q2 and Q3); task-331-02 result (contrast table); sys-aim-indicator (vault/reference/tech/systems/combat/sys-aim-indicator.md: parameter contract; do not change C++); aim-indicator (vault/reference/gdd/art-audio/aim-indicator.md); ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md: exposure, bloom); mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md: darkening, no compensation); sys-ground-telegraph (vault/reference/tech/systems/combat/sys-ground-telegraph.md); bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md); unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md); task-329-02-fade-material (vault/reference/tasks/task-329-aim-chevrons-fade/task-329-02-fade-material.md: what was concluded, unmeasured).
- expected_output: before changing anything: (1) in PIE, read back from the live dynamic instance of the miner's `AimIndicator` the scalar values actually used (Decay, Opacity, EmissiveGain, ChevronCount) and stop PIE; (2) read the current Custom HLSL and pins; (3) evaluate the HLSL per chevron centre (a Node port in the scratchpad with the read-back values) and give the opacity value of each chevron; (4) compare with the subtask 02 ratios and name the cause. Then fix in this material only: per chevron, one flat value, target ratio 0.78^i on the rendered contrast (Q3 answer); do not square the fade by also weighting the emissive (the engine already scales emissive by opacity, see `gdd_check`). EmissiveGain per the Q2 answer. Edges, line width, length unchanged. If the cause is outside the material, change nothing and report. Change summary for the doc subtask: measured values, cause, HLSL or graph change, parameter list and defaults.
- done_when: material compiles and is saved; read-back values and the per-chevron opacity table are in the report; the cause is tied to those numbers; EmissiveGain read back equals the Q2 decision; parameter names unchanged; committed by explicit path. Rendered result is checked by subtask 04.
- editor_state: open
- token_ceiling: 1.0 M units
- model: sonnet
- tokens: input 64, cache write 113700, cache read 2329848, output 27371
- usage: turns 32, context start 10k peak 114k, resumes 0, 5 min, units 0.51 M
- runs: 1
- milestone: M3
- started: 2026-10-27T20:33:50Z
- waiting_for: none
- result: Material already drew 0.78^i opacity (1/0.78/0.608/0.475/0.370, PIE read-back); weak fade is post-process tonemap+sRGB (fitted model). Fix: weight pow(Decay, FadeGain*i^FadeExp), FadeGain 5.15, FadeExp 0.55, EmissiveGain 1.0. Commit 5e935801. Owner kept it as is (2026-10-27).

## Relations
- [[task-331-aim-chevrons-fade-measured]] — parent global task.
