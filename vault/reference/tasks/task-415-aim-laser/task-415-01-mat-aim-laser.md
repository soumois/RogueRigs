---
id: task-415-01-mat-aim-laser
description: "Bp: create the M_AimLaser material (white-gold core, soft glow, tip fade) in Content/RogueRigs/FX/"
parent: task-415-aim-laser
kind: task
status: done
---
# Aim laser replaces the white aim chevrons: bp

- goal: Create the material /Game/RogueRigs/FX/M_AimLaser for the aim laser drawn on the engine Plane mesh (/Engine/BasicShapes/Plane, 100 x 100 uu; UV.x along the length from the Muzzle (0) to the tip (1), UV.y across): Surface, Translucent (or Additive if it reads better over flashes), Unlit, two-sided, no depth write. Parameters with these exact names and defaults: Color (vector, linear 1.0, 0.84, 0.40), EmissiveGain (1.0), Opacity (1.0), CoreFrac (0.16 = 3 / 19 uu, the centred bright core fraction of the width, edges one-pixel anti-aliased, never blurry), GlowOpacity (0.3, halo opacity outside the core falling smoothly to 0 at the edges), TipFadeFrac (0.15, the last fraction of UV.x fading linearly to 0). One main asset, its own commit; no other asset touched.
- agent: bp
- depends_on: none
- inputs: task-415-aim-laser (vault/reference/tasks/task-415-aim-laser.md), aim-indicator (vault/reference/gdd/art-audio/aim-indicator.md), color-code (vault/reference/gdd/art-audio/color-code.md), sys-aim-indicator (vault/reference/tech/systems/combat/sys-aim-indicator.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), `Art/Source/aim-indicator/laser-reference-2026-11-02/sheet.png`
- expected_output: Content/RogueRigs/FX/M_AimLaser.uasset committed; change summary: blend mode, parameter names and defaults, UV convention, commit hash.
- done_when: M_AimLaser exists, compiles without error, exposes Color, EmissiveGain, Opacity, CoreFrac, GlowOpacity, TipFadeFrac with the defaults above; committed by path as `task-415-01-mat-aim-laser: <summary>`.
- editor_state: Unreal editor open
- token_ceiling: 1.0 M units
- model: opus+sonnet
- tokens: input 56, cache write 108244, cache read 1090369, output 17497
- usage: turns 29, context start 12k peak 66k, resumes 0, 3 min, units 0.33 M
- runs: 1
- milestone: M3
- started: 2026-11-02T17:29:01Z
- waiting_for: none
- result: M_AimLaser created (Translucent, Unlit, two-sided; 6 params with defaults; commit 1700f6ff); render unverified

## Relations
- [[task-415-aim-laser]] — parent global task.
