---
id: task-330-01-sharp-edges-material
description: "Find why the M_AimChevrons chevron edges look blurry and make them sharp with one-pixel AA, glow per the owner's answer."
parent: task-330-aim-chevrons-sharp
kind: task
status: todo
---
# Aim chevrons: sharp edges in the material

- goal: M_AimChevrons draws chevrons with crisp outlines (one screen pixel of AA), as the ground telegraph rings.
- agent: bp
- depends_on: task-329-02-fade-material (same asset; run only after task-329-aim-chevrons-fade is done)
- inputs: aim-indicator (vault/reference/gdd/art-audio/aim-indicator.md), sys-aim-indicator (vault/reference/tech/systems/combat/sys-aim-indicator.md: parameter contract, as updated by task-329-03), sys-ground-telegraph (vault/reference/tech/systems/combat/sys-ground-telegraph.md: edges smoothstep(-aa/2, aa/2, e) with aa = fwidth(distance), no glow), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), the global `request` of task-330-aim-chevrons-sharp (owner answer Q2 on the glow), image `Art/Source/aim-indicator/owner-test-2026-10-27.webp` (the blur the owner saw).
- expected_output: read the current Custom HLSL of `/Game/RogueRigs/FX/M_AimChevrons` and name the cause before changing anything (examples to check, not assumed: AA width in world uu instead of fwidth; the GlowWidth 10 / GlowOpacity 0.35 halo hugging a 12 uu line; a smooth falloff across the whole line). Fix: edges = one-pixel screen-space AA on the signed distance to the chevron outline (fwidth), line width stays 12 uu constant in world uu; glow per the owner's Q2 answer (default: removed, GlowOpacity default 0 or the glow term deleted with its parameters reported). Keep the task-329 per-chevron fade, colour, sizes, spacing and VisibleLength skip unchanged. Material only, no C++ change; if the cause is outside the material (decal resolution, post-process, C++), change nothing and report. Change summary for the doc subtask: cause, HLSL change, parameters removed or changed.
- done_when: material compiles without error and is saved; read-back of the HLSL shows fwidth-based edge AA and the glow as decided; per-chevron Decay^i and the other parameter defaults read back unchanged; cause written in the report; committed by explicit path. Look judged by the owner (no capture unless the global `request` records a yes to Q3).
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-330-aim-chevrons-sharp]] — parent global task.
