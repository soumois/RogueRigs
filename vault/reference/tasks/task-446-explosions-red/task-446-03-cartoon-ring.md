---
id: task-446-03-cartoon-ring
description: "Bp: The ground ring has the cartoon look and still grows to the radius; the telegraphs do not change."
parent: task-446-explosions-red
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-446-explosions-red]] — parent global task.
