---
id: task-329-02-fade-material
description: "Find why M_AimChevrons draws five equally bright chevrons despite Decay 0.78, and fix the distance fade in the material."
parent: task-329-aim-chevrons-fade
kind: task
status: in-progress
---
# Aim chevrons: distance fade in the material

- goal: M_AimChevrons draws chevron i (0 = nearest the miner) at Opacity x Decay^i, and that drop is what the decal actually shows on screen.
- agent: bp
- depends_on: task-329-01-owner-screenshot
- inputs: aim-indicator (vault/reference/gdd/art-audio/aim-indicator.md), sys-aim-indicator (vault/reference/tech/systems/combat/sys-aim-indicator.md: parameter contract; C++ pushes Decay 0.78 and Opacity 1, do not change C++), sys-ground-telegraph (vault/reference/tech/systems/combat/sys-ground-telegraph.md: working decal model, opacity = mask x Opacity, emissive = colour), mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md), ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md: bloom, a possible brightening cause to rule in or out), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), task-328-02-chevron-material (vault/reference/tasks/task-328-aim-chevrons/task-328-02-chevron-material.md: how it was built; the asset was saved again later in commit d2b9c630, read the current state, not the history), images `Art/Source/aim-indicator/mockup-chevrons.png` (target) and `Art/Source/aim-indicator/owner-test-2026-10-27.webp` (what the owner saw).
- expected_output: read the current Custom HLSL and the output pins of `/Game/RogueRigs/FX/M_AimChevrons` first and name the cause before changing anything (examples to check, not assumed: Decay^i not computed or not multiplied in; the faded mask feeds only one of Opacity / Emissive while the other stays at full strength; mask clamped or combined with max() so all chevrons reach 1; the glow term at full strength on every chevron). Fix it inside this material only: per-chevron opacity Opacity x Decay^i, one flat value per chevron (no gradient along a chevron), defaults unchanged (Decay 0.78, Opacity 1) unless the owner's Q1 answer in the global `request` says otherwise. Edges, line width and glow are not touched here ([[task-330-aim-chevrons-sharp]]). If the cause is outside the material (C++, post-process volume, level), change nothing and report it. Change summary for the doc subtask: cause, what changed in the HLSL or graph, parameter list unchanged or not.
- done_when: material compiles without error and is saved; read-back of the HLSL shows chevron i weighted by Decay^i on the value the decal blends (state which pin); parameter names and defaults read back unchanged; the cause is written in the report; committed by explicit path. Look on screen judged by the owner (no capture unless the global `request` records a yes to Q3).
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: 2026-10-27T16:36:04Z
- waiting_for: none
- result: open

## Relations
- [[task-329-aim-chevrons-fade]] — parent global task.
