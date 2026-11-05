---
id: task-441-02-text-asset
description: "Bp: The text renders as gold "LEVEL UP" with the pop and no arc."
parent: task-441-level-up-effect
kind: task
status: in-progress
---
# Level-up effect rework: bp

- goal: The text renders as gold "LEVEL UP" with the pop and no arc.
- agent: bp
- depends_on: task-441-01-level-up-text
- inputs: bp-damage-numbers (vault/reference/tech/blueprints/fx/bp-damage-numbers.md), color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: see done_when
- done_when: Asset compiles with no error, letters read back as available to the render path, gold value set, normal numbers still use their arc.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-05T20:10:26Z
- waiting_for: none
- result: open

## Relations
- [[task-441-level-up-effect]] — parent global task.
