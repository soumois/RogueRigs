---
id: task-456-02-ghost-bar
description: "Bp: The XP widget gets the ghost bar behind the fill, in the retained look."
parent: task-456-xp-gain-ghost
kind: task
status: in-progress
---
# XP gain ghost on the XP bar: bp

- goal: The XP widget gets the ghost bar behind the fill, in the retained look.
- agent: bp
- depends_on: task-456-01-ghost-logic
- inputs: wbp-xp-status (vault/reference/tech/blueprints/ui/wbp-xp-status.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Art/Source/hud-unified/
- expected_output: see done_when
- done_when: Compiled and saved; ghost driven by the C++; PIE XP gain and level-up show no runtime error.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-06T01:07:07Z
- waiting_for: none
- result: open

## Relations
- [[task-456-xp-gain-ghost]] — parent global task.
