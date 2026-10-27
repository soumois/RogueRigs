---
id: task-328-02-chevron-material
description: "Build M_AimChevrons, the deferred decal material drawing a row of white-gold chevrons with distance fade, per the parameter contract."
parent: task-328-aim-chevrons
kind: task
status: todo
---
# Aim chevrons: decal material

- goal: one material asset that draws the chevron row of [[aim-indicator]] when used on a decal; C++ only drives its parameters.
- agent: bp
- depends_on: task-328-01-reference-images
- inputs: aim-indicator (vault/reference/gdd/art-audio/aim-indicator.md), sys-ground-telegraph (vault/reference/tech/systems/combat/sys-ground-telegraph.md: M_GroundTelegraph is the model to copy for domain, blend, Custom HLSL masks and one-pixel AA), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), reference `Art/Source/aim-indicator/mockup-chevrons.png`.
- expected_output: `/Game/RogueRigs/FX/M_AimChevrons`: domain Deferred Decal, Translucent, Unlit, decal response None (as M_GroundTelegraph). Chevrons laid along the decal's long axis, pointing away from the miner end. Scalar and vector parameters, names exact, defaults = the provisional values of aim-indicator (ChevronCount 5, FirstOffset 120 uu, Spacing 74 uu, OpeningDeg 60, ArmLength 40 uu, LineWidth 12 uu, Decay 0.78, Color linear (1.0, 0.84, 0.40), Opacity 1, HalfLength uu of the box, VisibleLength uu: a chevron whose far tip is beyond it is not drawn, GlowWidth and GlowOpacity for the soft halo). No brightness compensation for the player light mask (owner 2026-10-27, "Laisser s'éteindre"): plain emissive colour, no extra gain parameter. Sharp lines with one-pixel AA, sizes constant in world uu. A change summary for the doc subtask: asset path, parameter names, defaults, which UV axis is "forward" and where the miner end is (C++ needs it). No Blueprint and no level change.
- done_when: the material compiles without error and is saved; its parameters list matches the names above (read back); committed by explicit path.
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
- [[task-328-aim-chevrons]] — parent global task.
