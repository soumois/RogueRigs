---
id: task-469-01-green
description: "Bp: Aim laser colour set to the lime green of the owner capture, look otherwise unchanged."
parent: task-469-aim-laser-green
kind: task
status: done
---
# Aim laser in the lime green of the owner capture: bp

- goal: Aim laser colour set to the lime green of the owner capture, look otherwise unchanged.
- agent: bp
- depends_on: none
- inputs: aim-indicator (vault/reference/gdd/art-audio/aim-indicator.md), sys-aim-indicator (vault/reference/tech/systems/combat/sys-aim-indicator.md), color-code (vault/reference/gdd/art-audio/color-code.md), Art/Source/aim-indicator/owner-capture-green-unlimited-2026-11-06.webp
- expected_output: see done_when
- done_when: Acceptance A1 to A3 hold in PIE or -game: core and glow in the lime green sampled from Art/Source/aim-indicator/owner-capture-green-unlimited-2026-11-06.webp, width, glow, brightness profile, height and length rules unchanged; Shooter still distinct; style-guide pass against the owner capture; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: input 4, cache write 17517, cache read 13222, output 4165
- usage: turns 2, context start 9k peak 21k, resumes 0, 1 min, units 0.04 M
- runs: 1
- milestone: M2
- started: 2026-11-06T10:29:46Z
- waiting_for: none
- result: Done in C++ (not the editor): URRAimIndicatorComponent Color default linear (0.35, 1.0, 0.02) = sRGB (160, 255, 40), the lime body sampled from the owner capture (glow (90,191,0)-(119,204,0), body (167,255,70)-(233,255,88), core near-white yellow); BP_Miner overrides none; width, CoreFrac, GlowOpacity, EmissiveGain 1, height, length unchanged; M_AimLaser untouched; build only, succeeded (commit 90c21abd). style-guide: colour matches the capture (pass); Shooter distinction open (no Shooter in the reference, hue close to acid green); fail risk: a single colour at gain 1 may give a flat lime core, not the pale core of the capture (unmeasured, needs the owner's in-game look; fix would be a core colour or core gain); fail: readability-rules (kind rule, not editable by the executor) still says the aim laser is white-gold. No capture (owner): look unverified.

## Relations
- [[task-469-aim-laser-green]] — parent global task.
