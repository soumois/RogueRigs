---
id: task-206-01-run-state
description: "cpp: URRRunSubsystem."
parent: task-206-level-to-level
kind: task
status: done
---
# run-state

- goal: URRRunSubsystem (GameInstance subsystem): run level index 1 to 4, list of run layouts (arena_m1_fine, arena_l2, arena_l3, arena_l4) as data, snapshot of the miner at level end (upgrade cards, weapons, level, XP, health, ore) and its re-application to the new miner at level start; PIE or -game on a map with no run started begins level 1 with the grid LayoutName.
- agent: cpp
- depends_on: none
- inputs: run-structure (vault/reference/gdd/run/run-structure.md), intermission (vault/reference/gdd/run/intermission.md), sys-objectives-run-flow (vault/reference/tech/systems/world/sys-objectives/sys-objectives-run-flow.md), sys-objectives-ore-counter (vault/reference/tech/systems/world/sys-objectives/sys-objectives-ore-counter.md), sys-player (vault/reference/tech/systems/core/sys-player.md), sys-gas-core (vault/reference/tech/systems/core/sys-gas-core.md), sys-progression (vault/reference/tech/systems/core/sys-progression.md), sys-level-up (vault/reference/tech/systems/core/sys-level-up.md), sys-upgrades (vault/reference/tech/systems/core/sys-upgrades.md), sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md), conventions-index (vault/reference/tech/conventions/conventions-index.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ in Source/RogueRigs/ (new Run/ folder), new doc node sys-run in tech/systems/world/ linked from systems-world-index.
- done_when: build succeeds; existing automation tests pass (only expectations broken by this change updated); snapshot then re-apply restores the listed state (shown in the agent report from a -game log); sys-run doc written; committed.
- editor_state: closed
- token_ceiling: 80000
- model: opus
- tokens: input 100, cache write 119104, cache read 4054284, output 16616
- runs: 1
- milestone: M2
- result: URRRunSubsystem (c84abebb, 17431cb2, 5b25b562): level 1-4, layout list in URRRunSettings (C++ defaults), snapshot (cards in order, weapon classes, level, XP, health, recap ore) re-applied by replaying cards through ApplyUpgrade after RestoreProgress; ore carried as secured ore in URROreBankSubsystem; direct PIE/-game starts level 1 with the grid LayoutName; debug rr.Run.Status, rr.Run.NextLevel. -game log: snapshot applied on levels 2 and 3 (level 5, 75/100 HP, 2 weapons, 4/4 cards). Tests 260 pass, 2 fail also alone and unrelated to run code (Enemies.ShotHitsMovingRobotAndDeathShowsParts EyeMesh, Integration.JuiceHealRing pop distance since task-195-02); left unchanged. Open for 206-03: ResetRun on loss or won run; grid still loads its own LayoutName.

## Relations
- [[task-206-level-to-level]] — parent global task.
