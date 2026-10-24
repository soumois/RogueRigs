---
id: task-307-01-chest-one-upgrade
description: "Drop the chest direct XP grant (cpp): bonus chest gives one upgrade, not two, step 01."
parent: task-307-chest-one-upgrade
kind: task
status: done
---
# Drop the chest direct XP grant

- goal: Set the ARRBonusChest XPNeedFraction default to 0 so GrantReward gives no direct XP (property kept, header comment history kept, AddXP skipped at 0), the reward stays 5 ore chunks and one QueueBonusChoice(3); OnRewardGranted signature unchanged (XP 0); update the spec reward line of objective-bonus-chest (owner 2026-10-23: one upgrade, not two; was 50 % of the level need) and sys-objectives-console-chest; fix only the tests this change breaks.
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), objective-bonus-chest (vault/reference/gdd/run/objective-bonus-chest.md), sys-objectives-console-chest (vault/reference/tech/systems/world/sys-objectives/sys-objectives-console-chest.md), bp-bonus-chest (vault/reference/tech/blueprints/world/bp-bonus-chest.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md), sys-level-up (vault/reference/tech/systems/core/sys-level-up.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), sys-tests-progression (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-progression.md), sys-tests-console-extraction (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-console-extraction.md); files: Source/RogueRigs/Objectives/RRBonusChest.h, Source/RogueRigs/Objectives/RRBonusChest.cpp; Source/RogueRigs/Progression/RRLevelUpComponent.cpp (QueueBonusChoice)
- expected_output: C++ default changed, spec and tech doc lines, tests green
- done_when: build green; RogueRigs.RunFlow.*, RogueRigs.Console.* and RogueRigs.Progression.* (or the matching suites) pass; a chest reward with the bar above half queues one choice only (existing test or log check); docs updated; lint clean; committed
- editor_state: closed
- token_ceiling: 30000
- model: opus
- tokens: input 38, cache write 47210, cache read 573680, output 9487
- runs: 1
- milestone: M2
- result: Bonus chest XPNeedFraction default 0 (AddXP skipped at 0): reward is 5 ore chunks + one +3-luck choice, no level-up from the chest; HackXPBurst asserts one choice with bar at 75 %; spec and docs updated; build green, RunFlow/Console/Progression 13/13 pass; commit 15c2f58c

## Relations
- [[task-307-chest-one-upgrade]] — parent global task.
