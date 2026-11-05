---
id: task-441-05-miner-wiring
description: "Bp: BP_Miner plays the wave on level-up and no longer has the cyan disc; the heal ring stays."
parent: task-441-level-up-effect
kind: task
status: todo
---
# Level-up effect rework: bp

- goal: BP_Miner plays the wave on level-up and no longer has the cyan disc; the heal ring stays.
- agent: bp
- depends_on: task-441-04-floor-wave
- inputs: bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), bp-heal-ring (vault/reference/tech/blueprints/fx/bp-heal-ring.md), sys-level-up (vault/reference/tech/systems/core/sys-level-up.md)
- expected_output: see done_when
- done_when: Compiles and saves; no disc component or disc timer left; level-up in PIE logs no runtime error; heal ring still spawned; the owner's own light values in BP_Miner are untouched and any unsaved editor change of his is not lost.
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
- [[task-441-level-up-effect]] — parent global task.
