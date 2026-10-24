---
id: task-303-01-console-xp-burst
description: "Console hack XP burst (cpp): console hack reward: xp burst and big particle, step 01."
parent: task-303-console-hack-reward
kind: task
status: done
---
# Console hack XP burst

- goal: On ARRConsole CompleteHack (console only: ARRBonusChest keeps its own reward), spawn 3 big XP chips worth 8 nuts each, flung in 3 evenly spread yaw directions with the long pickup arc of the existing XP pickups; count and value EditDefaultsOnly, provisional owner 2026-10-23; a Blueprint event the FX can bind on completion if none exists; the chips spawn and land clear of the console mesh (since task-311 the console is 3 x 3 cells of 67 uu, about 201 x 172 x 147 uu at scale 1.361); GDD objective-console reward line; sys-objectives-console-chest and sys-pickups docs; fix only tests the change breaks.
- agent: cpp
- depends_on: task-301-console-c2-mesh (done)
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md), objective-console (vault/reference/gdd/run/objective-console.md), sys-objectives-console-chest (vault/reference/tech/systems/world/sys-objectives/sys-objectives-console-chest.md), bp-objective-console (vault/reference/tech/blueprints/world/bp-objective-console.md), sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), sys-objectives-events (vault/reference/tech/systems/world/sys-objectives/sys-objectives-events.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md); files: Source/RogueRigs/Objectives/, Source/RogueRigs/Pickups/ (check the real folder)
- expected_output: C++ change, GDD and tech doc lines, tests green
- done_when: build green; a test or existing test shows 3 chips of 8 on console complete and none extra on the chest; RogueRigs.Console.* pass; docs updated; lint clean; committed
- editor_state: closed
- token_ceiling: 40000
- model: opus
- tokens: input 72, cache write 178562, cache read 2567133, output 23732
- runs: 1
- milestone: M2
- result: ARRConsole::CompleteHack spawns 3 BP_PickupXPBig chips of 8 XP (EditDefaultsOnly XPBurstChipCount 3, XPBurstChipXP 8, spawn radius 140 uu, 120 deg apart, long arc, rest 220..440 uu); chest count 0; BlueprintImplementableEvent OnHackCompleteBurst(Location, ChipCount); RogueRigs.Console.* 6/6 incl. HackXPBurst; GDD and docs updated (6fcf9754, ab1d2d1e).

## Relations
- [[task-303-console-hack-reward]] — parent global task.
