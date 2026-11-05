---
id: task-446-03-cartoon-ring
description: "Bp: The ground ring has the cartoon look and still grows to the radius; the telegraphs do not change."
parent: task-446-explosions-red
kind: task
status: done
---
# Barrel and kamikaze explosions in cartoon style: bp

- goal: The ground ring has the cartoon look and still grows to the radius; the telegraphs do not change.
- agent: bp
- depends_on: task-446-02-cartoon-sphere
- inputs: bp-explosion-shockwave (vault/reference/tech/blueprints/fx/bp-explosion-shockwave.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md), ground-telegraph (vault/reference/gdd/art-audio/ground-telegraph.md)
- expected_output: see done_when
- done_when: Material compiles with no error; Radius, Progress and Opacity still drive it; telegraph material and instances untouched; explosion tests pass.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-446
- usage: included in the executor run of task-446
- runs: 1
- milestone: M2
- started: 2026-11-05T22:16:57Z
- waiting_for: none
- result: M_ExplosionShockwave graph replaced by one Custom node (float4): ring whose outer edge is at Radius x Progress with a jag of at most 3 % inward, width RingWidth x (1.35 to 2.25) jagged, three flat bands from outside in red-orange, orange, yellow (same colours as the sphere); during the fade (Opacity 1 to 0) the bands cool one step towards dark red and the width thins to 40 %; alpha = ring x step(Progress) x Opacity. Radius, Progress, Opacity and RingWidth still drive it; ZoneColor, ProgressColor, FillOpacity and the three old Custom nodes removed, MI_ExplosionShockwave stale colour overrides cleared. Material recompiles with no error. M_GroundTelegraph and its instances untouched (no diff). RogueRigs.Explosions: 10 of 10 passed (-nullrhi). Commit 32d096bf. Look unverified in game (A8).

## Relations
- [[task-446-explosions-red]] — parent global task.
