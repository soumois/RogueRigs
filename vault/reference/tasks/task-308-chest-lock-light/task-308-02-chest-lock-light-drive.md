---
id: task-308-02-chest-lock-light-drive
description: "Drive the chest lock light from its state (cpp): bonus chest lock light states, step 02."
parent: task-308-chest-lock-light
kind: task
status: todo
---
# Drive the chest lock light from its state

- goal: Extend the state drive of task-302-01 so ARRBonusChest sets LampColor and BlinkRate on the LockLight slot on each state change: Idle red blinking 2 Hz, Hacking orange blinking 2 Hz, Complete green steady (BlinkRate 0); colours and rate EditDefaultsOnly, provisional owner 2026-10-23; spec objective-bonus-chest light line; sys-objectives-console-chest; fix only tests the change breaks.
- agent: cpp
- depends_on: task-308-01-bp-chest-lock-light; task-302-01-console-state-drive
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), objective-bonus-chest (vault/reference/gdd/run/objective-bonus-chest.md), sys-objectives-console-chest (vault/reference/tech/systems/world/sys-objectives/sys-objectives-console-chest.md), bp-bonus-chest (vault/reference/tech/blueprints/world/bp-bonus-chest.md), bp-objective-console (vault/reference/tech/blueprints/world/bp-objective-console.md), color-code (vault/reference/gdd/art-audio/color-code.md), sys-objectives-events (vault/reference/tech/systems/world/sys-objectives/sys-objectives-events.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md); files: Source/RogueRigs/Objectives/
- expected_output: C++ change, spec and tech doc lines, tests green
- done_when: build green; RogueRigs.Console.* and RogueRigs.RunFlow.* pass; values in the docs; lint clean; committed
- editor_state: closed
- token_ceiling: 35000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-308-chest-lock-light]] — parent global task.
