---
id: task-308-02-chest-lock-light-drive
description: "Drive the chest lock light from its state (cpp): bonus chest lock light states, step 02."
parent: task-308-chest-lock-light
kind: task
status: done
---
# Drive the chest lock light from its state

- goal: Extend the state drive of task-302-01 so ARRBonusChest sets LampColor and BlinkRate on the LockLight slot on each state change: Idle red blinking 2 Hz, Hacking orange blinking at a rate that rises linearly with hack progress, 2 Hz at 0 % to 6 Hz at 100 % (default Q17; set on each OnHackProgress; report if the material blink jumps phase when the rate changes, then use steps 2, 3, 4, 6 Hz), Complete green steady (BlinkRate 0); leaving the zone goes back to red 2 Hz; colours, start and end rates EditDefaultsOnly, provisional owner 2026-10-23; spec objective-bonus-chest light line; sys-objectives-console-chest; fix only tests the change breaks.
- agent: cpp
- depends_on: task-308-01-bp-chest-lock-light; task-302-01-console-state-drive
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), objective-bonus-chest (vault/reference/gdd/run/objective-bonus-chest.md), sys-objectives-console-chest (vault/reference/tech/systems/world/sys-objectives/sys-objectives-console-chest.md), bp-bonus-chest (vault/reference/tech/blueprints/world/bp-bonus-chest.md), bp-objective-console (vault/reference/tech/blueprints/world/bp-objective-console.md), color-code (vault/reference/gdd/art-audio/color-code.md), sys-objectives-events (vault/reference/tech/systems/world/sys-objectives/sys-objectives-events.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md); files: Source/RogueRigs/Objectives/; Art/Source/chest/reference/egor-chest-juice.mp4 (owner game-feel reference, technique A)
- expected_output: C++ change, spec and tech doc lines, tests green
- done_when: build green; RogueRigs.Console.* and RogueRigs.RunFlow.* pass; values in the docs; lint clean; committed
- editor_state: closed
- token_ceiling: 1.2 M units
- model: opus
- tokens: input 40, cache write 76353, cache read 1043087, output 17536
- usage: turns 20, context start 12k peak 76k, resumes 0, 7 min, units 0.29 M
- runs: 1
- milestone: M2
- started: 2026-10-29T17:41:31Z
- waiting_for: none
- result: Chest sets LampColor/BlinkRate on LockLight per state (69e4a24c, docs 300eed81). Material blink jumps phase when rate changes (Sine(Time*Rate)), so steps 2,3,4,6 Hz by progress quarter used instead of linear (linear kept if steps emptied). Build green, Console+RunFlow 9 tests pass.

## Relations
- [[task-308-chest-lock-light]] — parent global task.
