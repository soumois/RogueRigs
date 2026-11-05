---
id: task-441-06-doc
description: "Doc: Document the text, the wave, the push and the removed disc."
parent: task-441-level-up-effect
kind: task
status: done
---
# Level-up effect rework: doc

- goal: Document the text, the wave, the push and the removed disc.
- agent: doc
- depends_on: task-441-05-miner-wiring
- inputs: bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), sys-level-up (vault/reference/tech/systems/core/sys-level-up.md), bp-damage-numbers (vault/reference/tech/blueprints/fx/bp-damage-numbers.md), blueprints-fx-index (vault/reference/tech/blueprints/fx/blueprints-fx-index.md)
- expected_output: see done_when
- done_when: Nodes state the current effect, no doc describes the disc as current; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: input 4, cache write 19674, cache read 12780, output 3795
- usage: turns 2, context start 9k peak 23k, resumes 0, 1 min, units 0.04 M
- runs: 1
- milestone: M2
- started: 2026-11-05T20:32:26Z
- waiting_for: none
- result: New bp-level-up-burst; updated bp-miner, sys-level-up, bp-damage-numbers, blueprints-fx-index, sys-tests-progression, sys-tests-weapon-types, unreal-mcp-quirks. No doc reviewed describes the disc as current. Lint clean. Critic style-guide on the LEVEL UP text and T_LevelUpText.png (made in subtask 02, launched late, recorded here): text pass, image pass, 0 fail.

## Relations
- [[task-441-level-up-effect]] — parent global task.
