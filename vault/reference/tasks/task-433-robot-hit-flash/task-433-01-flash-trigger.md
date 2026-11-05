---
id: task-433-01-flash-trigger
description: "Cpp: A robot taking damage from any source raises and clears its flash value at low cost."
parent: task-433-robot-hit-flash
kind: task
status: done
---
# Robot hit flash: cpp

- goal: A robot taking damage from any source raises and clears its flash value at low cost.
- agent: cpp
- depends_on: none
- inputs: readability-rules (vault/reference/gdd/art-audio/readability-rules.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), sys-damage (vault/reference/tech/systems/core/sys-damage.md)
- expected_output: see done_when
- done_when: Build passes and existing enemy and damage tests pass; damage to a robot sets the flash value and it returns to zero after 0.08 s, a new hit restarts it, no change to damage or death; the value is exposed for materials on a slot that does not collide with the animation data.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-433
- usage: included in the executor run of task-433
- runs: 1
- milestone: M2
- started: 2026-11-05T16:05:27Z
- waiting_for: none
- result: ARREnemy writes 1 to Custom Primitive Data 2 (HitFlashDataIndex) of the animated mesh in HandleDamageReceived, timer HitFlashDuration 0.08 s clears it, a new hit restarts it, no tick, cleared at pool reuse. New test RogueRigs.Enemies.HitFlash (chaser, shield-only hit, kamikaze core untouched). Build ok; 27/27 RogueRigs.Enemies, ShieldChaser, DamageNumbers pass.

## Relations
- [[task-433-robot-hit-flash]] — parent global task.
