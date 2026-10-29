---
id: task-309-01-chest-opening-sequence
description: "Chest opening sequence, slam, light drive and XP burst (cpp): bonus chest opening, step 01."
parent: task-309-chest-opening
kind: task
status: done
---
# Chest opening sequence, slam, light drive and XP burst

- goal: On ARRBonusChest CompleteHack: rotate the Latch component about its top edge (90 deg in 0.2 s); 0.5 s after completion rotate the Lid about its hinge axis to 115 deg in 0.4 s (ease-out), hold it 0.06 s (chest only: no global time dilation, the rest of the game runs), then settle to 100 deg in 0.15 s and stay open (Q10, Q18, Q19); at the hold start fire a BlueprintImplementableEvent OnLidSlam (ground ring), at its end play the light shake of task-310-01; at lid opening fire OnLidOpened (rays and sparks), switch on a C++ UPointLightComponent InnerLight (off by default) and a scalar InteriorGlow on the Interior slot, fade the light out after 3 s and keep InteriorGlow on (Q22); grant the reward at lid opening: the big XP chip burst of task-303-01 with 3 chips of 8 nuts, each chip scaling 0 to 1.3 to 1.0 in 0.25 s on spawn (chest burst only, console unchanged, Q25), the 5 ore chunks and the one upgrade choice; the rotation signs about X that open the lid up and backward and swing the latch outward and up are taken from the task-309-07 report (unmeasured until then, owner 2026-10-24); every angle, time, scale and intensity EditDefaultsOnly, provisional owner 2026-10-23; spec objective-bonus-chest opening and reward lines; sys-objectives-console-chest and sys-pickups docs; fix only tests the change breaks.
- agent: cpp
- depends_on: task-306-03-bp-chest-import; task-309-07-chest-hinge-recut; task-307-01-chest-one-upgrade; task-303-01-console-xp-burst; task-310-01-camera-shake
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), objective-bonus-chest (vault/reference/gdd/run/objective-bonus-chest.md), sys-objectives-console-chest (vault/reference/tech/systems/world/sys-objectives/sys-objectives-console-chest.md), bp-bonus-chest (vault/reference/tech/blueprints/world/bp-bonus-chest.md), sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), sys-pickups-xp (vault/reference/tech/systems/world/sys-pickups/sys-pickups-xp.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), sys-objectives-events (vault/reference/tech/systems/world/sys-objectives/sys-objectives-events.md), sys-camera-shake (vault/reference/tech/systems/core/sys-camera-shake.md, written by task-310-01), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md); files: Art/Source/chest/clean/chest_cut.report.json (task-309-07 pivots and opening signs), Source/RogueRigs/Objectives/RRBonusChest.h, Source/RogueRigs/Objectives/RRBonusChest.cpp, Source/RogueRigs/Objectives/RRConsole.h; Art/Source/chest/reference/egor-chest-juice.mp4 (owner reference, techniques D, E, H)
- expected_output: C++ change, spec and tech doc lines, tests green
- done_when: build green; RogueRigs.Console.* and RogueRigs.RunFlow.* pass; chest reward gives 3 chips of 8 and one choice, after the lid delay; no time dilation call; console burst unchanged; docs updated; lint clean; committed
- editor_state: closed
- token_ceiling: 1.2 M units
- model: opus
- tokens: input 118, cache write 196032, cache read 7834521, output 70417
- usage: turns 59, context start 12k peak 196k, resumes 0, 15 min, units 1.38 M
- runs: 1
- milestone: M2
- started: 2026-10-29T18:41:54Z
- result: Done: commits 15e1bbf1 (code), 944fd76a (docs). Sequence timed in ARRBonusChest tick; OnLidSlam/OnLidOpened; InnerLight C++ component; InteriorGlow param; chips scale pop; lid/latch RollSign -1 (derived, verify in editor). Console+RunFlow 9/9, pickups 19/19.

## Relations
- [[task-309-chest-opening]] — parent global task.
