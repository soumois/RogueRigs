---
id: task-309-01-chest-opening-sequence
description: "Chest opening sequence and XP burst (cpp): bonus chest opening: latch, lid, particle and xp burst, step 01."
parent: task-309-chest-opening
kind: task
status: todo
---
# Chest opening sequence and XP burst

- goal: On ARRBonusChest CompleteHack: rotate the Latch component about its top edge (90 deg in 0.2 s), then at 0.5 s after completion rotate the Lid about its hinge axis (100 deg in 0.4 s, stays open); grant the reward at lid opening instead of at completion: the big XP chip burst of task-303-01 enabled on the chest with 3 chips of 8 nuts, the 5 ore chunks and the one upgrade choice; angles, durations, delay and chip count EditDefaultsOnly, provisional owner 2026-10-23; a BlueprintImplementableEvent OnLidOpened for the particle; spec objective-bonus-chest opening and reward lines; sys-objectives-console-chest and sys-pickups docs; fix only tests the change breaks.
- agent: cpp
- depends_on: task-306-03-bp-chest-import; task-307-01-chest-one-upgrade; task-303-01-console-xp-burst
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), objective-bonus-chest (vault/reference/gdd/run/objective-bonus-chest.md), sys-objectives-console-chest (vault/reference/tech/systems/world/sys-objectives/sys-objectives-console-chest.md), bp-bonus-chest (vault/reference/tech/blueprints/world/bp-bonus-chest.md), sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), sys-pickups-xp (vault/reference/tech/systems/world/sys-pickups/sys-pickups-xp.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), sys-objectives-events (vault/reference/tech/systems/world/sys-objectives/sys-objectives-events.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md); files: Source/RogueRigs/Objectives/RRBonusChest.h, Source/RogueRigs/Objectives/RRBonusChest.cpp, Source/RogueRigs/Objectives/RRConsole.h
- expected_output: C++ change, spec and tech doc lines, tests green
- done_when: build green; RogueRigs.Console.* and RogueRigs.RunFlow.* pass; chest reward gives 3 chips of 8 and one choice, after the lid delay; docs updated; lint clean; committed
- editor_state: closed
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-309-chest-opening]] — parent global task.
